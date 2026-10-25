#pragma once
#include "Renderer.h"
#include "VulkanDraw.h"

class Application
{
public :

	int width = 800;
	int height = 600;
	GLFWwindow* window = nullptr;

	void Init(const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices);
	void Run();
	void Destroy();

private:

	Renderer renderer;

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto ren = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));
		ren->GetVulkanDraw().framebufferResized = true;
	}
	void InitWindow();
	void InitVulkan(const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices);
	void DestroyWindow();
	void DestroyVulkan();

};
