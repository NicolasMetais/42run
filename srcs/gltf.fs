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
uniform samplerCube irradianceMap; //CUBEMAP LIGHTNING

uniform bool hasNormalMap;
uniform bool hasBaseColorTexture;
uniform bool hasVertexColor;

uniform vec4 baseColorFactor;

uniform float lightIntensity;
uniform vec3 lightDir;

void main() {
	vec3 Nbase = normalize(Normal);
	if (!gl_FrontFacing) Nbase = -Nbase;

	vec3 N = Nbase;
	if (hasNormalMap)
	{
		vec3 normalTex = texture(normalMap, TexCoord).rgb;
		N = normalize(TBN * normalTex);

		float NdotNbase = dot(N, Nbase);
		if (NdotNbase < 0.0) N = normalize(N - NdotNbase * Nbase);
	// FragColor = vec4(normalTex, 1.0); // affiche la normal map brute sans transformation

	}

	vec4 baseColor = vec4(1.0);

	if (hasBaseColorTexture)
		baseColor *= texture(baseColorTex, TexCoord);

	if (hasVertexColor)
		baseColor *= VertexColor;
	baseColor *= baseColorFactor;

	vec3 L = normalize(-lightDir);
	float NdotL = max(dot(N, L), 0.0); //LIGHTANGLE DECONNE MAIS JE CROIS QUE CA REVELE UN BUG DE NORMAL ? MAYBE

	// vec3 radiance = vec3(lightIntensity); // radiance
	vec3 irradiance = texture(irradianceMap, N).rgb;

	vec3 ambient = irradiance * baseColor.rgb;

	vec3 diffuse = baseColor.rgb / 3.141592;

	vec3 Lo = diffuse * 0.1 * vec3(lightIntensity) * NdotL + ambient; //Outgoing radiance

	FragColor = vec4(Lo, baseColor.a);
	// FragColor = vec4(TBN[2] * 0.5 + 0.5, 1.0); // visualise T

}


