#version 330

in vec4 fragColor;
in vec2 fragTexCoord;

uniform sampler2D texture0;
uniform vec2 resolution;
uniform vec4 colDiffuse;
uniform sampler2D depth;

out vec4 finalColor;

void main() {
    vec4 texelColor = texture(texture0, fragTexCoord);
    vec4 outColor = texelColor * colDiffuse;
    float voxDepth = texture(depth, gl_FragCoord.xy / resolution).r;

    if (gl_FragCoord.z > voxDepth) {
        finalColor = vec4(outColor.rgb, outColor.a * 0.4);
        gl_FragDepth = voxDepth;
    } else {
        finalColor = outColor;
        gl_FragDepth = gl_FragCoord.z;
    }
}