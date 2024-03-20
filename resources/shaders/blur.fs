#version 430

uniform vec2 resolution;
out vec4 FragColor;

uniform sampler2D baseTexture;
uniform vec2 direction;

void main() {
    vec4 color = vec4(0.0);

    // Box blur of radius 5
    color += texture(baseTexture, (gl_FragCoord.xy) / resolution) * 0.090909;
    color += (texture(baseTexture, (gl_FragCoord.xy + direction * 1) / resolution) + texture(baseTexture, (gl_FragCoord.xy - direction * 1) / resolution)) * 0.090909;
    color += (texture(baseTexture, (gl_FragCoord.xy + direction * 2) / resolution) + texture(baseTexture, (gl_FragCoord.xy - direction * 2) / resolution)) * 0.090909;
    color += (texture(baseTexture, (gl_FragCoord.xy + direction * 3) / resolution) + texture(baseTexture, (gl_FragCoord.xy - direction * 3) / resolution)) * 0.090909;
    color += (texture(baseTexture, (gl_FragCoord.xy + direction * 4) / resolution) + texture(baseTexture, (gl_FragCoord.xy - direction * 4) / resolution)) * 0.090909;
    color += (texture(baseTexture, (gl_FragCoord.xy + direction * 5) / resolution) + texture(baseTexture, (gl_FragCoord.xy - direction * 5) / resolution)) * 0.090909;
    color.a = 1.0; // Properly set alpha in post or it will be too dim since we repeatedly blur
    FragColor = color;
}
