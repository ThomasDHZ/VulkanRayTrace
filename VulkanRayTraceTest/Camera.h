#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>


enum Camera_Movement {
    FORWARD,
    BACKWARD,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Camera
{
protected:
    float Width;
    float Height;
    float Aspect;

    glm::mat4 ProjectionMatrix;
    glm::mat4 ViewMatrix;
    glm::vec2 ViewScreenSize;

    glm::vec3 Position;

    float Zoom;
    float MovementSpeed = 5.0f;

public:
    Camera();
    ~Camera();

    virtual void Update();
    virtual void UpdateScreenSize(int NewWidth, int NewHeight);
    virtual void UpdateScreenSize(glm::vec2& ScreenSize);
    virtual void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    void SetPosition(const glm::vec2& position);
    void SetPosition(const glm::vec3& position);
    void SetPosition(float x, float y);
    void SetPosition(float x, float y, float z);
    void SetZoom(float zoom);

    const glm::vec2& GetViewScreenSize() { return ViewScreenSize; }
    const glm::mat4& GetProjectionMatrix() { return ProjectionMatrix; }
    const glm::mat4& GetViewMatrix() { return ViewMatrix; }
    const glm::vec3& GetPosition() { return Position; }
    const float GetZoom() { return Zoom; }
};