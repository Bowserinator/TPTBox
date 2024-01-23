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

const int MAX_RAY_STEPS = 712;
const bool DEBUG_CASTS = false;
const float SIMBOX_CAST_PAD = 0.999; // Casting directly on the surface of the sim box (pad=1.0) leads to "z-fighting"

bool isInSim(vec3 c) {
    return all(greaterThanEqual(c, vec3(0.0))) && all(lessThan(c, simRes));
}

bool getVoxel(vec3 c) {
    c = c + vec3(0.5);

    // If statements are required to prevent incorrect projections near sim bound faces
    // if (c.x < 1 || c.y < 1 || c.z < 1) return false;
    // if (c.x >= simRes.x - 1 || c.y >= simRes.y - 1 || c.z >= simRes.z - 1) return false;

	return texture(colors, vec3(c / simRes)).r > 0.0;
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

    // if ((b < 0) || (a > b)) // Collision missed, should not be possible
    //     return false;

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
    int steps = 0;
	for (steps = 0; steps < MAX_RAY_STEPS; steps++) {
        if (!isInSim(mapPos)) {
            if (!DEBUG_CASTS) steps = MAX_RAY_STEPS;
            break;
        }
		if (getVoxel(mapPos)) break;

        mask = lessThanEqual(sideDist.xyz, min(sideDist.yzx, sideDist.zxy));
        
        // All components of mask are false except for the corresponding largest component
        // of sideDist, which is the axis along which the ray should be incremented.			
        sideDist += vec3(mask) * deltaDist;
        mapPos += vec3(mask) * rayStep;
	}

    if (!DEBUG_CASTS)
        if (steps == MAX_RAY_STEPS) {
            FragColor = vec4(0.0);
            return;
        }

	vec3 color;
    if (DEBUG_CASTS)
        color = 200 * vec3(float(steps) / float(MAX_RAY_STEPS));
    else {
        if (mask.x)
            color = vec3(0.5);
        if (mask.y)
            color = vec3(1.0);
        if (mask.z)
            color = vec3(0.75);
    }
    FragColor.rgb = color;
    FragColor.a = 1.0;
}