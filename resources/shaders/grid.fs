#version 430

in vec2 fragTexCoord;
out vec4 FragColor;

uniform float size;
uniform float scale;

const float lineWidthBase = 0.4;
const int bigLineEveryNLines = 10;

bool shouldBeBold(float uvX, float lineAA) {
    return (fract(uvX) < max(0.1, lineAA)) && ((int(round(uvX)) % bigLineEveryNLines) == 0);
}

// Code is from https://bgolus.medium.com/the-best-darn-grid-shader-yet-727f9278b9d8
void main() {
    if (scale <= 0.0) {
        FragColor = vec4(0.);
        return;
    }

    float lineWidth = lineWidthBase / scale;
    vec2 uv = fragTexCoord * size / scale;

    vec4 uvDDXY = vec4(dFdx(uv), dFdy(uv));
    vec2 uvDeriv = vec2(length(uvDDXY.xz), length(uvDDXY.yw));
    vec2 drawWidth = clamp(vec2(lineWidth), uvDeriv, vec2(0.5));

    vec2 lineAA = uvDeriv * 1.5;
    vec2 gridUV = abs(fract(uv) * 2.0 - 1.0);
    gridUV = 1.0 - gridUV;

    vec2 grid2 = smoothstep(drawWidth + lineAA, drawWidth - lineAA, gridUV);
    grid2 *= clamp(lineWidth / drawWidth, vec2(0.), vec2(1.));
    grid2 = mix(grid2, vec2(lineWidth), vec2(clamp(uvDeriv * 2.0 - 1.0, 0., 1.)));

    float grid = mix(grid2.x, 1.0, grid2.y);
    float colorMul = (shouldBeBold(uv.x, lineAA.x) || shouldBeBold(uv.y, lineAA.y)) ? 1.0 : 0.2;
    grid *= colorMul;

    FragColor = vec4(grid, grid, grid, grid > 0 ? 1.0 : 0.0);
}