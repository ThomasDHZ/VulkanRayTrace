#include "Mouse.h"

void Mouse::Update(GLFWwindow* window, std::shared_ptr<Camera> camera)
{

	if (auto PCamera = dynamic_cast<PerspectiveCamera*>(camera.get()))
	{
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &MouseXPos, &MouseYPos);
			if (firstMouse)
			{
				lastX = MouseXPos;
				lastY = MouseYPos;
				firstMouse = false;
			}

			float xoffset = MouseXPos - lastX;
			float yoffset = lastY - MouseYPos;

			lastX = MouseXPos;
			lastY = MouseYPos;

			PCamera->ProcessMouseMovement(xoffset, yoffset);
		}
		else
		{
			firstMouse = true;
		}
	}
}