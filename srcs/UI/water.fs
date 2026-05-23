#version 330 core

in vec2 uv;
out vec4 fragColor;

uniform vec2 mouse;
uniform float time;


void main() {
    fragColor = vec4(0.0, 0.3, 0.5, 1.0);
}