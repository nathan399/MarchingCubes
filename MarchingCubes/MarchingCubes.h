#pragma once
#include "DeviceResources.h"
#include "FastNoise.h"
#include <SimpleMath.h>
#include <vector>
#include <CommonStates.h>
#include <SimpleMath.h>
#include "Shader.h"

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;
using std::vector;

class MarchingCubes;

enum EarthTypes {earth , water};

struct CUSTOMVERTEX
{
	float x, y, z; //position
	float nx, ny, nz; // normal
};

struct SVertices
{
	Vector3 pos;
	Vector3 normal;
};

struct PointData
{
	Vector3 pos;
	Vector3 normal;
	float value[2];
};

struct Neighbours
{
	MarchingCubes* Left;
	MarchingCubes* Right;
	MarchingCubes* Up;
	MarchingCubes* Down;
	MarchingCubes* Forward;
	MarchingCubes* Back;
};

struct sEdges
{
	bool xMax = false;
	bool xMin = false;
	bool yMax =	false;
	bool yMin = false;
	bool zMax =	false;
	bool zMin =	false;
};			  

class MarchingCubes
{
public:
	MarchingCubes(ID3D11DeviceContext* context,Vector3 pos, sEdges edges, float pointDistance = 1, float frequency = 1, int GridSize = 10, bool interpolate = true);
	~MarchingCubes() {};
	std::vector<SVertices> getVertices() { return vertices; }
	void generate(float pointDistance, float frequency, int GridSize, bool interpolate, float surfaceLevel);
	void CreateMesh(int type);
	void UpdateWater();
	void AffectPoints(Vector3 pos, int direction, float radius,int type);
	void Smooth(Vector3 pos, float radius, int type);
	void Flatten(Vector3 pos, float radius, int type);
	bool CubeToSphere(Vector3 sPos, float radius);
	bool GetSurfacePoint(Vector3& pos, float Radius, int type);
	void SetBuffer(int type);
	void RenderEarth(ID3D11RasterizerState* state, ID3D11BlendState* BlendState, ID3D11DepthStencilState* DepthState);
	void RenderWater(ID3D11RasterizerState* state, ID3D11BlendState* BlendState, ID3D11DepthStencilState* DepthState , bool HeightRender = false);
	float GetValueAt(float x, float y, float z, int type) {return Points[x][y][z].value[type];}
	

	Neighbours neighbours;
	bool MeshChanged = false;

private:
	ID3D11InputLayout* mpVertexLayout = NULL;
	ID3D11Buffer* mVertexBuffer[2] = { NULL,NULL };
	ID3D11Buffer* mpConstantBuffer = NULL;

	ID3D11VertexShader* mpVertexShader = NULL;
	ID3D11PixelShader* mpPixelShader = NULL;
	ID3D11PixelShader* mpWaterPixelShader = NULL;
	ID3D11PixelShader* mpWaterHeightPixelShader = NULL;

	ID3D11DeviceContext* mpContext = NULL;

	struct ConstantBuffer
	{
		Matrix ViewProj;
		Matrix World;
	};

	FastNoise Noise;
	vector <vector< vector< PointData>>> Points;
	vector<SVertices> vertices;
	vector<SVertices> WaterVertices;
	float SurfaceLevel = 0;
	int gridSize = 100;
	bool Interpolate = true;

	float cubesradius;

	const float MaxValue = 0.5f;
	const float MinValue = -MaxValue;
	Vector3 Pos;
	Vector3 center;

	sEdges EdgeState;

	void CalculateCubesVerticies(PointData edge[8], int type);
	void CalculateCubeNormals(int type);
	SVertices CalculateMid(const PointData& p1, const PointData& p2,int type);
	Vector3 Cross(const Vector3& v1, const Vector3& v2);
	Vector3 Normalise(const Vector3& v);
	

	
};


