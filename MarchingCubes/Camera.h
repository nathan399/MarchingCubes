#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <Math.h>
#include <SimpleMath.h>

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;


class Camera
{
public:
	Camera() {}
	Camera(Vector3 position, float width, float height);
	~Camera();

	void Move(Vector3 movement, float pitch, float yaw);
	
	void Render();
	Matrix getViewProj();
	Matrix getInverseView();
	Matrix getInverseProj();
	Vector3 getZAxis();
	Vector3 GetPos() { return Position; }


private:
	Matrix ViewMatrix;
	Matrix ProjMatrix;
	float Pitch = 0;
	float Yaw = 0;
	Vector3 Position;
};

