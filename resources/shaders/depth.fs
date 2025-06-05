#version 430

uniform vec2 resolution;  // Viewport res
out vec4 FragColor;

uniform sampler2D baseTexture;
uniform sampler2D depthTexture;

void main() {
    gl_FragDepth = texture(depthTexture, gl_FragCoord.xy / resolution).r;
    FragColor = texture(baseTexture, gl_FragCoord.xy / resolution);

    // For visualizing depth
    // float zNear = 0.01; // camera z near
    // float zFar = 500.0;  // camera z far
    // float z = texture(depthTexture, gl_FragCoord.xy / resolution).r;
    // float depth = (2.0*zNear)/(zFar + zNear - z*(zFar - zNear));
    // FragColor = vec4(depth, depth, depth, 1.0);
}
