#version 330 core

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
in vec4 VertexColor;
in mat3 TBN;

out vec4 FragColor;

uniform sampler2D normalMap;
uniform sampler2D baseColorTex;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D metallicRougnessTex;
uniform sampler2D occlusionTex;
uniform sampler2D emissiveTex;

uniform bool hasNormalMap;
uniform bool hasBaseColorTexture;
uniform bool hasOcclusionTexture;
uniform bool hasEmissiveTexture;
uniform bool hasMetallicRoughnessTexture;
uniform bool hasVertexColor;

uniform vec4  baseColorFactor;
uniform float lightIntensity;
uniform float metallicFactor;
uniform float roughnessFactor;
uniform vec3  emissiveFactor;
uniform float occlusionStrength;
uniform vec3  lightDir;
uniform float normalScale;
uniform vec3  viewPos;
uniform bool  debugMode;

// ─── Normal ───────────────────────────────────────────────────────────────────

vec3 getNormal(mat3 tbn)
{
	vec3 N = normalize(tbn[2]);
	if (hasNormalMap)
	{
		vec3 normalTex = texture(normalMap, TexCoord).rgb * 2.0 - 1.0;
		normalTex.xy *= normalScale;
		N = normalize(tbn * normalize(normalTex));
	}
	return N;
}

// ─── Base color ───────────────────────────────────────────────────────────────

vec4 getBaseColor()
{
	vec4 color = vec4(1.0);
	if (hasBaseColorTexture)
		color *= texture(baseColorTex, TexCoord);
	if (hasVertexColor)
		color *= VertexColor;
	color *= baseColorFactor;
	return color;
}

// ─── Material properties ──────────────────────────────────────────────────────

void getMaterial(out float metallic, out float roughness, out float ao)
{
	metallic  = metallicFactor;
	roughness = roughnessFactor;
	ao        = 1.0;
	if (hasOcclusionTexture)
		ao = 1.0 + occlusionStrength * (texture(occlusionTex, TexCoord).r - 1.0);
	if (hasMetallicRoughnessTexture)
	{
		metallic  = texture(metallicRougnessTex, TexCoord).b * metallicFactor;
		roughness = texture(metallicRougnessTex, TexCoord).g * roughnessFactor;
	}
}

// ─── Cook-Torrance BRDF ───────────────────────────────────────────────────────

float D_GGX(float NdotH, float roughness)
{
	float alpha  = roughness * roughness;
	float alpha2 = alpha * alpha;
	float denom  = NdotH * NdotH * (alpha2 - 1.0) + 1.0;
	return max(0.001, alpha2 / (3.141592 * denom * denom));
}

vec3 F_Schlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float G_Smith(float NdotV, float NdotL, float roughness)
{
	float k    = (roughness + 1.0) * (roughness + 1.0) / 8.0;
	float ggx1 = NdotV / (NdotV * (1.0 - k) + k);
	float ggx2 = NdotL / (NdotL * (1.0 - k) + k);
	return ggx1 * ggx2;
}

// ─── IBL ambient ──────────────────────────────────────────────────────────────

vec3 computeAmbient(vec3 N, vec3 R, vec3 kD, vec3 F, vec3 baseColorRGB, float roughness, float ao)
{
	vec3  irradiance      = debugMode ? vec3(1.0) : max(texture(irradianceMap, N).rgb, vec3(0.08));
	float MAX_MIP_LEVEL   = 4.0;
	vec3  prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_MIP_LEVEL).rgb;

	vec3 diffuseIBL  = irradiance * baseColorRGB;
	vec3 specularIBL = prefilteredColor * F;
	return (diffuseIBL * kD + specularIBL) * ao;
}

// ─── Emissive ─────────────────────────────────────────────────────────────────

vec3 getEmissive()
{
	if (hasEmissiveTexture)
		return texture(emissiveTex, TexCoord).rgb * emissiveFactor;
	return vec3(0.0);
}

// ─── Main ─────────────────────────────────────────────────────────────────────

void main()
{
	mat3 tbn = TBN;
	if (!gl_FrontFacing)
		tbn[2] = -tbn[2];

	vec3 N = getNormal(tbn);
	vec4 baseColor = getBaseColor();

	float metallic, roughness, ao;
	getMaterial(metallic, roughness, ao);

	vec3  V      = normalize(viewPos - FragPos);
	vec3  L      = vec3(0.0);
	vec3  H      = normalize(L + V);
	vec3  R      = reflect(-V, N);

	float NdotH  = max(dot(N, H), 0.0);
	float NdotV  = max(dot(N, V), 0.001);
	float NdotL  = max(dot(N, L), 0.001);

	vec3 F0 = mix(vec3(0.04), baseColor.rgb, metallic);

	float D = D_GGX(NdotH, roughness);
	vec3  F = F_Schlick(max(dot(H, V), 0.0), F0);
	float G = G_Smith(NdotV, NdotL, roughness);

	vec3 specular = (F * D * G) / (4.0 * NdotV * NdotL + 0.001);

	vec3 kD      = (1.0 - F) * (1.0 - metallic);
	vec3 diffuse = kD * baseColor.rgb / 3.141592;

	vec3 ambient  = computeAmbient(N, R, kD, F, baseColor.rgb, roughness, ao);
	vec3 emissive = getEmissive();

	vec3 Lo = (diffuse + specular) * vec3(lightIntensity) * max(dot(N, L), 0.0)
	        + ambient + emissive;

	FragColor = vec4(Lo, baseColor.a);

	// Debug views (décommenter pour tester)
	// FragColor = vec4(TBN[2] * 0.5 + 0.5, 1.0);
	// FragColor = vec4(TexCoord, 0.0, 1.0);
	// FragColor = texture(baseColorTex, TexCoord);
	// FragColor = vec4(texture(metallicRougnessTex, TexCoord).bgr, 1.0);
	// vec3 dbgN = texture(normalMap, TexCoord).rgb * 2.0 - 1.0; FragColor = vec4(dbgN * 0.5 + 0.5, 1.0);
	// FragColor = vec4(vec3(texture(occlusionTex, TexCoord).r), 1.0);
	// FragColor = texture(emissiveTex, TexCoord);
}
