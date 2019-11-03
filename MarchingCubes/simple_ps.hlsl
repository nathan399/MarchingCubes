//--------------------------------------------------------------------------------------
// Simple Pixel Shader
//--------------------------------------------------------------------------------------
// Shaders - we won't look at shaders until later in the module, but they are needed to
// render anything. Very simple shaders are used in this tutorial

struct PS_INPUT
{
	float4 Pos    : SV_POSITION;
	float3 Normal : NORMAL;
};

float4 main(in PS_INPUT pIn) : SV_Target
{
	float3 LightColour = float3(0.33,0.66,0.33);
	float3 AmbientColour = float3(0.1, 0.15, 0.1);
	float3 LightPos = float3(1, 1, 1);
	float3 LightDir = LightPos;//normalize(LightPos - pIn.Pos.xyz);   // Direction for each light is different
	float  LightDist = length(pIn.Normal - LightPos);
	float3 DiffuseLight1 = (LightColour * saturate(dot(pIn.Normal, LightDir))); /// Light1Dist;
	
	return //pIn.Pos.z / pIn.Pos.w;  
	float4(DiffuseLight1 + AmbientColour,1);    // Yellow, with Alpha = 1
}