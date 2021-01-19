#pragma once
#include "Camera.h"
#include <GLFW\glfw3.h>
#include "PerspectiveCamera.h"
#include <memory>

class Mouse
{
private:
	float lastX;
	float lastY;
	bool firstMouse;
	double MouseXPos;
	double MouseYPos;

public:
	void Update(GLFWwindow* window, std::shared_ptr<Camera> camera);
};