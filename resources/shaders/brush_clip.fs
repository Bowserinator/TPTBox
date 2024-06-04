#version 330

in vec4 fragColor;
in vec2 fragTexCoord;
// in vec3 vertexPos;

uniform sampler2D texture0;
uniform vec2 resolution;
uniform vec4 colDiffuse;
uniform sampler2D depth;

out vec4 finalColor;

// const float EDGE_THICKNESS = 0.05;
// bool checkSide(float a, float b) {
//     return min(a, b) > EDGE_THICKNESS && max(a, b) < 1.0 - EDGE_THICKNESS;
// }

void main() {
    vec4 texelColor = texture(texture0, fragTexCoord);
    vec4 outColor = texelColor * colDiffuse;
    float voxDepth = texture(depth, gl_FragCoord.xy / resolution).r;

    // vec3 m = fract(vertexPos);
    //if (checkSide(m.x, m.y) || checkSide(m.y, m.z) || checkSide(m.x, m.z))
    //    discard;

    if (gl_FragCoord.z > voxDepth) {
        finalColor = vec4(outColor.rgb, outColor.a * 0.5);
        gl_FragDepth = voxDepth;
    } else {
        finalColor = outColor;
        gl_FragDepth = gl_FragCoord.z;
    }
}