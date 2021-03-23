// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>
#include <vector>

class VulkanEngine {
public:

	//initializes everything in the engine
	void init();

	//shuts down the engine
	void cleanup();

	//draw loop
	void draw();

	//run main loop
	void run();

public:
	bool isInitialized{ false };
	int frameNumber{ 0 };

	VkExtent2D windowExtent{ 1700 , 900 };

	struct SDL_Window* window{ nullptr };

	/// <summary>
	/// Core Vulkan structure variables
	/// </summary>
	VkInstance instance;						// Vulkan handle, aka render context
	VkDebugUtilsMessengerEXT debugMessenger;	// debug handle
	VkPhysicalDevice renderGPU;					// GPU chosen as rendering GPU (leaves room for other GPUs)
	VkDevice device;							// Vulkan device for command handling
	VkSurfaceKHR surface;						// Window surface


	/// <summary>
	/// Swapchain variables
	/// </summary>
	VkSwapchainKHR swapchain;
	VkFormat swapchainFormat;
	std::vector<VkImage> swapchainImages;
	std::vector<VkImageView> swapchainImageViews;

	VkQueue graphicsQueue;
	uint32_t graphicsQueueFam;

	VkCommandPool commandPool;
	VkCommandBuffer mainCommandBuffer;

private:

	/// <summary>
	/// Initialize VK instance, SDL surface, and device/physical device
	/// </summary>
	/// <returns> returns 1 if success, 0 if error. Will also report error. </returns>
	uint32_t init_vk_context();

	/// <summary>
	/// Initialize the swapchain
	/// </summary>
	/// <returns> returns 1 if success, 0 if error. Will also report error. </returns>
	uint32_t init_swapchain();

	void init_commands();

	void CleanupSwapchain();
};
