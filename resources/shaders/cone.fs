#version 430

layout(std430, binding = 0) readonly restrict buffer ColorLod {
    uint colorsLod[];
};

layout(shared, binding = 1) uniform Constants {
    vec3 SIMRES;           // Vec3 of XRES, YRES, ZRES
    int NUM_LEVELS;        // Each octree goes up to blocks of side length 2^NUM_LENGTH
    float FOV_DIV2;        // (FOV in radians) / 2

    uint LAYER_OFFSETS[6]; // Pre-computed layer offsets
    int MOD_MASK;          // x % 2^(NUM_LEVELS) = x & MOD_MASK, MOD_MASK = (1 << NUM_LEVELS) - 1
    int AO_BLOCK_SIZE;     // Size of ambient occlusion blocks
    ivec3 OCTTREE_BLOCK_DIMS;  // Vec3 of octree block counts (w unused)
    ivec3 AO_BLOCK_DIMS;       // Vec3 of ao block counts (w unused)

    uint MORTON_X_SHIFTS[256];
    uint MORTON_Y_SHIFTS[256];
    uint MORTON_Z_SHIFTS[256];
};

uniform vec2 resolution;  // Viewport res
uniform mat4 mvp;         // Transform matrix
uniform vec3 cameraPos;   // Camera position in world space
uniform vec3 cameraDir;   // Camera look dir (normalized)

uniform vec3 uv1;         // "Up" direction on screen vector mapped to world space
uniform vec3 uv2;         // "Right" direction on screen vector mapped to world space

out vec4 FragColor;

// Other constants
const float SIMBOX_CAST_PAD = 0.999; // Casting directly on the surface of the sim box (pad=1.0) leads to "z-fighting"
const float DEPTH_FAR_AWAY = 10000.0;

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

// Since colorsLod was originally a uint8 array but is now uint32
// we need to do some bit math to get the byte we want
// pos = position in original colorsLod array on CPU
uint getByteColorsLod(uint pos) {
    uint data = colorsLod[pos >> 2]; // 4 bytes per uint32
    uint remainder = pos & 3; // % 4
    data >>= (remainder << 3);
    return data & 0xFF;
}

// If level = 0 returns the color as ARGB uint
// Else returns non-zero if chunk occupied, else 0
// Note: this variant assumes level > 1
uint sampleVoxels(ivec3 pos, int level) {
    if (level > NUM_LEVELS)
        return 1;

    ivec3 level0Pos = pos << level;
    uint chunkOffset = (level0Pos.x >> NUM_LEVELS) + (level0Pos.y >> NUM_LEVELS) * OCTTREE_BLOCK_DIMS.x
        + (level0Pos.z >> NUM_LEVELS) * OCTTREE_BLOCK_DIMS.x * OCTTREE_BLOCK_DIMS.y;

    uint offset = LAYER_OFFSETS[6 - level]; // Since our level numbering here is reversed from CPU octree... yeah...
    uint morton = mortonDecode(level0Pos.x & MOD_MASK, level0Pos.y & MOD_MASK, level0Pos.z & MOD_MASK) >> (3 * level);
    return getByteColorsLod(chunkOffset * LAYER_OFFSETS[NUM_LEVELS - 1] + offset + morton);
}

// result.xyz = block pos
ivec4 raymarch(vec3 pos, vec3 dir) {
    vec3 idir = 1.0 / dir;
    bvec3 dirSignBits = signBit(dir);
    ivec3 rayStep = 1 - ivec3(dirSignBits) * 2; // sign(dir)

    int level = NUM_LEVELS - 1;
    ivec3 voxelPos = ivec3(pos) >> level;
    int prevIdx = -1;

    for (int iter = 0; iter < 400; iter++) {
        if (!isInSim(voxelPos << level))
            return ivec4(-1);

        // Go down a level
        if (sampleVoxels(voxelPos, level) != 0) {
            if (level == 4) {
                vec3 tDelta = (vec3(voxelPos + ivec3(not(dirSignBits))) - pos) * idir;
                float minDis = min3(tDelta);
                return ivec4(voxelPos << level, 0);
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
    return ivec4(-1);
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
        FragColor.rgb = vec3(1.0);
        return;
    }

    ivec4 res = raymarch(rayPos, rayDir);
    float dis = distance(vec3(res.xyz), rayPos);
    FragColor.rgb = res.w < 0 ? vec3(1.0) : vec3(dis / dot(SIMRES, ivec3(1)));
    FragColor.a = 1.0;
}
