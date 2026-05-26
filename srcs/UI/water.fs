#version 330 core

in vec2 uv;
out vec4 fragColor;

uniform sampler2D bottomTex;
uniform float time;
uniform vec3 drops[8];
uniform int dropCount;

void main() {
    vec2 rippleOffset = vec2(0.0);
    for (int i = 0; i < dropCount; i++) {
        vec2 delta = uv - drops[i].xy;
        float dist = length(delta);
        float strength = max(1.0 - drops[i].z, 0.0);
        float ring = smoothstep(0.06, 0.0, abs(dist - drops[i].z * 0.5)) * strength;
        vec2 dir = dist > 0.001 ? normalize(delta) : vec2(0.0);
        rippleOffset += dir * ring * 0.015;
    }
    vec2 distortedUv = uv + vec2(
        sin(uv.y * 10.0 + time) * 0.01,
        cos(uv.x * 10.0 + time) * 0.01
    ) + rippleOffset;
    vec4 bottom = texture(bottomTex, distortedUv);
    vec4 waterTint = vec4(0.0, 0.3, 0.5, 0.5);
    float caustic = sin(uv.x * 10.0 + time) * sin(uv.y * 10.0 + time * 0.7)
                + sin(uv.x * 13.0 - time * 0.5) * sin(uv.y * 17.0 + time * 0.3);
    caustic = pow(max(caustic, 0.0), 1.0) * 0.1;
    fragColor = mix(bottom, waterTint, 0.4)+ vec4(caustic);
}
