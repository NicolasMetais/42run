#version 330 core

in vec2 uv;
out vec4 fragColor;

uniform vec4 color;
uniform sampler2D fontTex;
uniform int useTexture;


void main() {
    if (useTexture == 1)
        fragColor = texture(fontTex, uv);
    else
        fragColor = color;
}