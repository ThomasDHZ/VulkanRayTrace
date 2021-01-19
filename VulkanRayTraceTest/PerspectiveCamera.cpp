#include "PerspectiveCamera.h"

PerspectiveCamera::PerspectiveCamera(glm::vec2 ScreenSize, glm::vec3 position) : Camera()
{
    Position = position;
    Up = glm::vec3(0.0f, 1.0f, 0.0f);
    Front = glm::vec3(0.0f, 0.0f, -1.0f);

    MovementSpeed = 2.5f;
    MouseSensitivity = 0.1f;

    Position = position;
    WorldUp = Up;

    Zoom = 45.0f;
    Yaw = -90.0f;
    Pitch = 0.0f;
}

PerspectiveCamera::PerspectiveCamera(glm::vec2 ScreenSize, glm::vec3 position, float pitch, float yaw)
{
    Position = position;
    Up = glm::vec3(0.0f, 1.0f, 0.0f);
    Front = glm::vec3(0.0f, 0.0f, -1.0f);

    MovementSpeed = 2.5f;
    MouseSensitivity = 0.1f;

    Position = position;
    WorldUp = Up;

    Zoom = 45.0f;
    Yaw = pitch;
    Pitch = yaw;
}

PerspectiveCamera::~PerspectiveCamera()
{
}

void PerspectiveCamera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == UP)
        Position += Up * velocity;
    if (direction == DOWN)
        Position -= Up * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
}

void PerspectiveCamera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }
}

void PerspectiveCamera::MouseScroll(float yoffset)
{
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

void PerspectiveCamera::Update(float Width, float Height)
{
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);

    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));

    ViewMatrix = glm::lookAt(Position, Position + Front, Up);
    ProjectionMatrix = glm::perspective(glm::radians(Zoom), Width / Height, 0.1f, 10000.0f);
}
