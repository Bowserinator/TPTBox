#version 430

layout(std430, binding = 0) readonly restrict buffer Colors {
    uint colors[];
};
layout(std430, binding = 1) readonly restrict buffer MatFlags {
    uint colorFlags[];
};
layout(std430, binding = 2) readonly restrict buffer ColorLod {
    uint colorsLod[];
};

layout (binding = 3) uniform sampler3D aoBlocks;
layout (binding = 4) uniform sampler2D shadowMap;

// This is an SSBO due to alignment issues and better packing of arrays
layout(std430, binding = 5) readonly restrict buffer Constants {
    vec4 SIMRES;           // Vec3 of XRES, YRES, ZRES
    int NUM_LEVELS;        // Each octree goes up to blocks of side length 2^NUM_LENGTH
    float FOV_DIV2;        // (FOV in radians) / 2

    int MOD_MASK;          // x % 2^(NUM_LEVELS) = x & MOD_MASK, MOD_MASK = (1 << NUM_LEVELS) - 1
    int AO_BLOCK_SIZE;     // Size of ambient occlusion blocks
    ivec4 OCTTREE_BLOCK_DIMS;  // Vec3 of octree block counts (w unused)
    ivec4 AO_BLOCK_DIMS;       // Vec3 of ao block counts (w unused)
    uint LAYER_OFFSETS[6]; // Pre-computed layer offsets

    uint MORTON_X_SHIFTS[256];
    uint MORTON_Y_SHIFTS[256];
    uint MORTON_Z_SHIFTS[256];
};

layout(shared, binding = 6) uniform Settings {
    int MAX_RAY_STEPS;
    uint DEBUG_MODE;       // 0 = regular rendering, see Renderer.h for flags
    float AO_STRENGTH;     // 0 = No AO effect, 1 = max AO effect
    vec3 BACKGROUND_COLOR; // RGB normalized 0-1
    float SHADOW_STRENGTH; // 0 = no shadow, 1 = max strength
    vec3 SHADOW_COLOR;     // RGB normalized 0-1

    bool ENABLE_TRANSPARENCY;
    bool ENABLE_REFLECTION;
    bool ENABLE_REFRACTION;
    bool ENABLE_BLUR;
    bool ENABLE_GLOW;
    bool ENABLE_AO;
    bool ENABLE_SHADOWS;
};

uniform vec2 resolution;  // Viewport res
uniform mat4 mvp;         // Transform matrix
uniform vec3 cameraPos;   // Camera position in world space
uniform vec3 cameraDir;   // Camera look dir (normalized)

uniform vec3 uv1;         // "Up" direction on screen vector mapped to world space
uniform vec3 uv2;         // "Right" direction on screen vector mapped to world space

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragGlowOnly;
layout (location = 2) out vec4 FragBlurOnly;

// Other constants
const float FOG_START_PERCENT = 0.75;     // After this percentage of max ray steps begins to fade to black
const float ALPHA_THRESH = 0.96;          // Above this alpha a ray is considered "stopped"
const float AIR_INDEX_REFRACTION = 1.0;   // Note: can't be 0
const float GLASS_INDEX_REFRACTION = 1.05; // What everything that's "refractive" is assigned to
const vec3 FACE_COLORS = vec3(0.85, 1.0, 0.92);
const float SIMBOX_CAST_PAD = 0.999; // Casting directly on the surface of the sim box (pad=1.0) leads to "z-fighting"
const float DEPTH_FAR_AWAY = 10000.0;

const int MAX_REFRACT_COUNT = 4;
const int MAX_REFLECT_COUNT = 10;

// Graphics flags - same as 2^ what is in SimulationDef.h
const uint G_GLOW = 1;
const uint G_BLUR = 2;
const uint G_REFRACT = 4;
const uint G_REFLECT = 8;
const uint G_NO_LIGHTING = 16;

struct RayCastData {
    bool shouldContinue;
    int steps;
    vec4 color;
    vec3 outPos;
    vec3 outRay;
    ivec3 lastVoxel;
    float prevIndexOfRefraction;
    ivec2 counts;
};


// Functions
// ---------
bvec3 signBit(vec3 v) { return bvec3(v.x < 0, v.y < 0, v.z < 0); }
float min3(vec3 v) { return min(min(v.x,v.y), v.z); }
float max3(vec3 v) { return max(max(v.x,v.y), v.z); }

bool isInSim(vec3 c) {
    return clamp(c, vec3(0.0), SIMRES.xyz - vec3(SIMBOX_CAST_PAD)) == c;
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

// ARGB int32 -> vec4 of RGBA
vec4 toVec4Color(uint tmp) {
    return vec4(tmp & 0xFF, (tmp >> 8) & 0xFF, (tmp >> 16) & 0xFF, tmp >> 24) / 255.0;
}

// Convert to morton code. WARNING: precondition x, y, z < 256
uint mortonDecode(int x, int y, int z) {
    return MORTON_X_SHIFTS[x] | MORTON_Y_SHIFTS[y] | MORTON_Z_SHIFTS[z];
}

// Estimate AO at point, returns darkness multiplier
// (ie 1 = no shadow, 0 = all shadow)
float AO_estimate(ivec3 pos) {
    if (AO_STRENGTH == 0.0 || !ENABLE_AO) return 1.0;

    vec3 aoPos = vec3(pos) / vec3(AO_BLOCK_DIMS) / float(AO_BLOCK_SIZE);
    float ao = texture(aoBlocks, aoPos).r;
    return 1.0 - AO_STRENGTH * ao * ao;
}

// Since colorsLod was originally a uint8 array but is now uint32
// we need to do some bit math to get the byte we want
// pos = position in original colorsLod array on CPU
uint getByteColorsLod(uint pos) {
    uint data = colorsLod[pos >> 2]; // 4 bytes per uint32
    uint remainder = pos & 3; // % 4
    data >>= (remainder << 3);
    return data & 0xFF;
}

// Get flags at location
uint getByteFlags(uvec3 pos) {
    uint idx = uint(pos.x + SIMRES.x * pos.y + SIMRES.y * SIMRES.x * pos.z);
    uint data = colorFlags[idx >> 2]; // 4 bytes per uint32
    uint remainder = idx & 3; // % 4
    data >>= (remainder << 3);
    return data & 0xFF;
}

// If level = 0 returns the color as ARGB uint
// Else returns non-zero if chunk occupied, else 0
uint sampleVoxels(ivec3 pos, int level) {
    if (level > NUM_LEVELS)
        return 1;
    else if (level == 0)
        return colors[uint(pos.x + SIMRES.x * pos.y + (SIMRES.x * SIMRES.y) * pos.z)];

    ivec3 level0Pos = pos << level;
    ivec3 chunkPos = pos >> (NUM_LEVELS - level);
    uint chunkOffset = (chunkPos.x) + (chunkPos.y) * OCTTREE_BLOCK_DIMS.x
        + (chunkPos.z) * OCTTREE_BLOCK_DIMS.x * OCTTREE_BLOCK_DIMS.y;

    if (level == 1) {
        // To check if last level (level 1) is occupied (level with the 2x2x2 chunks,
        // which is one above level 0 (1x1x1 - aka particle resolution)
        // we check the 2nd last level and then the corresponding bit for our level

        uint morton = mortonDecode(level0Pos.x & MOD_MASK, level0Pos.y & MOD_MASK, level0Pos.z & MOD_MASK) >> 3;
        uint bitIdx = morton & 7;
        morton >>= 3;
        return getByteColorsLod(chunkOffset * LAYER_OFFSETS[NUM_LEVELS - 1] + LAYER_OFFSETS[NUM_LEVELS - 2] + morton)
            & (1 << bitIdx);
    }

    uint offset = LAYER_OFFSETS[6 - level]; // Since our level numbering here is reversed from CPU octree... yeah...
    uint morton = mortonDecode(level0Pos.x & MOD_MASK, level0Pos.y & MOD_MASK, level0Pos.z & MOD_MASK) >> (3 * level);
    return getByteColorsLod(chunkOffset * LAYER_OFFSETS[NUM_LEVELS - 1] + offset + morton);
}

// result.xyz = block pos
// result.w = normal (-x -y -z +x +y +z = 0 1 2 3 4 5)
// Thanks to unnick for writing this code
ivec4 raymarch(vec3 pos, vec3 dir, inout RayCastData data) {
    vec3 idir = 1.0 / dir;
    bvec3 dirSignBits = signBit(dir);
    ivec3 rayStep = 1 - ivec3(dirSignBits) * 2; // sign(dir)

    int level = NUM_LEVELS - 1;
    ivec3 voxelPos = ivec3(pos) >> level;
    int initialSteps = data.steps;

    int prevIdx = -1;
    vec4 prevVoxelColor = vec4(0.0);
    float prevIndexOfRefraction = data.prevIndexOfRefraction;
    uint tmpIntColor = 0;

    for (int iter = initialSteps; iter < MAX_RAY_STEPS; iter++, data.steps = iter) {
        if (!isInSim(voxelPos << level)) {
            data.shouldContinue = false;
            return ivec4(-1);
        }

        // Go down a level
        if ((tmpIntColor = sampleVoxels(voxelPos, level)) != 0) {
            if (level == 0) { // Base case, traversing individual voxels
                vec3 tDelta = (vec3(voxelPos + ivec3(not(dirSignBits))) - pos) * idir;
                float minDis = min3(tDelta);
                int idx = tDelta.x == minDis ? 0 : (tDelta.y == minDis ? 1 : 2);

                // Blend forward color (current screen color) with voxel color
                // We are blending front to back
                vec4 voxelColor = toVec4Color(tmpIntColor);
                uint flags = getByteFlags(voxelPos);
                data.lastVoxel = voxelPos;

                if (prevVoxelColor != voxelColor) {
                    float forwardAlphaInv = 1.0 - data.color.a;
                    float ao = ((flags & G_NO_LIGHTING) == 0) ? AO_estimate(voxelPos) : 1.0;
                    data.color.rgb += voxelColor.rgb *
                        (FACE_COLORS[prevIdx] * voxelColor.a * forwardAlphaInv) * ao;
                    data.color.a = 1.0 - forwardAlphaInv * (1.0 - voxelColor.a);
                }
                prevVoxelColor = voxelColor;

                // Color is "solid enough", return
                if (data.color.a > ALPHA_THRESH || !ENABLE_TRANSPARENCY) {
                    data.shouldContinue = false;
                    return ivec4(voxelPos, prevIdx + int(dirSignBits[prevIdx]) * 3);
                }

                float indexOfRefraction = ((flags & G_REFRACT) != 0) ? GLASS_INDEX_REFRACTION : AIR_INDEX_REFRACTION;
                bool shouldReflect = ENABLE_REFLECTION && ((flags & G_REFLECT) != 0) && data.counts.x < MAX_REFLECT_COUNT;
                bool shouldRefract = ENABLE_REFRACTION && ((flags & G_REFRACT) != 0) && prevIndexOfRefraction != indexOfRefraction && data.counts.y < MAX_REFRACT_COUNT;

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
                        data.counts.x++;
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
        if (sampleVoxels(voxelPos >> 1, level + 1) == 0) {
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
    FragGlowOnly = vec4(0.0);
    FragBlurOnly = vec4(0.0);

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
        gl_FragDepth = DEPTH_FAR_AWAY;
        return;
    }

    RayCastData data = RayCastData(
        true,      // shouldContinue
        0,         // steps
        vec4(0.0), // color
        vec3(0.0), // outPos
        vec3(0.0), // outRay
        ivec3(-1), // lastVoxel
        AIR_INDEX_REFRACTION, // prevIndexOfRefraction
        ivec2(0)   // counts
    );

    ivec4 res = ivec4(0);
    do {
        res = raymarch(rayPos, rayDir, data);
        rayDir = data.outRay;
        rayPos = data.outPos;
    } while (data.shouldContinue);
    
    // https://stackoverflow.com/a/29397319/6079328
    vec4 vClipCoord = mvp * vec4(res.xyz, 1.0);
    float fNdcDepth = vClipCoord.z / vClipCoord.w;
    gl_FragDepth = data.color.a > 0.0 ? (fNdcDepth + 1.0) * 0.5 : DEPTH_FAR_AWAY;

    if (DEBUG_MODE == 0) { // NODEBUG
        uint flags = getByteFlags(ivec3(data.lastVoxel));
        bool doShadow = ENABLE_SHADOWS && SHADOW_STRENGTH > 0.0 && ((flags & G_NO_LIGHTING) == 0);
        float shadowZ = doShadow ? 255.0 * texelFetch(shadowMap, data.lastVoxel.xy + ivec2(SIMRES.z - data.lastVoxel.z), 0).r : 0.0;
        float shadowMul = (doShadow && data.lastVoxel.z < shadowZ - 1.05) ? 1.0 - SHADOW_STRENGTH : 1.0;
        float mul = (res.w < 0 || ((flags & G_NO_LIGHTING) != 0) ? 1.0 : FACE_COLORS[res.w % 3]);

        FragColor.rgb = data.color.rgb * mul * shadowMul
            + SHADOW_COLOR * mul * (1 - shadowMul);
        FragColor.a = pow(data.color.a, 0.5); // This alpha is for blending with the grid / bg, not the voxels!

        if (ENABLE_GLOW && (flags & G_GLOW) != 0)
            FragGlowOnly = FragColor;
        else if (ENABLE_BLUR && (flags & G_BLUR) != 0)
            FragBlurOnly = FragColor;
    }
    else if (DEBUG_MODE == 1) { // DEBUG_STEPS
        FragColor = 8.0 * vec4(data.steps) / MAX_RAY_STEPS;
    }
    else if (DEBUG_MODE == 2) { // DEBUG_NORMALS
        FragColor = vec4(0.0);
        if (res.w >= 0) FragColor[res.w % 3] = 1.0;
        if (res.w > 3)  FragColor.rgb = vec3(1.0) - FragColor.rgb;
        FragColor.a = data.color.a > 0.0 ? 1.0 : 0.0;
    }
    else if (DEBUG_MODE == 3) { // DEBUG_AO
        float mul = (res.w < 0 ? 1.0 : FACE_COLORS[res.w % 3]) * AO_estimate(res.xyz);
        FragColor.rgb = vec3(mul);
        FragColor.a = data.color.a > 0.0 ? 1.0 : 0.0;
    }
}