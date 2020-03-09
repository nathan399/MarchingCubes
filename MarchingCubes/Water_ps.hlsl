Texture2D    DiffuseMap1 : register(t0); 
Texture2D    DiffuseMap2 : register(t1);
Texture2D    NormalHeightMap : register(t2);
SamplerState TexSampler  : register(s0);

cbuffer CameraPosConstantBuffer
{
	float3 CameraPos;
};

cbuffer WaterMovementConstantBuffer
{
	float WaterMovement;
};

struct PS_INPUT
{
	float4 Pos      : SV_POSITION;
	float4 ScreenPos : TEXCOORD;
	float4 WorldPos : POSITION;
	float3 Normal   : NORMAL;
	float4x4 viewProjInv : MATRIX;
};

// Water normal map/height map is sampled four times at different sizes, then overlaid to give a complex wave pattern
static const float WaterSize1 = 0.5f;
static const float WaterSize2 = 1.0f;
static const float WaterSize3 = 2.0f;
static const float WaterSize4 = 4.0f;

// Each wave layer moves at different speeds so it animates in a varying manner
static const float WaterSpeed1 = 0.5f;
static const float WaterSpeed2 = 1.0f;
static const float WaterSpeed3 = 1.7f;
static const float WaterSpeed4 = 2.6f;

float4 main(in PS_INPUT pIn) : SV_Target
{
	//float screenspacex = pIn.ScreenPos.x / pIn.ScreenPos.w;
	//float screenspacey = pIn.ScreenPos.y / pIn.ScreenPos.w;
	//float x = (1 + screenspacex) / 2 + (0.5 / 800);
	//float y = (1 - screenspacey) / 2 + (0.5 / 600);
	//float depthWorld = DepthMap.Sample(TexSampler, float2(x,y)).r;
	//float depthWater = 1 - (pIn.Pos.z / pIn.Pos.w);
	//// We are only interested in the depth here
	//float4 ndcCoords = float4(0, 0, depthWorld, 1.0f);
	//// Unproject the vector into (homogenous) view-space vector
	//float4 viewCoords = mul(pIn.viewProjInv, ndcCoords);
	//// Divide by w, which results in actual view-space z value
	//float linearDepth = viewCoords.z / viewCoords.w;
	//float4 shallowColor = float4(1.0f, 0.0f ,0.0f, 1.0f);
	//float4 deepColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
	//float depth = linearDepth;
	//float4 finalColor = lerp(shallowColor, deepColor, saturate(depth));
	//return finalColor;

	float2 scale = float2(0.01f, 0.01f);
	float speed = 0.06f;

	float3 normal1 = NormalHeightMap.Sample(TexSampler, (pIn.WorldPos.xz * scale + WaterMovement * speed)).rgb * 2.0f - 1.0f;
	float3 normal2 = NormalHeightMap.Sample(TexSampler, (pIn.WorldPos.xz * scale + WaterMovement * speed)).rgb * 2.0f - 1.0f;
	float3 normal3 = NormalHeightMap.Sample(TexSampler, (pIn.WorldPos.xz * scale + WaterMovement * speed)).rgb * 2.0f - 1.0f;
	float3 normal4 = NormalHeightMap.Sample(TexSampler, (pIn.WorldPos.xz * scale + WaterMovement * speed)).rgb * 2.0f - 1.0f;

	normalize(normal1);
	normalize(normal2);
	normalize(normal3);
	normalize(normal4);

	float3 norm = normalize(normal1 + normal2 + normal3 + normal4);

	float3 waterNormal = float3(norm.x, norm.z, norm.y); // Not this, read comment above

	waterNormal = normalize(waterNormal);  // Final normalization for above line

	float3 LightColour = float3(0.33,0.33,0.33);
	float3 normal = pIn.Normal;

	float3 color = float3(0, 0, 0.1);

	float3 normalToCamera = normalize(CameraPos - pIn.WorldPos);

	float3 LightPos = float3(1, 1, 1);

	float3 vectorToLight = LightPos;//LightPos - pIn.WorldPos;
	float3 normalToLight = normalize(vectorToLight);
	float3 halfwayVector = normalize(normalToLight + normalToCamera);
	float3 specularLight1 = LightColour * pow(max(dot(waterNormal, halfwayVector), 0), 256/*SpecularPower*/);



	return float4(color + specularLight1,0.2f);

}
