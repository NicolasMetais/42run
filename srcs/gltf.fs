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
uniform sampler2D metallicRougnessTex;
uniform sampler2D occlusionTex;
uniform sampler2D emissiveTex;


uniform bool hasNormalMap;
uniform bool hasBaseColorTexture;
uniform bool hasOcclusionTexture;
uniform bool hasEmissiveTexture;
uniform bool hasMetallicRoughnessTexture;

uniform bool hasVertexColor;

uniform vec4 baseColorFactor;

uniform float lightIntensity;

uniform float mettalicFactor;
uniform float roughnessFactor;
uniform float emmisiveFactor;
uniform float occlusionStrength;

uniform vec3 lightDir;

void main() {
	mat3 tbn = TBN;

	if (!gl_FrontFacing) {
		tbn[2] = -tbn[2];
		tbn[1] = -tbn[1];
	}

	vec3 Nbase = normalize(tbn[2]);
	vec3 N = Nbase;

	if (hasNormalMap)
	{
		vec3 normalTex = texture(normalMap, TexCoord).rgb * 2.0 - 1.0;
		N = normalize(tbn * normalTex);

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
	float NdotL = max(dot(N, L), 0.0);

	// vec3 radiance = vec3(lightIntensity); // radiance
	vec3 irradiance = texture(irradianceMap, N).rgb;
	
	vec3 ambient = irradiance * baseColor.rgb;

	if (hasMetallicRoughnessTexture) {
		float metallic = texture(metallicRougnessTex, TexCoord).b;
		float roughness = texture(metallicRougnessTex, TexCoord).g;
		float ao = 1.0;
		if (hasOcclusionTexture)
			ao = texture(occlusionTex, TexCoord).r;
		ambient = irradiance * baseColor.rgb * ao;
	}

	vec3 diffuse = baseColor.rgb / 3.141592;

	vec3 Lo = diffuse * vec3(lightIntensity) * NdotL + ambient; //Outgoing radiance

	FragColor = vec4(Lo, baseColor.a);
	// FragColor = vec4(TBN[2] * 0.5 + 0.5, 1.0); // visualise T

	// Test les UV d'abord - ils devraient varier de noir à blanc
    // FragColor = vec4(TexCoord, 0.0, 1.0);
    
    // Test baseColor
    // FragColor = texture(baseColorTex, TexCoord);
    
    // Test metallic/roughness
    // FragColor = vec4(texture(metallicRougnessTex, TexCoord).bgr, 1.0);
    
    // Test normal map
    // vec3 normalTex = texture(normalMap, TexCoord).rgb * 2.0 - 1.0;
    // FragColor = vec4(normalTex * 0.5 + 0.5, 1.0);
    
    // Test occlusion
    // FragColor = vec4(vec3(texture(occlusionTex, TexCoord).r), 1.0);
    
    // Test emissive
    // FragColor = texture(emissiveTex, TexCoord);
    
    // Test irradiance
    // vec3 N = normalize(TBN[2]);
    // vec3 irradiance = texture(irradianceMap, N).rgb;
    // FragColor = vec4(irradiance, 1.0);
}


