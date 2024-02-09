#version 430

uniform vec2 resolution;
out vec4 FragColor;

uniform sampler2D baseTexture;
uniform vec2 direction;

// const int RADIUS = 16;
// const float WEIGHTS[] = { // exp(-0.5 * (r * r) / (RADIUS * RADIUS)) / (total sum)
//     0.030526785252358146, 0.030524301077297273, 0.03051684976497561, 0.03050443495298911,
//     0.030487062700709236, 0.030464741484354147, 0.030437482190096257, 0.030405298105213998,
//     0.03036820490729781, 0.030326220651522684, 0.030279365756001694, 0.03022766298523718,
//     0.030171137431688304, 0.030109816495475936, 0.030043729862247823, 0.029972909479229044,
//     0.02989738952948485
// };

void main() {
    vec4 color = vec4(0.0);
    // for (int r = -RADIUS; r <= RADIUS; r++)
    //     color += texture(baseTexture, (gl_FragCoord.xy + direction * r) / resolution) * WEIGHTS[abs(r)];

    // Unrolling saves about 1ms of render time!
    color += texture(baseTexture, (gl_FragCoord.xy) / resolution) * 0.030526785252358146;
    color += (texture(baseTexture, (gl_FragCoord.xy + direction * 1) / resolution) + texture(baseTexture, (gl_FragCoord.xy - direction * 1) / resolution)) * 0.030524301077297273;
    color += (texture(baseTexture, (gl_FragCoord.xy + direction * 2) / resolution) + texture(baseTexture, (gl_FragCoord.xy - direction * 2) / resolution)) * 0.03051684976497561;
    color += (texture(baseTexture, (gl_FragCoord.xy + direction * 3) / resolution) + texture(baseTexture, (gl_FragCoord.xy - direction * 3) / resolution)) * 0.03050443495298911;
    color += (texture(baseTexture, (gl_FragCoord.xy + direction * 4) / resolution) + texture(baseTexture, (gl_FragCoord.xy - direction * 4) / resolution)) * 0.030487062700709236;
    color += (texture(baseTexture, (gl_FragCoord.xy + direction * 5) / resolution) + texture(baseTexture, (gl_FragCoord.xy - direction * 5) / resolution)) * 0.030464741484354147;
    color += (texture(baseTexture, (gl_FragCoord.xy + direction * 6) / resolution) + texture(baseTexture, (gl_FragCoord.xy - direction * 6) / resolution)) * 0.030437482190096257;
    color += (texture(baseTexture, (gl_FragCoord.xy + direction * 7) / resolution) + texture(baseTexture, (gl_FragCoord.xy - direction * 7) / resolution)) * 0.030405298105213998;
    color += (texture(baseTexture, (gl_FragCoord.xy + direction * 8) / resolution) + texture(baseTexture, (gl_FragCoord.xy - direction * 8) / resolution)) * 0.03036820490729781;
    color += (texture(baseTexture, (gl_FragCoord.xy + direction * 9) / resolution) + texture(baseTexture, (gl_FragCoord.xy - direction * 9) / resolution)) * 0.030326220651522684;
    color += (texture(baseTexture, (gl_FragCoord.xy + direction * 10) / resolution) + texture(baseTexture, (gl_FragCoord.xy - direction * 10) / resolution)) * 0.030279365756001694;
    color += (texture(baseTexture, (gl_FragCoord.xy + direction * 11) / resolution) + texture(baseTexture, (gl_FragCoord.xy - direction * 11) / resolution)) * 0.03022766298523718;
    color += (texture(baseTexture, (gl_FragCoord.xy + direction * 12) / resolution) + texture(baseTexture, (gl_FragCoord.xy - direction * 12) / resolution)) * 0.030171137431688304;
    color += (texture(baseTexture, (gl_FragCoord.xy + direction * 13) / resolution) + texture(baseTexture, (gl_FragCoord.xy - direction * 13) / resolution)) * 0.030109816495475936;
    color += (texture(baseTexture, (gl_FragCoord.xy + direction * 14) / resolution) + texture(baseTexture, (gl_FragCoord.xy - direction * 14) / resolution)) * 0.030043729862247823;
    color += (texture(baseTexture, (gl_FragCoord.xy + direction * 15) / resolution) + texture(baseTexture, (gl_FragCoord.xy - direction * 15) / resolution)) * 0.029972909479229044;
    color += (texture(baseTexture, (gl_FragCoord.xy + direction * 16) / resolution) + texture(baseTexture, (gl_FragCoord.xy - direction * 16) / resolution)) * 0.02989738952948485;

    FragColor = color;
    FragColor.a *= 2.0;
}
