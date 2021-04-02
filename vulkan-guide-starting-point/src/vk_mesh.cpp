#include <vk_mesh.h>

VertexInputDesc Vertex::get_vertex_desc()
{
	VertexInputDesc desc;

	VkVertexInputBindingDescription main = {};
	main.binding = 0;
	main.stride = sizeof(Vertex);
	main.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	desc.bindings.push_back(main);

	VkVertexInputAttributeDescription positionAttr = {};
	positionAttr.binding = 0;
	positionAttr.location = 0;
	positionAttr.format = VK_FORMAT_R32G32B32_SFLOAT;
	positionAttr.offset = offsetof(Vertex, position);

	//Normal will be stored at Location 1
	VkVertexInputAttributeDescription normalAttr = {};
	normalAttr.binding = 0;
	normalAttr.location = 1;
	normalAttr.format = VK_FORMAT_R32G32B32_SFLOAT;
	normalAttr.offset = offsetof(Vertex, normal);

	//Color will be stored at Location 2
	VkVertexInputAttributeDescription colorAttr = {};
	colorAttr.binding = 0;
	colorAttr.location = 2;
	colorAttr.format = VK_FORMAT_R32G32B32_SFLOAT;
	colorAttr.offset = offsetof(Vertex, color);

	desc.attributes.push_back(positionAttr);
	desc.attributes.push_back(normalAttr);
	desc.attributes.push_back(colorAttr);

	return desc;
}