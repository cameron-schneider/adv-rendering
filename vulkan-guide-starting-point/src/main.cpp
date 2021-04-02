#include <vk_engine.h>

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

int main(int argc, char* argv[])
{
	VulkanEngine engine;

	engine.init();	
	
	engine.run();	

	engine.cleanup();	

	return 0;
}
