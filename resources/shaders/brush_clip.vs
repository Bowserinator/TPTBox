#version 330

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;

out float gl_ClipDistance[1];
out vec4 fragColor;
out vec2 fragTexCoord;

uniform mat4 mvp;
uniform mat4 mv;
uniform vec3 simRes;

void main() {
    gl_Position = mvp * vec4(vertexPosition, 1.0);
    fragColor = vertexColor;
    fragTexCoord = vertexTexCoord;

    vec4 translatedPos = mv * vec4(vertexPosition, 1.0);
    bool outside =
        (dot(translatedPos, vec4(1.0, 0.0, 0.0, -1.0)) < 0) ||
        (dot(translatedPos, vec4(0.0, 1.0, 0.0, -1.0)) < 0) ||
        (dot(translatedPos, vec4(0.0, 0.0, 1.0, -1.0)) < 0) ||
        (dot(translatedPos, vec4(-1.0, 0.0, 0.0, simRes.x - 1.0)) < 0) ||
        (dot(translatedPos, vec4(0.0, -1.0, 0.0, simRes.y - 1.0)) < 0) ||
        (dot(translatedPos, vec4(0.0, 0.0, -1.0, simRes.z - 1.0)) < 0);
    gl_ClipDistance[0] = outside ? -1 : 1;
}
