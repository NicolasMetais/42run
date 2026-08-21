#version 330

out vec4 FragColor;

uniform sampler2D equirectMap;
in vec3 localPos;

const float PI = 3.14159265359;

void main() {
    vec3 dir = normalize(localPos);
    float u = atan(dir.z, dir.x) / (2.0 * PI) + 0.5;
    float v = asin(-dir.y) / PI + 0.5;
    vec3 color = texture(equirectMap, vec2(u, v)).rgb;
    color = min(color, vec3(65000.0));
    FragColor = vec4(color, 1.0);

}