#version 330 core

//data du vertex Shader
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
in vec4 VertexColor;
in mat3 TBN;

out vec4 FragColor;

uniform sampler2D normalMap;
uniform sampler2D baseColorTex;

uniform bool hasNormal;
uniform bool hasBaseColorTexture;
uniform bool hasVertexColor;

uniform vec4 baseColorFactor;

void main() {
	vec3 N = normalize(Normal);

	if (hasNormal)
	{
		vec3 normalTex = texture(normalMap, TexCoord).rgb;
		normalTex = normalTex * 2.0 - 1.0;
		N = normalize(TBN * normalTex);
	}

	vec4 baseColor = vec4(1.0);

	if (hasBaseColorTexture)
		baseColor *= texture(baseColorTex, TexCoord);

	if (hasVertexColor)
		baseColor *= VertexColor;

	baseColor *= baseColorFactor;

	FragColor = baseColor;
}


