#include "Camera.h"
#include "Window.h"

void Camera::Update(float deltaTime)
{

    DirectX::XMFLOAT3 rotation = { 0.f, 0.f, 0.f };
    float sens = 0.8f;
    float speed = 1.f;

    if (Input::KeyPressed(Input::EGO_LEFT)) rotation.y -= 1.f;
    if (Input::KeyPressed(Input::EGO_RIGHT)) rotation.y += 1.f;
    if (Input::KeyPressed(Input::EGO_UP)) rotation.x -= 1.f;
    if (Input::KeyPressed(Input::EGO_DOWN)) rotation.x += 1.f;
    if (Input::KeyPressed(Input::EGO_Q)) rotation.z += 1.f;
    if (Input::KeyPressed(Input::EGO_E)) rotation.z -= 1.f;

    rotation = DirectX::XMFLOAT3(
        rotation.x * deltaTime * sens,
        rotation.y * deltaTime * sens,
        rotation.z * deltaTime * sens
    );

    m_Rotation.x += rotation.x;
    m_Rotation.y += rotation.y;
    m_Rotation.z += rotation.z;

    DirectX::XMFLOAT3 movement = { 0.f, 0.f, 0.f };

    if (Input::KeyPressed(Input::EGO_W)) movement.z -= 1.f * speed;
    if (Input::KeyPressed(Input::EGO_D)) movement.x += 1.f * speed;
    if (Input::KeyPressed(Input::EGO_A)) movement.x -= 1.f * speed;
    if (Input::KeyPressed(Input::EGO_S)) movement.z += 1.f * speed;
    if (Input::KeyPressed(Input::EGO_SPACE)) movement.y += 1.f * speed;
    if (Input::KeyPressed(Input::EGO_RSHIFT)) movement.y -= 1.f * speed;

    movement = DirectX::XMFLOAT3(
        movement.x * deltaTime,
        movement.y * deltaTime,
        movement.z * deltaTime
    );

    m_Position.x += movement.x;
    m_Position.y += movement.y;
    m_Position.z += movement.z;
	
}
