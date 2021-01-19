#include "Camera.h"

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::Update()
{
}

void Camera::UpdateScreenSize(int NewWidth, int NewHeight)
{
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{

}

void Camera::SetPosition(const glm::vec2& position)
{
    Position = glm::vec3(position, Position.z);
}

void Camera::SetPosition(const glm::vec3& position)
{
    Position = position;
}

void Camera::SetPosition(float x, float y)
{
    Position = glm::vec3(x, y, Position.z);
}

void Camera::SetPosition(float x, float y, float z)
{
    Position = glm::vec3(x, y, z);
}

void Camera::SetZoom(float zoom)
{
    Zoom = zoom;
}

void Camera::UpdateScreenSize(glm::vec2& ScreenSize)
{
}
