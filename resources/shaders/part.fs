#version 430

layout (binding = 0) uniform sampler3D colors;

uniform vec2 resolution;  // Viewport res
uniform mat4 mvp;         // Transform matrix
uniform vec3 cameraPos;   // Camera position in world space
uniform vec3 cameraDir;   // Camera look dir (normalized)

uniform vec3 uv1;         // "Up" direction on screen vector mapped to world space
uniform vec3 uv2;         // "Right" direction on screen vector mapped to world space
uniform vec3 simRes;      // Vec3 of XRES, YRES, ZRES

out vec4 FragColor;

const float DEG2RAD = 3.141592 / 180.0;
const float FOV = 45; // FOV in degrees

const float FOG_START_PERCENT = 0.75;   // After this percentage of max ray steps begins to fade to black
const float ALPHA_THRESH = 0.96;        // Above this alpha a ray is considered "stopped"
const float AIR_INDEX_REFRACTION = 1.0; // Note: can't be 0

const int MAX_RAY_STEPS = 256 * 3;
const bool DEBUG_CASTS = false;
const float SIMBOX_CAST_PAD = 0.999; // Casting directly on the surface of the sim box (pad=1.0) leads to "z-fighting"

bool isInSim(vec3 c) {
    return all(greaterThanEqual(c, vec3(0.0))) && all(lessThan(c, simRes));
}

// Collide ray with sim bounding cube, rayDir should be normalized
vec3 rayCollideSim(vec3 rayPos, vec3 rayDir) {
    vec3 v = vec3(1.0) / rayDir;
    float xmin = (SIMBOX_CAST_PAD     - rayPos.x)        * v.x; // 0.0 = XMIN
    float xmax = (simRes.x - rayPos.x - SIMBOX_CAST_PAD) * v.x; // XRES = XMAX
    float ymin = (SIMBOX_CAST_PAD     - rayPos.y)        * v.y; // YMIN
    float ymax = (simRes.y - rayPos.y - SIMBOX_CAST_PAD) * v.y; // YMAX
    float zmin = (SIMBOX_CAST_PAD     - rayPos.z)        * v.z; // ZMIN
    float zmax = (simRes.z - rayPos.z - SIMBOX_CAST_PAD) * v.z; // ZMAX
    float a = max(max(min(xmin, xmax), min(ymin, ymax)), min(zmin, zmax));
    float b = min(min(max(xmin, xmax), max(ymin, ymax)), max(zmin, zmax));

    // We do not need to check whether we missed since whatever result
    // will be out of the simulation and the loop will immedately terminate
    vec3 collisionPoint = rayPos + rayDir * a;
    return collisionPoint;
}


// Modified from https://www.shadertoy.com/view/4dX3zl
void main() {
    // Normalized to 0, 0 = center, scale -1 to 1
    vec2 screenPos = (gl_FragCoord.xy / resolution.xy) * 2.0 - 1.0;
	vec3 rayDir = cameraDir + tan(FOV / 2 * DEG2RAD) * (screenPos.x * uv1 + screenPos.y * uv2);

    vec3 rayPos = cameraPos;
    vec3 mapPos = vec3(floor(rayPos + 0.0));

    // If not in sim bounding box project to nearest face on ray bounding box
    if (!isInSim(mapPos)) {
        rayPos = rayCollideSim(rayPos, rayDir);
        mapPos = vec3(floor(rayPos + 0.0));
    }

	vec3 deltaDist = abs(vec3(1.0) / rayDir);
	vec3 rayStep   = vec3(sign(rayDir));
	vec3 sideDist  = (sign(rayDir) * (vec3(mapPos) - rayPos) + (sign(rayDir) * 0.5) + 0.5) * deltaDist;

	bvec3 mask;
    vec3 maskv;
    int steps = 0;

    vec4 forward_color = vec4(0.0); // Starts empty, can accumulate transparency
    vec4 voxel_color;
    float prev_index_of_refraction = AIR_INDEX_REFRACTION;

	for (steps = 0; steps < MAX_RAY_STEPS; steps++) {
        if (!isInSim(mapPos))
            break;
        
        voxel_color = texture(colors, vec3((mapPos + vec3(0.5)) / simRes));
		if (voxel_color.a > 0.0) {
            float forward_alpha_inv = 1.0 - forward_color.a;
            forward_color.rgb += voxel_color.rgb * voxel_color.a * forward_alpha_inv;
            forward_color.a = 1.0 - forward_alpha_inv * (1.0 - voxel_color.a);

            if (forward_color.a > ALPHA_THRESH)
                break;
        }

        // Reflection
        bool should_reflect = voxel_color.a < 0.1 && voxel_color.a > 0.0; // TODO texture

        if (should_reflect) {
            // Undo previous move
            mapPos -= maskv * rayStep;
            sideDist -= maskv * deltaDist;

            vec3 normal = maskv * -rayStep;
            rayDir = reflect(rayStep, normal);
            rayStep = vec3(sign(rayDir));
        }

        // Refraction
        // float index_of_refraction = voxel_color.a < 0.5 ? 0.5 : AIR_INDEX_REFRACTION; // TODO use a texture to get
        // if (voxel_color.a == 0.0)
        //     index_of_refraction = AIR_INDEX_REFRACTION;

        // if (abs(index_of_refraction - prev_index_of_refraction) > 0.001) {
        //     vec3 normal = maskv * -rayStep;
        //     rayDir = refract(-rayStep, normal, index_of_refraction / prev_index_of_refraction);
        //     rayStep = vec3(sign(rayDir));
        //     prev_index_of_refraction = index_of_refraction;
        // }

        // DDA
        mask = lessThanEqual(sideDist.xyz, min(sideDist.yzx, sideDist.zxy));
        maskv = vec3(mask);
        
        // All components of mask are false except for the corresponding largest component
        // of sideDist, which is the axis along which the ray should be incremented.			
        sideDist += maskv * deltaDist;
        mapPos += maskv * rayStep;
	}

    if (!DEBUG_CASTS)
        if (forward_color.a == 0.0) {
            FragColor = vec4(0.0);
            return;
        }

    if (DEBUG_CASTS) {
        FragColor = vec4(float(steps) / float(MAX_RAY_STEPS));
        FragColor.a = 1.0;
    }
    else {
        float mul = 0.0;
        if (mask.x)
            mul = 0.5;
        if (mask.y)
            mul = 1.0;
        if (mask.z)
            mul = 0.75;
        if (steps > FOG_START_PERCENT * MAX_RAY_STEPS)
            mul *= 1.0 - (steps - MAX_RAY_STEPS * FOG_START_PERCENT) / (MAX_RAY_STEPS * (1 - FOG_START_PERCENT));
        mul *= forward_color.a;

        FragColor.a = 1.0;
        FragColor.rgb = mul * forward_color.rgb;
    }
}