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
	return float4(0,0,1,0.5f);
}