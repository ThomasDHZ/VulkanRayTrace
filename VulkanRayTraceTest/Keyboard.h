#pragma once
#include <GLFW\glfw3.h>
#include "Camera.h"
#include "PerspectiveCamera.h"
#include <memory>

class Keyboard
{
private:
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

public:

	void Update(GLFWwindow* window, std::shared_ptr<Camera> camera);
};