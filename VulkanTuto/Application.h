#pragma once
#include "Dependencies.h"
#include "Renderer.h"
#include "VulkanDraw.h"
#include "VulkanDevice.h"

class Application
{
public :

	int width = 800;
	int height = 600;
	GLFWwindow* window = nullptr;

	void Init();
	void Run();
	void Destroy();
private:
	Renderer renderer;
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto app = reinterpret_cast<VulkanDraw*>(glfwGetWindowUserPointer(window));
		app->framebufferResized = true;
	}
	void InitWindow();
	void InitVulkan();
	void DestroyWindow();
	void DestroyVulkan();

};
