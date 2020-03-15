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

struct ObjectTextures
{
	ID3D11Resource* mpAlbedoMap = nullptr;
	ID3D11ShaderResourceView* mpAlbedoMapSRV = nullptr;
	ID3D11Resource* mpRoughnessMap = nullptr;
	ID3D11ShaderResourceView* mpRoughnessMapSRV = nullptr;
	ID3D11Resource* mpMetalnessMap = nullptr;
	ID3D11ShaderResourceView* mpMetalnessMapSRV = nullptr;
	ID3D11Resource* mpNormalMap = nullptr;
	ID3D11ShaderResourceView* mpNormalMapSRV = nullptr;
	ID3D11Resource* mpHeightMap = nullptr;
	ID3D11ShaderResourceView* mpHeightMapSRV = nullptr;
};

class Terrain
{
public:
	Terrain(ChunkSize size);
	~Terrain() {};

	void setUp(ID3D11DeviceContext* context, DX::DeviceResources* deviceResources);
	void generateTerrain(float pointDistance,float frequency,int GridSize,bool interpolate, float surfaceLevel);
	void UpdateCubes();
	void AffectMesh(Vector3 pos,bool direction, float radius, int type);
	void Smooth(Vector3 pos, float radius, int type);
	void Flatten(Vector3 pos, float radius, int type);
	void SetBuffers();
	void sendData(Matrix viewProj, Matrix viewProjInv, float frameTime, Vector3 CameraPos);
	void render(Matrix viewProj, Matrix viewProjInv, bool Wireframe, float frameTime, Vector3 CameraPos);

	bool RayCast(Vector3& Pos, Vector3 Direction, float RayRadius, int RayCastLoops, int type);
	
	

private:
	int Chunks = 10;
	ChunkSize Chunk;
	ID3D11InputLayout* mpVertexLayout = NULL;
	ID3D11Buffer* mVertexBuffer = NULL;
	ID3D11Buffer* mpConstantBuffer = NULL;
	ID3D11Buffer* mpWaterConstantBuffer = NULL;
	ID3D11Buffer* mpCameraConstantBuffer = NULL;

	ID3D11VertexShader* mpVertexShader = NULL;
	ID3D11PixelShader* mpPixelShader = NULL;

	ObjectTextures SurfaceTextures; 
	ObjectTextures EdgeTextures;
	ID3D11Resource* mpWaterHeightMap = nullptr; 
	ID3D11ShaderResourceView* mpWaterHeightMapSRV = nullptr;

	ID3D11SamplerState* mpTextureSampler = nullptr;

	ID3D11DeviceContext* mpContext = NULL;
	DX::DeviceResources* mpDeviceResources = nullptr;

	Matrix world;

	vector <MarchingCubes> Cubes;
	std::vector<CUSTOMVERTEX> Vertices;
	std::unique_ptr<DirectX::CommonStates> States;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>  DepthTex = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> DepthSrv = nullptr;
	
	Neighbours GetNeightbours(int cubeNum);

	struct ConstantBuffer
	{
		Matrix ViewProj;
		Matrix World;
		Matrix ViewProjInv;
	};

	struct WaterConstantBuffer
	{
		float WaterMovement;
		float Pad1;
		float Pad2;
		float Pad3;
	};

	struct CameraConstantBuffer
	{
		float x;
		float y;
		float z;
		float Pad1;
	};

	
};

