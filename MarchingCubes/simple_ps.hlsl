Texture2D    DiffuseMap1 : register(t0); 
Texture2D    DiffuseMap2 : register(t1);
Texture2D    WaterHeightMap : register(t2);
SamplerState TexSampler : register(s0);

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

float4 main(in PS_INPUT pIn) : SV_Target
{
	float3 LightColour = float3(0.33,0.33,0.33);
	float3 AmbientColour = float3(0.1, 0.15, 0.1);
	float3 LightPos = float3(1, 1, 1);
	float3 LightDir = LightPos;//normalize(LightPos - pIn.Pos.xyz);   // Direction for each light is different
	float  LightDist = length(pIn.Normal - LightPos);
	float3 DiffuseLight1 = (LightColour * saturate(dot(pIn.Normal, LightDir))); /// Light1Dist;

	float2 uv = float2(0,0); 
	
	float2 uv2 = float2(0, 0); 
	uv2.x = pIn.WorldPos.y / 80;
	uv2.y = pIn.WorldPos.y / 80;

	
	uv = float2(pIn.WorldPos.x / 20 + uv.x, pIn.WorldPos.z / 20 + uv.y);	
	uv2 = float2(pIn.WorldPos.x / 20 + uv2.x, pIn.WorldPos.z / 20 + uv2.y);

	float3 diffuseMapColour1 = DiffuseMap1.Sample(TexSampler, uv2).rgb;;
	float3 diffuseMapColour2 = DiffuseMap2.Sample(TexSampler, uv).rgb;;

	float lerpV = 0;
	
	lerpV = pIn.Normal.y * 2 - 1;

	lerpV = clamp(lerpV, 0, 1);

	float3  diffuseMapColour = diffuseMapColour1 * (1 - lerpV) + diffuseMapColour2 * lerpV;

	float3 color = diffuseMapColour + DiffuseLight1 + AmbientColour;

	float screenspacex = pIn.ScreenPos.x / pIn.ScreenPos.w;
	float screenspacey = pIn.ScreenPos.y / pIn.ScreenPos.w;

	float x = (1 + screenspacex) / 2 + (0.5 / 800);
	float y = (1 - screenspacey) / 2 + (0.5 / 600);
	float2 screenUV = float2(x, y);
	float waterHeight = WaterHeightMap.Sample(TexSampler, screenUV );
	float objectDepth = waterHeight - pIn.WorldPos.y;
	float3 depthDarken = saturate(objectDepth / WaterExtinction);
	float3 refractionColour = lerp(color, normalize(WaterExtinction) * WaterDiffuseLevel, depthDarken);

	//return //pIn.Pos.z / pIn.Pos.w;  
	//float4(diffuseMapColour + DiffuseLight1 + AmbientColour,1);    

	return float4(refractionColour, saturate(objectDepth / MaxDistortionDistance));
}