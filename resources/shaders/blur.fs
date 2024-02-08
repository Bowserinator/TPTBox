#version 430

uniform vec2 resolution;
out vec4 FragColor;

uniform sampler2D baseTexture;
uniform vec2 direction;

const int RADIUS = 16;
const float WEIGHTS[] = { // exp(-0.5 * (r * r) / (RADIUS * RADIUS)) / (total sum)
    0.030526785252358146, 0.030524301077297273, 0.03051684976497561, 0.03050443495298911,
    0.030487062700709236, 0.030464741484354147, 0.030437482190096257, 0.030405298105213998,
    0.03036820490729781, 0.030326220651522684, 0.030279365756001694, 0.03022766298523718,
    0.030171137431688304, 0.030109816495475936, 0.030043729862247823, 0.029972909479229044,
    0.02989738952948485
};

void main() {
    vec4 color = vec4(0.0);
    for (int r = -RADIUS; r <= RADIUS; r++)
        color += texture(baseTexture, (gl_FragCoord.xy + direction * r) / resolution) * WEIGHTS[abs(r)];
    FragColor = color;
}
