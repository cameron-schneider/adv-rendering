﻿
#include "vk_engine.h"

#include <SDL.h>
#include <SDL_vulkan.h>

#include <VkBootstrap.h>

#include <vk_types.h>
#include <vk_initializers.h>

#include <iostream>
#include <fstream>

#include <glm/gtx/transform.hpp>

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

	load_meshes();

	//everything went fine
	isInitialized = true;
}
void VulkanEngine::cleanup()
{	
	if (isInitialized) 
	{
		vkWaitForFences(device, 1, &renderFence, true, timer);

		mainDeletionQueue.flush();
		vmaDestroyAllocator(allocator);
		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyDevice(device, nullptr);
		vkDestroyInstance(instance, nullptr);	//Some child objects of instance not destroyed before this is called
		SDL_DestroyWindow(window);
		/*
		vkDestroyCommandPool(device, commandPool, nullptr);

		CleanupSwapchain();


		vkDestroyDevice(device, nullptr);
		vkDestroySurfaceKHR(instance, surface, nullptr);
		destroy_debug_utils_messenger(instance, debugMessenger);
		vkDestroyInstance(instance, nullptr);

		SDL_DestroyWindow(window);*/
	}
}

void VulkanEngine::draw()
{
	//Waits for render fence, times out after 1 second
	VK_CHECK(vkWaitForFences(device, 1, &renderFence, true, 1000000000));
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

	VkClearValue depthClear;
	depthClear.depthStencil.depth = 1.0f;

	//Starting render pass
	VkRenderPassBeginInfo rpInfo = vkinit::renderpass_begin_info(renderPass, windowExtent, frameBuffers[swapchainImgIndex]);

	/*
	VkRenderPassBeginInfo rpInfo = {};
	rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	rpInfo.pNext = nullptr;
	rpInfo.renderPass = renderPass;

	rpInfo.renderArea.offset.x = 0;
	rpInfo.renderArea.offset.y = 0;
	rpInfo.renderArea.extent = windowExtent;
	rpInfo.framebuffer = frameBuffers[swapchainImgIndex];
	*/
	rpInfo.clearValueCount = 2;

	VkClearValue clearValues[] = { clearVal, depthClear };

	rpInfo.pClearValues = &clearValues[0];


	vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

	/*
	if (selectedShader == 0)
	{
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, trianglePipeline);
	}
	else
	{
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, redTriPipeline);
	}
	vkCmdDraw(cmd, 3, 1, 0, 0);
	*/

	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, meshPipeline);

	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(cmd, 0, 1, &monkeyMesh.vertBuffer.buffer, &offset);

	//Pushin Constants
	glm::vec3 camPos = { 0.f,0.f,-2.f };
	glm::mat4 view = glm::translate(glm::mat4(1.f), camPos);
	//Cam projection
	glm::mat4 projection = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.0f);
	projection[1][1] *= -1;
	//model rotation
	glm::mat4 model = glm::rotate(glm::mat4{ 1.0f }, glm::radians(frameNumber * 0.4f), glm::vec3(0, 1, 0));

	glm::mat4 meshMatrix = projection * view * model;

	MeshPushConsts constants;
	constants.renderMatrix = meshMatrix;

	vkCmdPushConstants(cmd, meshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConsts), &constants);

	vkCmdDraw(cmd, monkeyMesh.vertices.size(), 1, 0, 0);

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

	VK_CHECK(vkQueueSubmit(graphicsQueue, 1, &sub, renderFence));	//Semaphore issue prints here (not on first runthrough though)

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;

	presentInfo.pSwapchains = &swapchain;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores = &renderSem;
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &swapchainImgIndex;

	VK_CHECK(vkQueuePresentKHR(graphicsQueue, &presentInfo));

	frameNumber++;
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

			else if (e.type == SDL_KEYDOWN)
			{
				if (e.key.keysym.sym == SDLK_SPACE)
				{
					selectedShader++;
					if (selectedShader > 1)	//CHANGING SHADERS
					{
						selectedShader = 0;
					}
				}
			}
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

	VmaAllocatorCreateInfo allocInfo = {};
	allocInfo.physicalDevice = renderGPU;
	allocInfo.device = device;
	allocInfo.instance = instance;
	vmaCreateAllocator(&allocInfo, &allocator);

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

	mainDeletionQueue.push_funct([=]() {vkDestroySwapchainKHR(device, swapchain, nullptr); });


	VkExtent3D depthImgExtent = {
		windowExtent.width,
		windowExtent.height,
		1
	};

	depthFormat = VK_FORMAT_D32_SFLOAT;

	VkImageCreateInfo dimg_info = vkinit::image_create_info(depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depthImgExtent);

	VmaAllocationCreateInfo dimg_allocInfo = {};
	dimg_allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	dimg_allocInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	vmaCreateImage(allocator, &dimg_info, &dimg_allocInfo, &depthImg.image, &depthImg.alloc, nullptr);

	VkImageViewCreateInfo dViewInfo = vkinit::imageview_create_info(depthFormat, depthImg.image, VK_IMAGE_ASPECT_DEPTH_BIT);

	VK_CHECK(vkCreateImageView(device, &dViewInfo, nullptr, &depthImgView));

	mainDeletionQueue.push_funct([=]() {
		vkDestroyImageView(device, depthImgView, nullptr);
		vmaDestroyImage(allocator, depthImg.image, depthImg.alloc);
	});

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


	mainDeletionQueue.push_funct([=]() {vkDestroyCommandPool(device, commandPool, nullptr); });
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

	//Depth shit
	VkAttachmentDescription depth_att = {};
	depth_att.flags = 0;
	depth_att.format = depthFormat;
	depth_att.samples = VK_SAMPLE_COUNT_1_BIT;
	depth_att.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_att.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	depth_att.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_att.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_att.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_att.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depth_att_ref = {};
	depth_att_ref.attachment = 1;
	depth_att_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//Subby passy
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_att_ref;
	subpass.pDepthStencilAttachment = &depth_att_ref;


	VkAttachmentDescription attachments[2] = {color_att, depth_att};

	VkRenderPassCreateInfo pass_info = {};
	pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

	pass_info.attachmentCount = 2;
	pass_info.pAttachments = &attachments[0];

	pass_info.subpassCount = 1;
	pass_info.pSubpasses = &subpass;

	VK_CHECK(vkCreateRenderPass(device, &pass_info, nullptr, &renderPass));

	mainDeletionQueue.push_funct([=]() {vkDestroyRenderPass(device, renderPass, nullptr); });

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
		VkImageView attachments[2];
		attachments[0] = swapchainImageViews[i];
		attachments[1] = depthImgView;

		fbInfo.pAttachments = attachments;
		fbInfo.attachmentCount = 2;

		VK_CHECK(vkCreateFramebuffer(device, &fbInfo, nullptr, &frameBuffers[i]));
		mainDeletionQueue.push_funct([=]() {vkDestroyFramebuffer(device, frameBuffers[i], nullptr); });
		mainDeletionQueue.push_funct([=]() {vkDestroyImageView(device, swapchainImageViews[i], nullptr); });
	}
}

void VulkanEngine::init_sync_structures()
{
	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.pNext = nullptr;

	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VK_CHECK(vkCreateFence(device, &fenceInfo, nullptr, &renderFence));

	mainDeletionQueue.push_funct([=]() {vkDestroyFence(device, renderFence, nullptr); });

	//Creating semaphores
	VkSemaphoreCreateInfo semInfo = {};
	semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semInfo.pNext = nullptr;
	semInfo.flags = 0;

	VK_CHECK(vkCreateSemaphore(device, &semInfo, nullptr, &currentSem));
	VK_CHECK(vkCreateSemaphore(device, &semInfo, nullptr, &renderSem));

	mainDeletionQueue.push_funct([=]()
	{
		vkDestroySemaphore(device, currentSem, nullptr); 
		vkDestroySemaphore(device, renderSem, nullptr); 
	});
}

void VulkanEngine::init_pipelines()
{
	VkShaderModule triangleFragShader;
	if (!load_shader_mod("../../shaders/ColorTriangle.frag.spv", &triangleFragShader))
	{
		std::cout << "Error when building the triangle fragment shader module" << std::endl;
	}
	else std::cout << "Triangle fragment shader succesfully loaded" << std::endl;

	VkShaderModule triangleVertShader;
	if (!load_shader_mod("../../shaders/ColorTriangle.vert.spv", &triangleVertShader))
	{
		std::cout << "Error when building the triangle vertex shader module" << std::endl;
	}
	else std::cout << "Triangle vertex shader succesfully loaded" << std::endl;

	VkShaderModule redTriangleFragShader;
	if (!load_shader_mod("../../shaders/Triangle.frag.spv", &redTriangleFragShader))
	{
		std::cout << "Error when building the triangle fragment shader module" << std::endl;
	}
	else std::cout << "Red triangle fragment shader succesfully loaded" << std::endl;

	VkShaderModule redTriangleVertShader;
	if (!load_shader_mod("../../shaders/Triangle.vert.spv", &redTriangleVertShader))
	{
		std::cout << "Error when building the triangle vertex shader module" << std::endl;
	}
	else std::cout << "Red triangle vertex shader succesfully loaded" << std::endl;


	VkPipelineLayoutCreateInfo pipeline_layout_info = vkinit::pipeline_layout_create_info();

	VK_CHECK(vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr, &trianglePipelineLayout));

	PipelineBuilder pipeBuild;

	pipeBuild.shaderStages.push_back(
		vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, triangleVertShader));
	pipeBuild.shaderStages.push_back(
		vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, triangleFragShader));

	pipeBuild.vertexInputInfo = vkinit::vertex_input_state_create_info();

	pipeBuild.inputAssembly = vkinit::input_assembly_create_info(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

	pipeBuild.viewport.x = 0.0f;
	pipeBuild.viewport.y = 0.0f;
	pipeBuild.viewport.width = windowExtent.width;
	pipeBuild.viewport.height = windowExtent.height;
	pipeBuild.viewport.minDepth = 0.0f;
	pipeBuild.viewport.maxDepth = 1.0f;

	pipeBuild.scissor.offset = {0,0};
	pipeBuild.scissor.extent = windowExtent;

	pipeBuild.rasterizer = vkinit::rasterization_state_create_info(VK_POLYGON_MODE_FILL);
	pipeBuild.multisampling = vkinit::multisampling_state_create_info();
	pipeBuild.colorBlendAttachment = vkinit::color_blend_attachment_state();
	pipeBuild.pipelineLayout = trianglePipelineLayout;

	//Depth
	pipeBuild.depthStencil = vkinit::depth_stencil_create_info(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);

	trianglePipeline = pipeBuild.build_pipeline(device, renderPass);

	pipeBuild.shaderStages.clear();

	pipeBuild.shaderStages.push_back(
		vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, redTriangleVertShader));
	pipeBuild.shaderStages.push_back(
		vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, redTriangleFragShader));

	redTriPipeline = pipeBuild.build_pipeline(device, renderPass);


	VertexInputDesc vertDesc = Vertex::get_vertex_desc();
	pipeBuild.vertexInputInfo.pVertexAttributeDescriptions = vertDesc.attributes.data();
	pipeBuild.vertexInputInfo.vertexAttributeDescriptionCount = vertDesc.attributes.size();

	pipeBuild.vertexInputInfo.pVertexBindingDescriptions = vertDesc.bindings.data();
	pipeBuild.vertexInputInfo.vertexBindingDescriptionCount = vertDesc.bindings.size();

	pipeBuild.shaderStages.clear();

	VkPipelineLayoutCreateInfo mesh_pipeline_layout_info = vkinit::pipeline_layout_create_info();

	VkPushConstantRange pushConst;
	pushConst.offset = 0;
	pushConst.size = sizeof(MeshPushConsts);
	pushConst.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	mesh_pipeline_layout_info.pPushConstantRanges = &pushConst;
	mesh_pipeline_layout_info.pushConstantRangeCount = 1;

	VK_CHECK(vkCreatePipelineLayout(device, &mesh_pipeline_layout_info, nullptr, &meshPipelineLayout));


	VkShaderModule meshVertShader;
	if (!load_shader_mod("../../shaders/tri_mesh.vert.spv", &meshVertShader))
	{
		std::cout << "Error when building the mesh vertex shader module" << std::endl;
	}
	else std::cout << "Mesh vertex shader succesfully loaded" << std::endl;

	pipeBuild.shaderStages.push_back(
		vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, meshVertShader));
	pipeBuild.shaderStages.push_back(
		vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, triangleFragShader));

	pipeBuild.pipelineLayout = meshPipelineLayout;

	meshPipeline = pipeBuild.build_pipeline(device, renderPass);

	vkDestroyShaderModule(device, meshVertShader, nullptr);
	vkDestroyShaderModule(device, redTriangleVertShader, nullptr);
	vkDestroyShaderModule(device, redTriangleFragShader, nullptr);
	vkDestroyShaderModule(device, triangleFragShader, nullptr);
	vkDestroyShaderModule(device, triangleVertShader, nullptr);
	
	mainDeletionQueue.push_funct([=]()
	{
			vkDestroyPipeline(device, meshPipeline, nullptr);
			vkDestroyPipeline(device, trianglePipeline, nullptr);
			vkDestroyPipeline(device, redTriPipeline, nullptr);
			vkDestroyPipelineLayout(device, trianglePipelineLayout, nullptr);
			vkDestroyPipelineLayout(device, meshPipelineLayout, nullptr);
	});

	//mainDeletionQueue.push_funct([=]() {vkDestroyPipelineLayout(device, meshPipelineLayout, nullptr); });
}

void VulkanEngine::load_meshes()
{
	triangleMesh.vertices.resize(3);

	triangleMesh.vertices[0].position = { 1.f, 1.f, 0.5f };
	triangleMesh.vertices[1].position = { -1.f, 1.f, 0.5f };
	triangleMesh.vertices[2].position = { 0.f, -1.f, 0.5f };	//TRIANGLE WASN'T SHOWING UP BECAUSE IT WAS POINTED DOWN NOT UP, NICE ONE HUBNER

	triangleMesh.vertices[0].color = { 0.f, 1.f, 0.0f };
	triangleMesh.vertices[1].color = { 0.f, 1.f, 0.0f };
	triangleMesh.vertices[2].color = { 0.f, 1.f, 0.0f };

	monkeyMesh.load_from_obj("../../assets/suzanne.obj");

	upload_mesh(triangleMesh);
	upload_mesh(monkeyMesh);
}

void VulkanEngine::upload_mesh(Mesh& mesh)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	//bufferInfo.pNext = nullptr;
	bufferInfo.size = mesh.vertices.size() * sizeof(Vertex);
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

	VmaAllocationCreateInfo vmaAlloc = {};
	vmaAlloc.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

	VK_CHECK(vmaCreateBuffer(allocator, &bufferInfo, &vmaAlloc,
		&mesh.vertBuffer.buffer,
		&mesh.vertBuffer.alloc,
		nullptr));

	mainDeletionQueue.push_funct([=]() {vmaDestroyBuffer(allocator, mesh.vertBuffer.buffer, mesh.vertBuffer.alloc); });

	void* data;
	vmaMapMemory(allocator, mesh.vertBuffer.alloc, &data);
	memcpy(data, mesh.vertices.data(), mesh.vertices.size() * sizeof(Vertex));
	vmaUnmapMemory(allocator, mesh.vertBuffer.alloc);
}

VkPipeline PipelineBuilder::build_pipeline(VkDevice device, VkRenderPass pass)
{
	VkPipelineViewportStateCreateInfo vpState = {};
	vpState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	vpState.pNext = nullptr;

	vpState.viewportCount = 1;
	vpState.pViewports = &viewport;
	vpState.scissorCount = 1;
	vpState.pScissors = &scissor;

	VkPipelineColorBlendStateCreateInfo colorBlend = {};
	colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlend.pNext = nullptr;

	colorBlend.logicOpEnable = VK_FALSE;
	colorBlend.logicOp = VK_LOGIC_OP_COPY;
	colorBlend.attachmentCount = 1;
	colorBlend.pAttachments = &colorBlendAttachment;

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = nullptr;

	pipelineInfo.stageCount = shaderStages.size();
	pipelineInfo.pStages = shaderStages.data();

	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &vpState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlend;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = pass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	pipelineInfo.pDepthStencilState = &depthStencil;

	VkPipeline newPipe; // ;^)
	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &newPipe) != VK_SUCCESS)
	{
		std::cout << "Failed to create pipeline" << std::endl;
		return VK_NULL_HANDLE;
	}
	else return newPipe;

}
