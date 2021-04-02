// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include "vk_types.h"
#include <vector>
#include <glm/vec3.hpp>

struct VertexInputDesc
{
	std::vector<VkVertexInputBindingDescription> bindings;
	std::vector<VkVertexInputAttributeDescription> attributes;

	VkPipelineVertexInputStateCreateFlags flags = 0;
};

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;

	static VertexInputDesc get_vertex_desc();
};

struct Mesh
{
	std::vector<Vertex> vertices;
	AllocBuffer vertBuffer;

	bool load_from_obj(const char* fileName);
};
