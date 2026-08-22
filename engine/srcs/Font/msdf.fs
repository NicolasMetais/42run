#version 330 core

in vec2 uv;

out vec4 fragColor;

uniform vec3 textColor;
uniform sampler2D fontTex;
uniform float pxRange; // distanceRange de l'atlas (resources/*.json), en texels
uniform float outlineWidth;
uniform vec3 outlineColor;

float median(float r, float g, float b) {
    return max(min(r,g), min(max(r,g), b));
}

// Largeur de la zone d'anti-aliasing en unites de distance MSDF, adaptee a la
// taille reelle a l'ecran : sans ca, un seuil fixe est soit trop fin (bords
// crenetes quand le texte est affiche grand) soit trop large (flou quand petit).
float screenPxRange() {
    vec2 unitRange = vec2(pxRange) / vec2(textureSize(fontTex, 0));
    vec2 screenTexSize = vec2(1.0) / fwidth(uv);
    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

void main() {
    vec3 sample = texture(fontTex, uv).rgb;
    float dist = median(sample.r, sample.g, sample.b);
    float px = screenPxRange();

    float fillAlpha = clamp(px * (dist - 0.5) + 0.5, 0.0, 1.0);
    float outlineAlpha = clamp(px * (dist - (0.5 - outlineWidth)) + 0.5, 0.0, 1.0);

    vec3 color = outlineWidth > 0.0 ? mix(outlineColor, textColor, fillAlpha) : textColor;
    fragColor = vec4(color, outlineAlpha);
}