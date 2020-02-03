#include "Camera.h"
#include <algorithm>

using DirectX::SimpleMath::Quaternion;

Camera::Camera(Vector3 position, float width, float height) : Position(position)
{
	ProjMatrix = Matrix::CreatePerspectiveFieldOfView(DirectX::XMConvertToRadians(70.f), width / height, NearClip, FarClip);
}

Camera::~Camera()
{
}

void Camera::Move(Vector3 movement, float pitch, float yaw)
{
	Pitch += pitch * RotSpeed;
	Yaw += yaw * RotSpeed;
	movement *= MoveSpeed;

	Quaternion q = Quaternion::CreateFromYawPitchRoll(Yaw, -Pitch, 0.f);
	movement = Vector3::Transform(movement, q);

	Position += movement;


	// limit pitch to straight up or straight down
	// with a little fudge-factor to avoid gimbal lock
	float limit = DirectX::XM_PI / 2.0f - 0.04f;
	pitch = (std::max)(-limit, pitch);
	pitch = (std::min)(+limit, pitch);

	// keep longitude in sane range by wrapping
	if (yaw > DirectX::XM_PI)
	{
		yaw -= DirectX::XM_PI * 2.0f;
	}
	else if (yaw < -DirectX::XM_PI)
	{
		yaw += DirectX::XM_PI * 2.0f;
	}
}

void Camera::Render()
{
	float y = sinf(Pitch);
	float r = cosf(Pitch);
	float z = r * cosf(Yaw);
	float x = r * sinf(Yaw);

	Vector3 lookAt = Position + Vector3(x, y, z);

	ViewMatrix = XMMatrixLookAtRH(Position, lookAt, Vector3::Up);
}

Matrix Camera::getViewProj()
{
	return ViewMatrix * ProjMatrix;
}

Matrix Camera::getInverseView()
{
	return ViewMatrix.Invert();
}

Matrix Camera::getInverseProj()
{
	return ProjMatrix.Invert();
}

Vector3 Camera::getZAxis()
{
	return Vector3(ViewMatrix._13, ViewMatrix._23, ViewMatrix._33);
}

Vector3 Camera::WorldPointFromPixel(float x, float y, int width, int height)
{
	Vector3 screen = {x,y,0};
	return DirectX::XMVector3Unproject(screen, 0, 0, width, height, NearClip, FarClip, ProjMatrix, ViewMatrix, Matrix::Identity);

}