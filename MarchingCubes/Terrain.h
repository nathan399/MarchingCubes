#pragma once
#include "DeviceResources.h"
#include "Shader.h"
#include "FastNoise.h"
#include "MarchingCubes.h"
#include "WICTextureLoader.h"
#include <memory>
#include <SimpleMath.h>
#include <vector>
#include <CommonStates.h>

using DirectX::SimpleMath::Matrix;

struct ChunkSize
{
	int x;
	int y;
	int z;
};

class Terrain
{
public:
	Terrain(ChunkSize size);
	~Terrain() {};

	void setUp(ID3D11DeviceContext* context);
	void generateTerrain(float pointDistance,float frequency,int GridSize,bool interpolate, float surfaceLevel);
	void AffectMesh(Vector3 pos,bool direction, float radius);
	void Smooth(Vector3 pos, float radius);
	void SetBuffers();
	void sendData(Matrix viewProj);
	void render(Matrix viewProj, bool Wireframe);
	
	

private:
	int Chunks = 10;
	ChunkSize Chunk;
	ID3D11InputLayout* mpVertexLayout = NULL;
	ID3D11Buffer* mVertexBuffer = NULL;
	ID3D11Buffer* mpConstantBuffer = NULL;

	ID3D11VertexShader* mpVertexShader = NULL;
	ID3D11PixelShader* mpPixelShader = NULL;

	ID3D11Resource* mpDiffuseMap = nullptr; // This object represents the memory used by the texture on the GPU
	ID3D11ShaderResourceView* mpDiffuseMapSRV = nullptr; // This object is used to give shaders access to the texture above (SRV = shader resource view)
	ID3D11Resource* mpDiffuseMap2 = nullptr; // This object represents the memory used by the texture on the GPU
	ID3D11ShaderResourceView* mpDiffuseMapSRV2 = nullptr; // This object is used to give shaders access to the texture above (SRV = shader resource view)

	ID3D11SamplerState* mpTextureSampler = nullptr;

	ID3D11DeviceContext* mpContext = NULL;

	Matrix world;

	vector <MarchingCubes> Cubes;

	std::vector<CUSTOMVERTEX> Vertices;

	std::unique_ptr<DirectX::CommonStates> States;

	

	struct ConstantBuffer
	{
		Matrix ViewProj;
		Matrix World;
	};
};

