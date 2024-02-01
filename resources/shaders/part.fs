#version 430

#define LOD_LEVELS 6 // Should match value in Renderer and frag shader TODO uniform for this

layout(std430, binding = 0) readonly restrict buffer Colors {
    uint colors[];
};
layout(std430, binding = 1) readonly restrict buffer colorLod {
    uint colors_lod[];
};


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

    if (b < 0 || a > b) // Failed to hit box, needs to check when very close to sim edge
        return vec3(-1);

    vec3 collisionPoint = rayPos + rayDir * a;
    return collisionPoint;
}

vec4 getColorAt(ivec3 pos) {
    uint tmp = colors[uint(pos.x + simRes.x * pos.y + (simRes.x * simRes.y) * pos.z)];
    vec4 color = vec4(tmp & 0xFF, (tmp >> 8) & 0xFF, (tmp >> 16) & 0xFF, tmp >> 24) / 255.0;
    return color;
}

// Note: here level is different from colors_lod
// level 0 is the 1x1x1 voxel layer, level 6 is the 64x64x64 cube layer
uint morton_decode(int x, int y, int z) {
    // 3,3,3 = 63, 1,2,3 = 29
    // TODO: faster
    int j = 0;
    uint out2 = 0;
    for (int i = 0; i < 8; i++) {
        if ((z & 1) != 0) out2 |= (1 << j);
        j++;
        if ((y & 1) != 0) out2 |= (1 << j);
        j++;
        if ((x & 1) != 0) out2 |= (1 << j);
        j++;

        x >>= 1;
        y >>= 1;
        z >>= 1;
    }
    return out2;
}

uint get_byte(uint pos) {
    uint data = colors_lod[pos / 4];
    uint remainder = pos & 3; // % 4
    data >>= 8 * (remainder);
    data = data & 0xFF;
    return data;
}

bool sampleVoxels(ivec3 pos, int level) {
    if (level >= NUM_LEVELS) return true;
    if (level == 0)
        return (colors[uint(pos.x + simRes.x * pos.y + (simRes.x * simRes.y) * pos.z)]) != 0;

    int offset = 0;

    ivec3 pos2 = pos << (level);
    // TODO: compile these constants into shaders
    // ie 4 = ceil(XRES / (1 << NUM_LEVELS))
    int chunk_offset = (pos2.x >> 6) + (pos2.y >> NUM_LEVELS) * 4 + (pos2.z >> NUM_LEVELS) * 4 * 4;

    if (level == 5)
        offset = 0;
    else if (level == 4)
        offset = 1;
    else if (level == 3)
        offset = 1 + 8;
    else if (level == 2)
        offset = 1 + 8 + 64;
    else if (level == 1)
        offset = 1 + 8 + 64 + 64 * 8;

    uint morton = morton_decode(pos2.x & 63, pos2.y & 63, pos2.z & 63) >> (3 * level + 3);
    return get_byte(chunk_offset * 37449 + offset + morton) != 0; //  & (1 << bit_idx)
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
                vec4 voxelColor = getColorAt(voxelPos);
                if (prevVoxelColor != voxelColor) {
                    float forwardAlphaInv = 1.0 - data.color.a;
                    data.color.rgb += voxelColor.rgb * (FACE_COLORS[prevIdx] * voxelColor.a * forwardAlphaInv);
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
                    normal[prevIdx] = rayStep[prevIdx];

                    data.shouldContinue = true;

                    // Refraction
                    if (shouldRefract) {
                        data.outRay = refract(dir, -normal, prevIndexOfRefraction / indexOfRefraction);
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
    // Outside of box early termination
    if (rayPos.x < 0) {
        FragColor = vec4(0.0);
        return;
    }

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

    if (DEBUG_CASTS)
        FragColor = 8.0 * vec4(data.steps) / MAX_RAY_STEPS;
    else if (DEBUG_NORMALS) {
        FragColor = vec4(0.0);
        if (res.w >= 0)
            FragColor[res.w % 3] = 1.0;
        if (res.w > 3)
            FragColor.rgb = vec3(1.0) - FragColor.rgb;
        FragColor.a = data.color.a > 0.0 ? 1.0 : 0.0;
    }
    else {
        float mul = (res.w < 0 ? 1.0 : FACE_COLORS[res.w % 3]) * data.color.a;
        FragColor.rgb = data.color.rgb * mul;
        FragColor.a = data.color.a > 0.0 ? 1.0 : 0.0;
    }
}