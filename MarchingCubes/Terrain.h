#pragma once
#include "DeviceResources.h"
#include "Shader.h"
#include "FastNoise.h"
#include "MarchingCubes.h"
#include <memory>
#include <SimpleMath.h>
#include <vector>
#include <CommonStates.h>

using DirectX::SimpleMath::Matrix;

//struct CUSTOMVERTEX
//{
//	float x, y, z; //position
//	float nx, ny, nz; // normal
//};

class Terrain
{
public:
	Terrain();
	~Terrain() {};

	void setUp(ID3D11DeviceContext* context);
	void generateTerrain(float pointDistance,float frequency,int GridSize,bool interpolate);
	void AffectMesh(Vector3 pos,bool direction, float radius);
	void SetBuffers();
	void sendData(Matrix viewProj);
	void render(Matrix viewProj, bool Wireframe);
	
	

private:
	ID3D11InputLayout* mpVertexLayout = NULL;
	ID3D11Buffer* mVertexBuffer = NULL;
	ID3D11Buffer* mpConstantBuffer = NULL;

	ID3D11VertexShader* mpVertexShader = NULL;
	ID3D11PixelShader* mpPixelShader = NULL;

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

