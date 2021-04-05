#include "vk_mesh.h"
#include <VK/tinyobjloader/tiny_obj_loader.h>
#include <iostream>

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

bool Mesh::load_from_obj(const char* fileName)
{
	tinyobj::attrib_t attribute;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warning;
	std::string error;

	tinyobj::LoadObj(&attribute, &shapes, &materials, &warning, &error, fileName, nullptr);

	if (!warning.empty())
	{
		std::cout << "WARNING: " << warning << std::endl;
	}
	if (!error.empty())
	{
		std::cerr << "ERROR: " << error << std::endl;
		return false;
	}

	for (size_t shapeIndex = 0; shapeIndex < shapes.size(); shapeIndex++)
	{
		size_t index_offset = 0;
		for (size_t faces = 0; faces < shapes[shapeIndex].mesh.num_face_vertices.size(); faces++)
		{
			int fv = 3;
			for (size_t vertIndex = 0; vertIndex < fv; vertIndex++)
			{
				tinyobj::index_t idx = shapes[shapeIndex].mesh.indices[index_offset + vertIndex];

				//Vertex Pos
				tinyobj::real_t vertX = attribute.vertices[3 * idx.vertex_index + 0];
				tinyobj::real_t vertY = attribute.vertices[3 * idx.vertex_index + 1];
				tinyobj::real_t vertZ = attribute.vertices[3 * idx.vertex_index + 2];
				//Vertex Norm
				tinyobj::real_t normX = attribute.normals[3 * idx.normal_index + 0];
				tinyobj::real_t normY = attribute.normals[3 * idx.normal_index + 1];
				tinyobj::real_t normZ = attribute.normals[3 * idx.normal_index + 2];

				Vertex newVertex;
				newVertex.position.x = vertX;
				newVertex.position.y = vertY;
				newVertex.position.z = vertZ;
				newVertex.normal.x = normX;
				newVertex.normal.y = normY;
				newVertex.normal.z = normZ;

				newVertex.color = newVertex.normal;
				vertices.push_back(newVertex);
			}
			index_offset += fv;
		}
	}
	return true;
}
