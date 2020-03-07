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

	float screenspacex = pIn.ScreenPos.x / pIn.ScreenPos.w;
	float screenspacey = pIn.ScreenPos.y / pIn.ScreenPos.w;

	float x = (1 + screenspacex) / 2 + (0.5 / 800);
	float y = (1 - screenspacey) / 2 + (0.5 / 600);

	float depthWorld = DepthMap.Sample(TexSampler, float2(x,y)).r;
	float depthWater = 1 - (pIn.Pos.z / pIn.Pos.w);

	// We are only interested in the depth here
	float4 ndcCoords = float4(0, 0, depthWorld, 1.0f);

	// Unproject the vector into (homogenous) view-space vector
	float4 viewCoords = mul(pIn.viewProjInv, ndcCoords);

	// Divide by w, which results in actual view-space z value
	float linearDepth = viewCoords.z / viewCoords.w;

	float4 shallowColor = float4(1.0f, 0.0f ,0.0f, 1.0f);
	float4 deepColor = float4(0.0f, 0.0f, 1.0f, 1.0f);

	float depth = linearDepth;

	float4 finalColor = lerp(shallowColor, deepColor, saturate(depth));

	//return finalColor;

	return float4(0, 0, 0,0.5f);

}
