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

const int MAX_RAY_STEPS = 512;

bool isInSim(vec3 c) {
    if (c.x < 0 || c.y < 0 || c.z < 0) return false;
    if (c.x > simRes.x || c.y > simRes.y || c.z > simRes.z) return false;
    return true;
}

bool getVoxel(vec3 c) {
    if (c.x < 1 || c.y < 1 || c.z < 1) return false;
    if (c.x >= simRes.x - 1 || c.y >= simRes.y - 1 || c.z >= simRes.z - 1) return false;

	return isInSim(c) && 
        texture(colors, vec3(
            (c.x + 0.5) / simRes.x,
            (c.y + 0.5) / simRes.y, 
            (c.z + 0.5) / simRes.z)
        ).r > 0.0;
}

// Collide ray with sim bounding cube, rayDir should be normalized
vec3 rayCollideSim(vec3 rayPos, vec3 rayDir) {
    vec3 v = 1.0 / rayDir;
    float xmin = (1.0            - rayPos.x) * v.x; // 1.0 = XMIN
    float xmax = (simRes.x - 1.0 - rayPos.x) * v.x; // XRES = XMAX
    float ymin = (1.0            - rayPos.y) * v.y; // YMIN
    float ymax = (simRes.y - 1.0 - rayPos.y) * v.y; // YMAX
    float zmin = (1.0            - rayPos.z) * v.z; // ZMIN
    float zmax = (simRes.z - 1.0 - rayPos.z) * v.z; // ZMAX
    float a = max(max(min(xmin, xmax), min(ymin, ymax)), min(zmin, zmax));
    float b = min(min(max(xmin, xmax), max(ymin, ymax)), max(zmin, zmax));

    // if ((b < 0) || (a > b)) // Collision missed, should not be possible
    //     return false;

    vec3 collisionPoint = rayPos + rayDir * a;
    return collisionPoint;
}


// Modified from https://www.shadertoy.com/view/4dX3zl
void main() {
    FragColor.a = 1.0;

    // Normalized to 0, 0 = center, scale -1 to 1
    vec2 screenPos = (gl_FragCoord.xy / resolution.xy) * 2.0 - 1.0;
	vec3 U = uv1;
	vec3 V = uv2 * resolution.y / resolution.x;
	vec3 rayDir = cameraDir + 0.8 * (screenPos.x * U + screenPos.y * V);

    vec3 rayPos = cameraPos;
    vec3 mapPos = vec3(floor(rayPos + 0.));

    // If not in sim bounding box project to nearest face on ray bounding box
    if (!isInSim(mapPos)) {
        rayPos = rayCollideSim(mapPos, rayDir);
        mapPos = vec3(floor(rayPos + 0.));
    }

	vec3 deltaDist = abs(vec3(length(rayDir)) / rayDir);
	vec3 rayStep   = vec3(sign(rayDir));
	vec3 sideDist  = (sign(rayDir) * (vec3(mapPos) - rayPos) + (sign(rayDir) * 0.5) + 0.5) * deltaDist;

	bvec3 mask;
    int steps = 0;
	for (steps = 0; steps < MAX_RAY_STEPS; steps++) {
		if (getVoxel(mapPos)) break;
        if (!isInSim(mapPos)) {
            steps = MAX_RAY_STEPS;
            break;
        }

        mask = lessThanEqual(sideDist.xyz, min(sideDist.yzx, sideDist.zxy));
        
        // All components of mask are false except for the corresponding largest component
        // of sideDist, which is the axis along which the ray should be incremented.			
        sideDist += vec3(mask) * deltaDist;
        mapPos += vec3(mask) * rayStep;
	}

    if (steps == MAX_RAY_STEPS)
        discard;

	vec3 color;
	if (mask.x)
		color = vec3(0.5);
	if (mask.y)
		color = vec3(1.0);
	if (mask.z)
		color = vec3(0.75);
    // color = vec3(float(steps) / float(MAX_RAY_STEPS));
    FragColor.rgb = color;
}