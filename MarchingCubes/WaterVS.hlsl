//--------------------------------------------------------------------------------------
// Simple Vertex Shader
//--------------------------------------------------------------------------------------
// Shaders - we won't look at shaders until later in the module, but they are needed to
// render anything. Very simple shaders are used in this tutorial

Texture2D NormalHeightMap : register(t0);
SamplerState TexSampler  : register(s0);

cbuffer ConstantBuffer
{
	row_major matrix viewProj;
	row_major matrix world;
	row_major matrix viewProjInv;
};

cbuffer WaterConstantBuffer
{
	float WaterMovement;
};

struct VS_INPUT
{
	float3 Pos    : POSITION;
	float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 Pos    : SV_POSITION;
	float4 ScreenPos : TEXCOORD;
	float4 WorldPos : POSITION;
	float3 Normal : NORMAL;
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

void main(in VS_INPUT vIn,out VS_OUTPUT vOut)
{
	float4 modelPosition = float4(vIn.Pos, 1.0f);
	float4 worldPos = mul(modelPosition, world);

	float2 scale = float2(0.01f, 0.01f);
	float speed = 0.06f;

	if (vIn.Normal.y > 0.0f)
	{
		float height1 = NormalHeightMap.SampleLevel(TexSampler, (worldPos.xz * scale + WaterMovement * speed), 0).a;
		float height2 = NormalHeightMap.SampleLevel(TexSampler, (worldPos.xz * scale + WaterMovement * speed), 0).a;
		float height3 = NormalHeightMap.SampleLevel(TexSampler, (worldPos.xz * scale + WaterMovement * speed), 0).a;
		float height4 = NormalHeightMap.SampleLevel(TexSampler, (worldPos.xz * scale + WaterMovement * speed), 0).a;

		float height = height1 + height2 + height3 + height4;
		float yplus = (0.25f * height - 0.5f) * 5; // -0.5 makes wave movement an equal amount up or down from basic water height
		modelPosition.y += yplus;
	}
	worldPos = mul(modelPosition, world);

	vOut.WorldPos = worldPos;
    vOut.Pos = mul(worldPos,viewProj);
	vOut.ScreenPos = vOut.Pos;
	vOut.Normal = vIn.Normal;
	vOut.viewProjInv = viewProjInv;
}
