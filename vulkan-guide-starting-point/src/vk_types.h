// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

struct AllocBuffer
{
	VkBuffer buffer;
	VmaAllocation alloc;
};

//we will add our main reusable types here