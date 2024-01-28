#version 430

layout (binding = 0) uniform sampler3D colors;
layout (binding = 1) uniform sampler3D lod_2;
layout (binding = 2) uniform sampler3D lod_4;
layout (binding = 3) uniform sampler3D lod_8;
layout (binding = 4) uniform sampler3D lod_16;
layout (binding = 5) uniform sampler3D lod_32;
layout (binding = 6) uniform sampler3D lod_64;

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

const bool DEBUG_CASTS = false;
const bool DEBUG_NORMALS = false;

const vec3 FACE_COLORS = vec3(0.7, 1.0, 0.85);
const int MAX_RAY_STEPS = 256 * 2;
const float SIMBOX_CAST_PAD = 0.999; // Casting directly on the surface of the sim box (pad=1.0) leads to "z-fighting"
const int NUM_LEVELS = 6;

const int MAX_REFRACT_COUNT = 4;
const int MAX_REFLECT_COUNT = 10;

struct RayCastData {
    bool shouldContinue;
    int steps;
    vec4 color;
    vec3 outPos;
    vec3 outRay;
    float prevIndexOfRefraction;
    ivec2 counts;
};


// Functions
// ---------
bvec3 signBit(vec3 v) { return bvec3(v.x < 0, v.y < 0, v.z < 0); }
float min3(vec3 v) { return min(min(v.x,v.y), v.z); }
float max3(vec3 v) { return max(max(v.x,v.y), v.z); }

bool isInSim(vec3 c) {
    return clamp(c, vec3(0.0), simRes - vec3(SIMBOX_CAST_PAD)) == c;
}

// Collide ray with sim bounding cube, rayDir should be normalized
vec3 rayCollideSim(vec3 rayPos, vec3 rayDir) {
    vec3 v = vec3(1.0) / rayDir;
    vec3 boundMin = (vec3(SIMBOX_CAST_PAD) - rayPos) * v;
    vec3 boundMax = (simRes - vec3(SIMBOX_CAST_PAD) - rayPos) * v;
    float a = max3(min(boundMin, boundMax));
    float b = min3(max(boundMin, boundMax));

    // We do not need to check whether we missed since whatever result
    // will be out of the simulation and the loop will immedately terminate
    vec3 collisionPoint = rayPos + rayDir * a;
    return collisionPoint;
}

bool sampleVoxels(ivec3 pos, int level) {
    if (level >= NUM_LEVELS) return true;
    if (level == 0)
        return texelFetch(colors, ivec3(pos), 0).a != 0.0;

    if (level == 1)
        return texelFetch(lod_2, ivec3(pos), 0).r != 0.0;
    if (level == 2)
        return texelFetch(lod_4, ivec3(pos), 0).r != 0.0;
    if (level == 3)
        return texelFetch(lod_8, ivec3(pos), 0).r != 0.0;
    if (level == 4)
        return texelFetch(lod_16, ivec3(pos), 0).r != 0.0;
    if (level == 5)
        return texelFetch(lod_32, ivec3(pos), 0).r != 0.0;
    return texelFetch(lod_64, ivec3(pos), 0).r != 0.0;
}

// result.xyz = block pos
// result.w = normal (-x -y -z +x +y +z = 0 1 2 3 4 5)
// Thanks to unnick for writing this code
ivec4 raymarch(vec3 pos, vec3 dir, out RayCastData data) {
    vec3 idir = 1.0 / dir;
    bvec3 dirSignBits = signBit(dir);
    ivec3 rayStep = 1 - ivec3(dirSignBits) * 2; // sign(dir)

    int level = NUM_LEVELS - 1;
    ivec3 voxelPos = ivec3(pos) >> level;
    int prevIdx = -1;
    int initialSteps = data.steps;
    vec4 prevVoxelColor = vec4(0.0);
    float prevIndexOfRefraction = data.prevIndexOfRefraction;

    for (int iter = initialSteps; iter < MAX_RAY_STEPS; iter++, data.steps = iter) {
        if (!isInSim(voxelPos << level)) {
            data.shouldContinue = false;
            return ivec4(-1);
        }

        // Go down a level
        if (sampleVoxels(voxelPos, level)) {
            if (level == 0) { // Base case, traversing individual voxels
                vec3 tDelta = (vec3(voxelPos + ivec3(not(dirSignBits))) - pos) * idir;
                float minDis = min3(tDelta);
                int idx = tDelta.x == minDis ? 0 : (tDelta.y == minDis ? 1 : 2);

                // Blend forward color (current screen color) with voxel color
                // We are blending front to back
                vec4 voxelColor = texelFetch(colors, voxelPos, 0);
                if (prevVoxelColor != voxelColor) {
                    float forwardAlphaInv = 1.0 - data.color.a;
                    data.color.rgb += voxelColor.rgb * FACE_COLORS[prevIdx] * voxelColor.a * forwardAlphaInv;
                    data.color.a = 1.0 - forwardAlphaInv * (1.0 - voxelColor.a);
                }
                prevVoxelColor = voxelColor;

                // Color is "solid enough", return
                if (data.color.a > ALPHA_THRESH) {
                    data.shouldContinue = false;
                    return ivec4(voxelPos, prevIdx + int(dirSignBits[prevIdx]) * 3);
                }

                float indexOfRefraction = voxelColor.a < 1.0 ? 1.5 : AIR_INDEX_REFRACTION; // TODO use a texture to get
                bool shouldReflect = false && data.counts.x < MAX_REFLECT_COUNT; // TODO
                bool shouldRefract = false && prevIndexOfRefraction != indexOfRefraction && data.counts.y < MAX_REFRACT_COUNT;

                if (shouldReflect || shouldRefract) {
                    int normalIdx = prevIdx + int(dirSignBits[prevIdx]) * 3;
                    vec3 normal = vec3(0.0);
                    normal[prevIdx] = 1.0 * rayStep[prevIdx];

                    data.shouldContinue = true;

                    // Refraction
                    if (shouldRefract) {
                        data.outRay = refract(dir, -normal, 1.0 / (indexOfRefraction / prevIndexOfRefraction));
                        data.counts.y++;

                        if (dot(data.outRay, data.outRay) > 0.0) {
                            voxelPos[prevIdx] -= rayStep[prevIdx];
                            data.outPos = voxelPos;
                            data.prevIndexOfRefraction = indexOfRefraction;
                            return ivec4(voxelPos, normalIdx);
                        }
                        shouldReflect = true; // Total internal reflection
                    }

                    // Reflection
                    if (shouldReflect) {
                        voxelPos[prevIdx] -= rayStep[prevIdx];
                        data.outPos = voxelPos;
                        data.outRay = reflect(dir, normal);
                        return ivec4(voxelPos, normalIdx);
                    }
                }

                voxelPos[idx] += rayStep[idx];
                prevIdx = idx;
                continue;
            } else {
                vec3 tDelta = (vec3((voxelPos + ivec3(dirSignBits)) << level) - pos) * idir;
                float dist = max(tDelta[prevIdx], 0.);
                level--;
                voxelPos = clamp(ivec3(pos + dir * dist) >> level, voxelPos << 1, (voxelPos << 1) + 1);
                continue;
            }
        }

        // Go up a level
        if (!sampleVoxels(voxelPos >> 1, level + 1)) {
            level++;
            voxelPos >>= 1;
        }

        vec3 tDelta = (vec3((voxelPos + ivec3(not(dirSignBits))) << level) - pos) * idir;
        float minDis = min3(tDelta);
        int idx = tDelta.x == minDis ? 0 : (tDelta.y == minDis ? 1 : 2);
        voxelPos[idx] += rayStep[idx];
        prevIdx = idx;
    }

    data.shouldContinue = false;
    return ivec4(-1);
}


void main() {
    // Normalized to 0, 0 = center, scale -1 to 1
    vec2 screenPos = (gl_FragCoord.xy / resolution.xy) * 2.0 - 1.0;
	vec3 rayDir = cameraDir + tan(FOV / 2 * DEG2RAD) * (screenPos.x * uv1 + screenPos.y * uv2);
    vec3 rayPos = cameraPos;

    // If not in sim bounding box project to nearest face on ray bounding box
    if (!isInSim(ivec3(rayPos)))
        rayPos = rayCollideSim(rayPos, rayDir);

    RayCastData data = RayCastData(
        true,      // shouldContinue
        0,         // steps
        vec4(0.0), // color
        vec3(0.0), // outPos
        vec3(0.0), // outRay
        AIR_INDEX_REFRACTION,  // prevIndexOfRefraction
        ivec2(0)   // counts
    );

    ivec4 res = ivec4(0);
    do {
        res = raymarch(rayPos, rayDir, data);
        rayDir = data.outRay;
        rayPos = data.outPos;
    } while (data.shouldContinue);

    // display number of steps
    if (DEBUG_CASTS)
        FragColor = 4.0 * vec4(data.steps) / MAX_RAY_STEPS;
    else if (DEBUG_NORMALS) {
        FragColor = vec4(0.0);
        if (res.w >= 0)
            FragColor[res.w % 3] = 1.0;
        if (res.w > 3)
            FragColor.rgb = vec3(1.0) - FragColor.rgb;
        FragColor.a = sign(data.color.a);
    }
    else {
        float mul = (res.w < 0 ? 1.0 : FACE_COLORS[res.w % 3]) * data.color.a;
        FragColor.rgb = data.color.rgb * mul;
        FragColor.a = sign(data.color.a);
    }
}