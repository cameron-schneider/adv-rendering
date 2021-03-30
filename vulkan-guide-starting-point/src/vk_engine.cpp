
#include "vk_engine.h"

#include <SDL.h>
#include <SDL_vulkan.h>

#include <VkBootstrap.h>

#include <vk_types.h>
#include <vk_initializers.h>

#include <iostream>
#include <fstream>

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

	init_commands();

	init_default_renderpass();

	init_framebuffers();

	init_sync_structures();

	init_pipelines();

	//everything went fine
	isInitialized = true;
}
void VulkanEngine::cleanup()
{	
	if (isInitialized) 
	{
		vkDestroyCommandPool(device, commandPool, nullptr);

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
	//Waits for render fence, times out after 1 second
	VK_CHECK(vkWaitForFences(device, 1, &renderFence, true, timer));
	VK_CHECK(vkResetFences(device, 1, &renderFence));

	uint32_t swapchainImgIndex;
	VK_CHECK(vkAcquireNextImageKHR(device, swapchain, timer, currentSem, nullptr, &swapchainImgIndex));

	VK_CHECK(vkResetCommandBuffer(mainCommandBuffer, 0));

	VkCommandBuffer cmd = mainCommandBuffer;

	VkCommandBufferBeginInfo cmdBegin = {};
	cmdBegin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBegin.pNext = nullptr;
	cmdBegin.pInheritanceInfo = nullptr;
	//Only used once
	cmdBegin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBegin));

	//Time for some fun
	VkClearValue clearVal;
	float flash = abs(sin(frameNumber / 120.0f));
	clearVal.color = { { 0.0f, flash, flash, 1.0f } };

	//Starting render pass
	VkRenderPassBeginInfo rpInfo = {};
	rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	rpInfo.pNext = nullptr;
	rpInfo.renderPass = renderPass;

	rpInfo.renderArea.offset.x = 0;
	rpInfo.renderArea.offset.y = 0;
	rpInfo.renderArea.extent = windowExtent;
	rpInfo.framebuffer = frameBuffers[swapchainImgIndex];

	rpInfo.clearValueCount = 1;
	rpInfo.pClearValues = &clearVal;

	vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdEndRenderPass(cmd);
	VK_CHECK(vkEndCommandBuffer(cmd));

	VkSubmitInfo sub = {};
	sub.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	sub.pNext = nullptr;

	VkPipelineStageFlags wait = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	sub.pWaitDstStageMask = &wait;

	sub.waitSemaphoreCount = 1;
	sub.pWaitSemaphores = &currentSem;

	sub.signalSemaphoreCount = 1;
	sub.pSignalSemaphores = &renderSem;

	sub.commandBufferCount = 1;
	sub.pCommandBuffers = &cmd;

	VK_CHECK(vkQueueSubmit(graphicsQueue, 1, &sub, renderFence));

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;

	presentInfo.pSwapchains = &swapchain;
	presentInfo.swapchainCount = 1;

	presentInfo.pImageIndices = &swapchainImgIndex;

	VK_CHECK(vkQueuePresentKHR(graphicsQueue, &presentInfo));

	++frameNumber;
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

bool VulkanEngine::load_shader_mod(const char* filePath, VkShaderModule* outShaderMod)
{
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);
	if (!file.is_open())
	{
		return false;
	}
	size_t fSize = (size_t)file.tellg();

	std::vector<uint32_t> buffer(fSize / sizeof(uint32_t));

	file.seekg(0);
	file.read((char*)buffer.data(), fSize);
	file.close();

	VkShaderModuleCreateInfo vkCreateInfo = {};
	vkCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vkCreateInfo.pNext = nullptr;
	vkCreateInfo.codeSize = buffer.size() * sizeof(uint32_t);
	vkCreateInfo.pCode = buffer.data();

	VkShaderModule shaderMod;
	if (vkCreateShaderModule(device, &vkCreateInfo, nullptr, &shaderMod)!= VK_SUCCESS)
	{
		return false;
	}
	*outShaderMod = shaderMod;
	return true;
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

	graphicsQueue = dev.get_queue(vkb::QueueType::graphics).value();
	graphicsQueueFam = dev.get_queue_index(vkb::QueueType::graphics).value();

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

void VulkanEngine::init_commands()
{
	//Creating command pool for submitted graphics commands
	//VkCommandPoolCreateInfo commandPoolInfo = {};
	VkCommandPoolCreateInfo commandPoolInfo = vkinit::command_pool_create_info(graphicsQueueFam, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	//commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	//commandPoolInfo.pNext = nullptr;

	//Command pool can submit graphics commands
	//commandPoolInfo.queueFamilyIndex = graphicsQueueFam;
	//Need to be able to reset command pool as well
	//commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	VK_CHECK(vkCreateCommandPool(device, &commandPoolInfo, nullptr, &commandPool));

	//VkCommandBufferAllocateInfo cmdAlloInfo = {};
	VkCommandBufferAllocateInfo cmdAlloInfo = vkinit::allocate_command_buffer_info(commandPool, 1);
	//cmdAlloInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	//cmdAlloInfo.pNext = nullptr;

	//cmdAlloInfo.commandPool = commandPool;
	//cmdAlloInfo.commandBufferCount = 1;
	//cmdAlloInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	VK_CHECK(vkAllocateCommandBuffers(device, &cmdAlloInfo, &mainCommandBuffer));
}

void VulkanEngine::CleanupSwapchain()
{
	vkDestroySwapchainKHR(device, swapchain, nullptr);

	vkDestroyRenderPass(device, renderPass, nullptr);
	/*
	for (int i = 0; i < frameBuffers.size(); i++)
	{
		vkDestroyFramebuffer(device, frameBuffers[i], nullptr);
	}
	*/
	for (int i = 0; i < swapchainImageViews.size(); i++)
	{
		vkDestroyFramebuffer(device, frameBuffers[i], nullptr);
		vkDestroyImageView(device, swapchainImageViews[i], nullptr);
	}

}

void VulkanEngine::init_default_renderpass()
{
	VkAttachmentDescription color_att = {};
	color_att.format = swapchainFormat;
	color_att.samples = VK_SAMPLE_COUNT_1_BIT;
	color_att.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_att.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_att.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_att.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	//Can be any layout
	color_att.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	//Allows image to be displayed on screen
	color_att.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference color_att_ref = {};
	color_att_ref.attachment = 0;
	color_att_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_att_ref;

	VkRenderPassCreateInfo pass_info = {};
	pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

	pass_info.attachmentCount = 1;
	pass_info.pAttachments = &color_att;

	pass_info.subpassCount = 1;
	pass_info.pSubpasses = &subpass;

	VK_CHECK(vkCreateRenderPass(device, &pass_info, nullptr, &renderPass));

}

void VulkanEngine::init_framebuffers()
{
	VkFramebufferCreateInfo fbInfo = {};
	fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fbInfo.pNext = nullptr;

	fbInfo.renderPass = renderPass;
	fbInfo.attachmentCount = 1;

	fbInfo.width = windowExtent.width;
	fbInfo.height = windowExtent.height;
	fbInfo.layers = 1;

	const uint32_t swapchainImgCount = swapchainImages.size();
	frameBuffers = std::vector<VkFramebuffer>(swapchainImgCount);

	for (int i = 0; i < swapchainImgCount; i++)
	{
		fbInfo.pAttachments = &swapchainImageViews[i];
		VK_CHECK(vkCreateFramebuffer(device, &fbInfo, nullptr, &frameBuffers[i]));
	}
}

void VulkanEngine::init_sync_structures()
{
	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.pNext = nullptr;

	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VK_CHECK(vkCreateFence(device, &fenceInfo, nullptr, &renderFence));

	//Creating semaphores
	VkSemaphoreCreateInfo semInfo = {};
	semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semInfo.pNext = nullptr;
	semInfo.flags = 0;

	VK_CHECK(vkCreateSemaphore(device, &semInfo, nullptr, &currentSem));
	VK_CHECK(vkCreateSemaphore(device, &semInfo, nullptr, &renderSem));

}

void VulkanEngine::init_pipelines()
{
	VkShaderModule triangleFragShader;
	if (!load_shader_mod("../../shaders/triangle.frag.spv", &triangleFragShader))
	{
		std::cout << "Error when building the triangle fragment shader module" << std::endl;
	}
	else std::cout << "Triangle fragment shader succesfully loaded" << std::endl;

	VkShaderModule triangleVertShader;
	if (!load_shader_mod("../../shaders/triangle.vert.spv", &triangleVertShader))
	{
		std::cout << "Error when building the triangle vertex shader module" << std::endl;
	}
	else std::cout << "Triangle vertex shader succesfully loaded" << std::endl;
}


