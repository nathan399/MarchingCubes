Texture2D    DiffuseMap1 : register(t0); 
Texture2D    DiffuseMap2 : register(t1);
Texture2D    DepthMap	 : register(t2);
SamplerState TexSampler  : register(s0);

struct PS_INPUT
{
	float4 Pos      : SV_POSITION;
	float4 ScreenPos : TEXCOORD;
	float4 WorldPos : POSITION;
	float3 Normal   : NORMAL;
};

float4 main(in PS_INPUT pIn) : SV_Target
{

	float screenspacex = pIn.ScreenPos.x / pIn.ScreenPos.w;
	float screenspacey = pIn.ScreenPos.y / pIn.ScreenPos.w;

	float x = (1 + screenspacex) / 2 + (0.5 / 800);
	float y = (1 - screenspacey) / 2 + (0.5 / 600);

	float depth = pow(DepthMap.Sample(TexSampler, float2(x,y)).r, 1000);

	return float4(depth, depth, depth, 1.0);//float4(0,0,1,0.5f);
}
