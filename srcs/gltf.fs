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

uniform float metallicFactor;
uniform float roughnessFactor;
uniform vec3 emissiveFactor;
uniform float occlusionStrength;

uniform vec3 lightDir;

uniform vec3 viewPos;

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

		// float NdotNbase = dot(N, Nbase);
		// if (NdotNbase < 0.0) N = normalize(N - NdotNbase * Nbase);
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

	// vec3 irradiance = texture(irradianceMap, N).rgb; //irradiance normale avec couleur de la skybox
	vec3 irradiance = vec3(1.0); //irradiance blanche pour tester
	
	vec3 ambient = irradiance * baseColor.rgb;
	float metallic = 1.0;
	float roughness = 1.0;
	if (hasMetallicRoughnessTexture) {
		metallic = texture(metallicRougnessTex, TexCoord).b;
		roughness = texture(metallicRougnessTex, TexCoord).g;
		float ao = 1.0; //ambiant occlusion
		if (hasOcclusionTexture)
			ao = texture(occlusionTex, TexCoord).r;
		ambient = irradiance * baseColor.rgb * ao;
	}

	vec3 V = normalize(viewPos - FragPos);
	vec3 H = normalize(L + V);

	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float NdotH = max(dot(N, H), 0.0);
	float denom = NdotH * NdotH * (alpha2 - 1.0) + 1.0;
	float D = max(0.001, alpha2 / (3.141592 * denom * denom));

	vec3 F0 = mix(vec3(0.04), baseColor.rgb, metallic);
	vec3 F = F0 + (1.0 - F0) * pow(1.0 - max(dot(H, V), 0.0), 5.0);

	float k = (roughness + 1.0)	* (roughness + 1.0) / 8.0;
	float NdotV = max(dot(N, V), 0.001);
	float NdotL_val = max(dot(N, L), 0.001);

	float ggx1 = NdotV / (NdotV * (1.0 - k) + k);
	float ggx2 = NdotL_val / (NdotL_val * (1.0 - k) + k);
	float G = ggx1 * ggx2;

	vec3 emissive = texture(emissiveTex, TexCoord).rgb * emissiveFactor;

	vec3 specular = (F * D * G) / (4.0 * NdotV * NdotL_val + 0.001);
	vec3 diffuse = baseColor.rgb / 3.141592;

	vec3 Lo = (diffuse + specular) * vec3(lightIntensity) * NdotL  + ambient + emissive; //Outgoing radiance

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


