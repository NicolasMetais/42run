#version 330 core

in vec2 uv;
out vec4 fragColor;

uniform vec2 mouse;
uniform float mouseSpeed;
uniform sampler2D bottomTex;
uniform float time;
uniform float rippleTime;


void main() {
    float dist = distance(uv, mouse);
    float ring = smoothstep(0.04, 0.0, abs(dist - rippleTime * 0.4)) * mouseSpeed;
    float ripple = ring * 0.015;
    vec2 distortedUv = uv + vec2(
        sin(uv.y * 10.0 + time) * 0.01,
        cos(uv.x * 10.0 + time) * 0.01
    ) + ripple;
    vec4 bottom = texture(bottomTex, distortedUv);
    vec4 WaterTint = vec4(0.0, 0.3, 0.5, 0.5);
    fragColor = mix(bottom, WaterTint, 0.4);
}