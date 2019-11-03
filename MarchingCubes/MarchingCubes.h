#pragma once
#include "DeviceResources.h"
#include "FastNoise.h"
#include <SimpleMath.h>
#include <vector>

using DirectX::SimpleMath::Vector3;
using std::vector;

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

class MarchingCubes
{
public:
	MarchingCubes(Vector3 pos, bool xmax, bool xmin, bool ymax, bool ymin, bool zmax, bool zmin, float pointDistance = 1, float frequency = 1, int GridSize = 10, bool interpolate = true);
	~MarchingCubes() {};
	std::vector<SVertices> getVertices() { return vertices; }
	void generate(float pointDistance, float frequency, int GridSize, bool interpolate);
	void CreateMesh();
	void AffectPoints(Vector3 pos, int direction, float radius);
	bool CubeToSphere(Vector3 sPos, float radius);
private:
	
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


