#pragma once
#include <DirectXMath.h>
class Camera
{
public:
	Camera() = default;
	Camera(float ratio, float fov, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation):
		m_AspectRatio(ratio), m_Fov(fov), m_Position(position), m_Rotation(rotation)
	{
	
		m_AspectRatio = ratio;
		m_Fov = fov;						

	}


	void Update(float deltaTime);
	const DirectX::XMMATRIX GetView()
	{
		
		// Create rotation matrix based on m_Rotation
		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
		// Transform the camera's position from world space to local space
		DirectX::XMMATRIX invRotationMatrix = DirectX::XMMatrixInverse(nullptr, rotationMatrix);
		DirectX::XMVECTOR localPosition = DirectX::XMVector3TransformCoord(XMLoadFloat3(&m_Position), invRotationMatrix);

		// Calculate the view matrix without rotation
		DirectX::XMMATRIX translation = DirectX::XMMatrixTranslationFromVector(localPosition);
		
		DirectX::XMMATRIX view;// = DirectX::XMMatrixLookAtRH(XMLoadFloat3(&m_Position), XMLoadFloat3(&m_Focus), XMLoadFloat3(&m_Up));

		// Apply the rotation to the view matrix
		view =  rotationMatrix * translation;


		
		
		return view;
	}
	const DirectX::XMFLOAT4 GetProjectionVector()
	{
		DirectX::XMFLOAT4 projectionVector;

		// convert to radians
		float Rfov = m_Fov * (DirectX::XM_PI / 180.f);
		projectionVector = { m_Position.x, m_Position.y, -1.f * m_Position.z, tanf(Rfov * 0.5f) };

		return projectionVector;
	}
	const DirectX::XMFLOAT3 GetPosition() { return m_Position; }
private:
	float m_AspectRatio;
	float m_Fov;

	DirectX::XMFLOAT3 m_Position;
	DirectX::XMFLOAT3 m_Rotation;

	DirectX::XMFLOAT3 m_Up = { 0.f,1.f,0.f };
	DirectX::XMFLOAT3 m_Focus = { 0.f,0.f,0.f };
};

