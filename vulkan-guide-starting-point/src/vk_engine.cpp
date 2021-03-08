
#include "vk_engine.h"

#include <SDL.h>
#include <SDL_vulkan.h>

#include <VkBootstrap.h>

#include <vk_types.h>
#include <vk_initializers.h>

#include <iostream>

using namespace std;
using namespace vkb;


#define VK_CHECK(x)                                             \
	do                                                              \
	{                                                               \
		VkResult err = x;												\
		if (err)														\
		{																\
			cout <<"Detected Vulkan error: " << err << endl;				\
			abort();														\
		}																\
	} while (0)


void VulkanEngine::init()
{
	// We initialize SDL and create a window with it. 
	SDL_Init(SDL_INIT_VIDEO);

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);
	
	window = SDL_CreateWindow(
		"Vulkan Engine",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowExtent.width,
		windowExtent.height,
		window_flags
	);
	

	// load vk core
	init_vk_context();

	//create swapchain
	init_swapchain();


	//everything went fine
	isInitialized = true;
}
void VulkanEngine::cleanup()
{	
	if (isInitialized) 
	{
		CleanupSwapchain();

		vkDestroyDevice(device, nullptr);
		vkDestroySurfaceKHR(instance, surface, nullptr);
		destroy_debug_utils_messenger(instance, debugMessenger);
		vkDestroyInstance(instance, nullptr);

		SDL_DestroyWindow(window);
	}
}

void VulkanEngine::draw()
{
	//nothing yet
}

void VulkanEngine::run()
{
	SDL_Event e;
	bool bQuit = false;

	//main loop
	while (!bQuit)
	{
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			//close the window when user alt-f4s or clicks the X button			
			if (e.type == SDL_QUIT)
				bQuit = true;
		}

		draw();
	}
}

uint32_t VulkanEngine::init_vk_context()
{
	InstanceBuilder contextBuilder;

	detail::Result<Instance> inst = contextBuilder.set_app_name("Test App")
		.request_validation_layers(true)
		.require_api_version(1, 1, 0)
		.use_default_debug_messenger()
		.build();

	instance = inst.value().instance;

	debugMessenger = inst.value().debug_messenger;

	SDL_Vulkan_CreateSurface(window, instance, &surface);

	PhysicalDeviceSelector selector{ inst.value() };
	PhysicalDevice physDev = selector
		.set_minimum_version(1, 1)
		.set_surface(surface)
		.select()
		.value();

	DeviceBuilder deviceBuilder{ physDev };

	Device dev = deviceBuilder.build().value();

	device = dev.device;
	renderGPU = physDev.physical_device;

	return 1;
}

uint32_t VulkanEngine::init_swapchain()
{
	SwapchainBuilder chainBuilder{ renderGPU, device, surface };

	Swapchain vkbChain = chainBuilder
		.use_default_format_selection()
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.set_desired_extent(windowExtent.width, windowExtent.height)
		.build()
		.value();

	swapchain = vkbChain.swapchain;
	swapchainImages = vkbChain.get_images().value();
	swapchainImageViews = vkbChain.get_image_views().value();

	swapchainFormat = vkbChain.image_format;

	return 1;
}

void VulkanEngine::CleanupSwapchain()
{
	vkDestroySwapchainKHR(device, swapchain, nullptr);

	for (int i = 0; i < swapchainImageViews.size(); i++)
	{
		vkDestroyImageView(device, swapchainImageViews[i], nullptr);
	}
}


