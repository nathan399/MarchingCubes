Texture2D	 AlbedoMap1  : register(t0);
Texture2D	 RoughnessMap1: register(t1);
Texture2D	 MetalnessMap1: register(t2);
Texture2D	 NormalMap1: register(t3);
Texture2D	 HeightMap1: register(t4);

Texture2D	 AlbedoMap2: register(t5);
Texture2D	 RoughnessMap2: register(t6);
Texture2D	 MetalnessMap2: register(t7);
Texture2D	 NormalMap2: register(t8);
Texture2D	 HeightMap2: register(t9);

Texture2D    WaterHeightMap : register(t10);
SamplerState TexSampler : register(s0);


cbuffer CameraPosConstantBuffer
{
	float3 CameraPos;
};

struct PS_INPUT
{
	float4 Pos      : SV_POSITION;
	float4 ScreenPos : TEXCOORD;
	float4 WorldPos : POSITION;
	float3 Normal   : NORMAL;
};

static const float3 WaterExtinction = float3(15, 75, 300);
static const float  WaterDiffuseLevel = 0.5f;
static const float  MaxDistortionDistance = 2;
static const float PI = 3.14159265359f;

//float3 SampleNormal(float3 normal, float3 tangent, float2 uv, float2 uv2, float lerpValue)
//{
//	// Calculate inverse tangent matrix
//	float3 biTangent = cross(normal, tangent);
//	float3x3 invTangentMatrix = float3x3(tangent, biTangent, normal);
//
//	// Extract normal from map and shift to -1 to 1 range
//
//	float3 Normal1 = NormalMap1.Sample(TexSampler, uv2).rgb;
//	float3 Normal2 = NormalMap2.Sample(TexSampler, uv).rgb;
//	float3 FinalNormal = Normal1 * (1 - lerpValue) + Normal2 * lerpValue;
//
//	float3 textureNormal = 2.0f * FinalNormal - 1.0f;
//	textureNormal.y = -textureNormal.y;
//
//	
//
//	// Convert normal from tangent space to world space
//	return normalize(mul(mul(textureNormal, invTangentMatrix), (float3x3)WorldMatrix));
//}

float4 main(in PS_INPUT pIn) : SV_Target
{
	float3 LightColour = float3(0.33,0.33,0.33);
	float3 AmbientColour = float3(0.1, 0.15, 0.1);
	float3 LightPos = float3(1, 1, 1);
	float3 LightDir = LightPos;//normalize(LightPos - pIn.Pos.xyz);   // Direction for each light is different
	float  LightDist = length(pIn.Normal - LightPos);

	float2 uv = float2(0,0); 
	
	float2 uv2 = float2(0, 0); 
	uv2.x = pIn.WorldPos.y / 80;
	uv2.y = pIn.WorldPos.y / 80;

	
	uv = float2(pIn.WorldPos.x / 20 + uv.x, pIn.WorldPos.z / 20 + uv.y);	
	uv2 = float2(pIn.WorldPos.x / 20 + uv2.x, pIn.WorldPos.z / 20 + uv2.y);

	float3 abledo1 = AlbedoMap1.Sample(TexSampler, uv2).rgb;
	float3 abledo2 = AlbedoMap2.Sample(TexSampler, uv).rgb;

	float3 Roughness1 = RoughnessMap1.Sample(TexSampler, uv2).rgb;
	float3 Roughness2 = RoughnessMap2.Sample(TexSampler, uv).rgb;

	float3 Metalness1 = MetalnessMap1.Sample(TexSampler, uv2).rgb;
	float3 Metalness2 = MetalnessMap2.Sample(TexSampler, uv).rgb;

	float3 Normal1 = NormalMap1.Sample(TexSampler, uv2).rgb;
	float3 Normal2 = NormalMap2.Sample(TexSampler, uv).rgb;
	

	//get lerp value
	float lerpV = 0;
	lerpV = pIn.Normal.y * 2 - 1;
	lerpV = clamp(lerpV, 0, 1);

	float3  FinalAlbedo = abledo1 * (1 - lerpV) + abledo2 * lerpV;
	float3  FinalRoughness = Roughness1 * (1 - lerpV) + Roughness2 * lerpV;
	float3  FinalMetalness = Metalness1 * (1 - lerpV) + Metalness2 * lerpV;
	float3  FinalNormal = Normal1 * (1 - lerpV) + Normal2 * lerpV;

	float3 specularColour = lerp(float3(0.04f, 0.04f, 0.04f), FinalAlbedo, FinalMetalness); // Select specular color based on metalness
	float3 color = FinalAlbedo + AmbientColour;

	float3 n = FinalNormal;
	float3 v = normalize(CameraPos - pIn.WorldPos);
	//pbr code
	float3 l = LightDir;
	float rdist = 1 / length(l);
	l *= rdist;
	float  li = 3;
	float3 lc = LightColour;

	// Halfway vector (normal halfway between view and light vector)
	float3 h = normalize(l + v);

	// Various dot products used throughout
	float nDotV = max(dot(n, v), 0.001f);
	float nDotL = max(dot(n, l), 0.001f);
	float nDotH = max(dot(n, h), 0.001f);

	// Lambert diffuse
	float3 lambert = FinalAlbedo / PI;

	// Microfacet specular - fresnel term
	float3 F = specularColour + (1 - specularColour) * pow(max(1.0f - nDotH, 0.0f), 5.0f);

	// Microfacet specular - normal distribution term
	float alpha = max(FinalRoughness * FinalRoughness, 2.0e-3f); // Dividing by alpha in the dn term so don't allow it to reach 0
	float alpha2 = alpha * alpha;
	float nDotH2 = nDotH * nDotH;
	float dn = nDotH2 * (alpha2 - 1) + 1;
	float D = alpha2 / (PI * dn * dn);

	// Microfacet specular - geometry term
	float k = (FinalRoughness + 1);
	k = k * k / 8;
	float gV = nDotV / (nDotV * (1 - k) + k);
	float gL = nDotL / (nDotL * (1 - k) + k);
	float G = gV * gL;

	// Full brdf, diffuse + specular
	float3 brdf = lambert + F * G * D / (4 * nDotL * nDotV);

	// Accumulate punctual light equation for this light
	color += PI * li * lc * brdf * nDotL;



	//water effect code
	float screenspacex = pIn.ScreenPos.x / pIn.ScreenPos.w;
	float screenspacey = pIn.ScreenPos.y / pIn.ScreenPos.w;

	float x = (1 + screenspacex) / 2 + (0.5 / 800);
	float y = (1 - screenspacey) / 2 + (0.5 / 600);
	float2 screenUV = float2(x, y);
	float waterHeight = WaterHeightMap.Sample(TexSampler, screenUV );
	float objectDepth = waterHeight - pIn.WorldPos.y;
	float3 depthDarken = saturate(objectDepth / WaterExtinction);
	float3 refractionColour = lerp(color, normalize(WaterExtinction) * WaterDiffuseLevel, depthDarken);

	//final color

	return float4(refractionColour, saturate(objectDepth / MaxDistortionDistance));
}