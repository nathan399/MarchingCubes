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
	float value;
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

class MarchingCubes
{
public:
	MarchingCubes(ID3D11DeviceContext* context,Vector3 pos, bool xmax, bool xmin, bool ymax, bool ymin, bool zmax, bool zmin, float pointDistance = 1, float frequency = 1, int GridSize = 10, bool interpolate = true);
	~MarchingCubes() {};
	std::vector<SVertices> getVertices() { return vertices; }
	void generate(float pointDistance, float frequency, int GridSize, bool interpolate);
	void CreateMesh();
	void AffectPoints(Vector3 pos, int direction, float radius);
	void Smooth(Vector3 pos, int direction, float radius, Neighbours neighbours);
	bool CubeToSphere(Vector3 sPos, float radius);
	void SetBuffer();
	void Render(ID3D11RasterizerState* state);
	float GetValueAt(float x, float y, float z) {return Points[x][y][z].value;}
	
private:
	ID3D11InputLayout* mpVertexLayout = NULL;
	ID3D11Buffer* mVertexBuffer = NULL;
	ID3D11Buffer* mpConstantBuffer = NULL;

	ID3D11VertexShader* mpVertexShader = NULL;
	ID3D11PixelShader* mpPixelShader = NULL;

	ID3D11DeviceContext* mpContext = NULL;

	struct ConstantBuffer
	{
		Matrix ViewProj;
		Matrix World;
	};

	//std::unique_ptr<DirectX::CommonStates> States;

	FastNoise Noise;
	vector <vector< vector< PointData>>> Points;
	vector<SVertices> vertices;
	float SurfaceLevel = 0;
	int gridSize = 100;
	bool Interpolate = true;

	float cubesradius;

	const float MaxValue = 0.5f;
	const float MinValue = -MaxValue;
	Vector3 Pos;
	Vector3 center;

	bool xMax, xMin, yMax, yMin, zMax, zMin;

	void CalculateCubesVerticies(PointData edge[8]);
	Vector3 CalculateMid(const PointData& p1, const PointData& p2);
	Vector3 Cross(const Vector3& v1, const Vector3& v2);
	Vector3 Normalise(const Vector3& v);
	
	
};


