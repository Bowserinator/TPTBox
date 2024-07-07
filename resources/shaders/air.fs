#version 430

layout(std430, binding = 0) readonly restrict buffer VX { float vx[]; };
layout(std430, binding = 1) readonly restrict buffer VY { float vy[]; };
layout(std430, binding = 2) readonly restrict buffer VZ { float vz[]; };

layout(shared, binding = 3) uniform Constants {
    ivec4 AIRRES;
    vec4 SIMRES;
    int CELL_SIZE;
    float FOV_DIV2;
    vec4 VIEW_SLICE_BEGIN; // xyz
    vec4 VIEW_SLICE_END;   // xyz
};

uniform vec2 resolution;  // Viewport res
uniform mat4 mvp;         // Transform matrix
uniform vec3 cameraPos;   // Camera position in world space
uniform vec3 cameraDir;   // Camera look dir (normalized)

uniform vec3 uv1;         // "Up" direction on screen vector mapped to world space
uniform vec3 uv2;         // "Right" direction on screen vector mapped to world space

layout (location = 0) out vec4 FragColor;

const float SIMBOX_CAST_PAD = 0.999; // Casting directly on the surface of the sim box (pad=1.0) leads to "z-fighting"
const int MAX_RAY_STEPS = 180;
const float MAX_VEL_SCALE = 0.1;

// Functions
// ---------
float min3(vec3 v) { return min(min(v.x,v.y), v.z); }
float max3(vec3 v) { return max(max(v.x,v.y), v.z); }

bool isInSim(vec3 c) {
    return clamp(c, vec3(0.0), SIMRES.xyz - vec3(SIMBOX_CAST_PAD)) == c;
}

bool isInView(vec3 c) {
    return clamp(c, VIEW_SLICE_BEGIN.xyz, VIEW_SLICE_END.xyz) == c;
}

// Collide ray with sim bounding cube, rayDir should be normalized
vec3 rayCollideSim(vec3 rayPos, vec3 rayDir) {
    vec3 v = vec3(1.0) / rayDir;
    vec3 boundMin = (vec3(SIMBOX_CAST_PAD) - rayPos) * v;
    vec3 boundMax = (SIMRES.xyz - vec3(SIMBOX_CAST_PAD) - rayPos) * v;
    float a = max3(min(boundMin, boundMax));
    float b = min3(max(boundMin, boundMax));

    if (b < 0 || a > b) // Failed to hit box, needs to check when very close to sim edge
        return vec3(-1);

    vec3 collisionPoint = rayPos + rayDir * a;
    return collisionPoint;
}

// Return color from pressure data
vec4 raymarch(vec3 pos, vec3 dir, bool is_vel) {
    vec4 color = vec4(0.0);
    pos /= CELL_SIZE;
    ivec3 voxelPos = ivec3(pos);

	vec3 deltaDist = abs(vec3(1.0) / dir);
	ivec3 rayStep = ivec3(sign(dir));
	vec3 sideDist = (sign(dir) * (vec3(voxelPos) - pos) + (sign(dir) * 0.5) + 0.5) * deltaDist; 
	bvec3 mask;

    for (int i = 0; i < MAX_RAY_STEPS; i++) {
		if (!isInSim(voxelPos * CELL_SIZE)) return color;
        if (isInView(voxelPos * CELL_SIZE)) {
            ivec3 airGridPos = voxelPos;
            float v_x = vx[airGridPos.x + AIRRES.x * airGridPos.y + AIRRES.x * AIRRES.y * airGridPos.z];
            float v_y = vy[airGridPos.x + AIRRES.x * airGridPos.y + AIRRES.x * AIRRES.y * airGridPos.z];
            float v_z = vz[airGridPos.x + AIRRES.x * airGridPos.y + AIRRES.x * AIRRES.y * airGridPos.z];

            // float v_x = vx[airGridPos.x + 1 + AIRRES.x * airGridPos.y + AIRRES.x * AIRRES.y * airGridPos.z] -
            //             vx[airGridPos.x + AIRRES.x * airGridPos.y + AIRRES.x * AIRRES.y * airGridPos.z];
            // float v_y = vy[airGridPos.x + AIRRES.x * (airGridPos.y + 1) + AIRRES.x * AIRRES.y * airGridPos.z] -
            //             vy[airGridPos.x + AIRRES.x * (airGridPos.y) + AIRRES.x * AIRRES.y * airGridPos.z];
            // float v_z = vz[airGridPos.x + AIRRES.x * airGridPos.y + AIRRES.x * AIRRES.y * ( airGridPos.z + 1)] -
            //             vz[airGridPos.x + AIRRES.x * airGridPos.y + AIRRES.x * AIRRES.y * airGridPos.z];

            vec3 this_v = vec3(v_x, v_y, v_z);
            this_v = clamp(this_v, -vec3(MAX_VEL_SCALE), vec3(MAX_VEL_SCALE)) / (MAX_VEL_SCALE);

            float forwardAlphaInv = 1.0 - color.a;
            float this_a = (abs(v_x) + abs(v_y) + abs(v_z)) / 3.0 * 0.3;
            color.rgb += abs(this_v) * this_a * forwardAlphaInv;
            color.a = 1.0 - forwardAlphaInv * (1.0 - this_a);
        }

        mask = lessThanEqual(sideDist.xyz, min(sideDist.yzx, sideDist.zxy));
        sideDist += vec3(mask) * deltaDist;
        voxelPos += ivec3(vec3(mask)) * rayStep;
	}
    return color;
}

void main() {
    // Normalized to 0, 0 = center, scale -1 to 1
    vec2 screenPos = (gl_FragCoord.xy / resolution.xy) * 2.0 - 1.0;
	vec3 rayDir = cameraDir + tan(FOV_DIV2) * (screenPos.x * uv1 + screenPos.y * uv2);
    vec3 rayPos = cameraPos;

    // If not in sim bounding box project to nearest face on ray bounding box
    if (!isInSim(ivec3(rayPos)))
        rayPos = rayCollideSim(rayPos, rayDir);
    // Outside of box early termination
    if (rayPos.x < 0) {
        FragColor = vec4(0.0);
        return;
    }

    FragColor = raymarch(rayPos, rayDir, true);
}
