//--------------------------------------------------------------------------------------
// Simple Vertex Shader
//--------------------------------------------------------------------------------------
// Shaders - we won't look at shaders until later in the module, but they are needed to
// render anything. Very simple shaders are used in this tutorial

cbuffer ConstantBuffer
{
	row_major matrix viewProj;
	row_major matrix world;
};

struct VS_INPUT
{
	float3 Pos    : POSITION;
	float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 Pos    : SV_POSITION;
	float4 WorldPos : POSITION;
	float3 Normal : NORMAL;
};

void main(in VS_INPUT vIn,out VS_OUTPUT vOut)
{
	float4 worldPos = mul(float4(vIn.Pos,1), world);
	vOut.WorldPos = worldPos;
    vOut.Pos = mul(worldPos,viewProj);
	vOut.Normal = vIn.Normal;
}
