﻿// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include "vk_types.h"
#include <vector>
#include <functional>
#include <deque>
#include "vk_mesh.h"
#include <glm/glm.hpp>

struct MeshPushConsts
{
	glm::vec3 data;
	glm::mat4 renderMatrix;
};

struct DelQueue
{
	std::deque<std::function<void()>> deletors;

	void push_funct(std::function<void()>&& function) { deletors.push_back(function); }

	void flush()
	{
		for (auto iter = deletors.rbegin(); iter != deletors.rend(); iter++)
		{
			(*iter)();
		}
		deletors.clear();
	}
};


/*
* TODO:
*	-Translate all of our structs into Animal structs, implementing the a3graphics interface.
*		a3_BufferObject
*		a3_VertexBuffer
*		a3_VertexDescriptors
*		a3_VertexDrawable		--Meshes I think
*		a3_Framebuffer
*		a3_ShaderProgram		--Idk about this one, since Vulkan handles things a bit differently
*		a3_UniformBuffer		--Basically push constants
*		a3_Texture
*		a3_GraphicsObjectHandle	--I think it's just a generic object reference tracker, plus a release function for deletion.
* 
*	Might have to create some custom structs to manage other things, not too sure yet
* 
*	Still need to set up context & window creation in DemoState_load as the very first thing.
*/


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

	bool load_shader_mod(const char* filePath, VkShaderModule* outShaderMod);

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

	VkPipelineLayout trianglePipelineLayout;

	VkPipeline trianglePipeline;
	VkPipeline redTriPipeline;


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

	VkRenderPass renderPass;
	std::vector<VkFramebuffer> frameBuffers;

	VkSemaphore currentSem, renderSem;
	VkFence renderFence;

	DelQueue mainDeletionQueue;

	VmaAllocator allocator;

	VkPipeline meshPipeline;
	Mesh triangleMesh;

	VkPipelineLayout meshPipelineLayout;

	Mesh monkeyMesh; //:)

private:

	static const uint64_t timer = 1000000000;

	int selectedShader{ 0 };

	/// <summary>
	/// Initialize VK instance, SDL surface, and device/physical device
	/// </summary>
	/// <returns> returns 1 if success, 0 if error. Will also report error. </returns>
	uint32_t a3createDefaultVKContext();

	/// <summary>
	/// Initialize the swapchain
	/// </summary>
	/// <returns> returns 1 if success, 0 if error. Will also report error. </returns>
	
	//All of these need to get moved elsewhere, they'll be split up into their respective files.
	uint32_t init_swapchain();

	void init_commands();

	void CleanupSwapchain();

	void init_default_renderpass();

	void init_framebuffers();

	void init_sync_structures();

	void init_pipelines();

	void load_meshes();

	void upload_mesh(Mesh& mesh);
};

class PipelineBuilder
{
public:
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	VkPipelineVertexInputStateCreateInfo vertexInputInfo;
	VkPipelineInputAssemblyStateCreateInfo inputAssembly;
	VkViewport viewport;
	VkRect2D scissor;
	VkPipelineRasterizationStateCreateInfo rasterizer;
	VkPipelineColorBlendAttachmentState colorBlendAttachment;
	VkPipelineMultisampleStateCreateInfo multisampling;
	VkPipelineLayout pipelineLayout;

	VkPipeline build_pipeline(VkDevice device, VkRenderPass pass);
};