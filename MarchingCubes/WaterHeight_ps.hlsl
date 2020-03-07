Texture2D    DiffuseMap1 : register(t0); 
Texture2D    DiffuseMap2 : register(t1);
Texture2D    DepthMap	 : register(t2);
SamplerState TexSampler  : register(s0);

cbuffer ConstantBuffer
{
	row_major matrix viewProj;
	row_major matrix world;
	row_major matrix viewProjInv;
};

struct PS_INPUT
{
	float4 Pos      : SV_POSITION;
	float4 ScreenPos : TEXCOORD;
	float4 WorldPos : POSITION;
	float3 Normal   : NORMAL;
	float4x4 viewProjInv : MATRIX;
};

float4 main(in PS_INPUT pIn) : SV_Target
{
	// return y in world space
	return pIn.WorldPos.y;

}
