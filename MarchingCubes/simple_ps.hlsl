Texture2D    DiffuseMap1 : register(t0); 
Texture2D    DiffuseMap2 : register(t1);
SamplerState TexSampler : register(s0);

struct PS_INPUT
{
	float4 Pos      : SV_POSITION;
	float4 WorldPos : POSITION;
	float3 Normal   : NORMAL;
};

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

	if (abs(pIn.Normal.x) > 0.5)
		uv2.x = pIn.WorldPos.y / 20;

	else if (abs(pIn.Normal.z) > 0.5)
		uv2.y = pIn.WorldPos.y / 20;

	
	uv = float2(pIn.WorldPos.x / 20 + uv.x, pIn.WorldPos.z / 20 + uv.y);	
	uv2 = float2(pIn.WorldPos.x / 20 + uv2.x, pIn.WorldPos.z / 20 + uv2.y);

	float3 diffuseMapColour1 = DiffuseMap1.Sample(TexSampler, uv2).rgb;;
	float3 diffuseMapColour2 = DiffuseMap2.Sample(TexSampler, uv).rgb;;

	float lerpV = 1;
	
	lerpV -= abs(pIn.Normal.y);

	float3  diffuseMapColour = diffuseMapColour1 * lerpV + diffuseMapColour2 * (1 - lerpV);

	return //pIn.Pos.z / pIn.Pos.w;  
	float4(diffuseMapColour + DiffuseLight1 + AmbientColour,1);    // Yellow, with Alpha = 1
}