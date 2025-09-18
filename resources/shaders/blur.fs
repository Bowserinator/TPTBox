#version 430

uniform vec2 resolution;
out vec4 FragColor;

uniform sampler2D baseTexture;
uniform int iteration;

void main() {
    vec4 color = vec4(0.0);

    // Kawase blur
    color += texture(baseTexture, (gl_FragCoord.xy + vec2(0.5 + iteration, 0.5 + iteration)) / resolution) * 0.25;
    color += texture(baseTexture, (gl_FragCoord.xy + vec2(0.5 + iteration, -(0.5 + iteration))) / resolution) * 0.25;
    color += texture(baseTexture, (gl_FragCoord.xy + vec2(-(0.5 + iteration), 0.5 + iteration)) / resolution) * 0.25;
    color += texture(baseTexture, (gl_FragCoord.xy + vec2(-(0.5 + iteration), -(0.5 + iteration))) / resolution) * 0.25;
   
    color.a = 1.0; // Properly set alpha in post or it will be too dim since we repeatedly blur
    FragColor = color;
}
