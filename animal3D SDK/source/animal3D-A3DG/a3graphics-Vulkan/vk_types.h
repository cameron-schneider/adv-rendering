// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <VK/vulkan.h>
#include <VK/vma/vk_mem_alloc.h>
#include <glm/common.hpp>
#include <glm/gtx/transform.hpp>
#include <animal3D-A3DM/animal3D-A3DM.h>

struct AllocBuffer
{
	VkBuffer buffer;
	VmaAllocation alloc;
};

//we will add our main reusable types here