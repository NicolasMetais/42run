#version 330 core

in vec2 uv;

out vec4 fragColor;

uniform vec3 textColor;
uniform sampler2D fontTex;

float median(float r, float g, float b) {
    return max(min(r,g), min(max(r,g), b));
}

void main() {
    vec3 sample = texture(fontTex, uv).rgb;
    float dist = median(sample.r, sample.g, sample.b);
    float alpha = smoothstep(0.5 - 0.005, 0.5 + 0.005, dist); //smoothstep for antialising
    fragColor = vec4(textColor, alpha);
}