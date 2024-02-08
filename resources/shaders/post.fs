#version 430

// TODO
layout(shared, binding = 4) uniform Settings {
    int MAX_RAY_STEPS;
    uint DEBUG_MODE;      // 0 = regular rendering, see Renderer.h for flags
    float AO_STRENGTH;    // 0 = No AO effect, 1 = max AO effect
};

uniform vec2 resolution;  // Viewport res
out vec4 FragColor;

uniform sampler2D baseTexture;
uniform sampler2D glowTexture;
uniform sampler2D blurTexture;
uniform sampler2D depthTexture;

void main() {
    gl_FragDepth = texture(depthTexture, gl_FragCoord.xy / resolution).r;

    vec4 orgColor = texture(baseTexture, gl_FragCoord.xy / resolution);
    vec4 glowColor = texture(glowTexture, gl_FragCoord.xy / resolution);
    vec4 blurColor = texture(blurTexture, gl_FragCoord.xy / resolution);

    orgColor.rgb = orgColor.rgb * orgColor.a + blurColor.rgb * (1.0 - orgColor.a) * blurColor.a;
    orgColor.a = orgColor.a + blurColor.a * (1.0 - orgColor.a);

    vec4 outColor = orgColor + 4.0 * glowColor;
    FragColor = outColor;
}