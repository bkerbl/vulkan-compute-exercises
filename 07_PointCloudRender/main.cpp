#include <vulkan/vulkan.hpp>
#include <framework.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>

int main(int argc, char* argv[])
{
	try
	{
		if (argc < 2)
		{
			std::cout << "No image point cloud file name provided!" << std::endl;
			return -1;
		}

		vk::UniqueInstance instance;
		vk::PhysicalDevice physicalDevice;
		vk::UniqueDevice device;
		vk::UniqueCommandPool commandPool;
		vk::Queue queue;

		Framework::setupBasicCompute("Task 9", VK_API_VERSION_1_3, {}, {}, instance, physicalDevice, device, queue, commandPool);

		// TODO: This is the point cloud struct that you hold on the CPU. The method below
		// will read a file and store its contents into the variables named "position" and
		// "color". Afterwards, it is copied to the GPU and read in the shader pointcloud.comp. 
		// The shader defines its own version of this struct, also with two 3D vectors.
		// However, in its current form, this will most likely not work! Take a look at 
		// the shader. Where is the discrepancy? Find a solution so the GPU can easily read
		// the data you provide in your struct on the GPU. HINT: It will be helpful to consult
		// the GLSL specification of the std430 layout that the shader code expects. HINT:
		// To debug your program, just start a single block with a single thread and let it
		// fetch and print a few points. Compare with outputs on the CPU. This is an easy way
		// to make sure they match.
		struct Point
		{
			glm::vec3 position;
			glm::vec3 color;
		};

		// TODO: Same as above, the shader has an equivalent of the below struct. There is
		// a little twist: the .width and .height are packed into a vector resolution[2] in
		// the shader code. They will be stored in a uniform buffer, which does not have the
		// std430 layout, but instead uses std140. What do you need to change to make sure 
		// the CPU data can be accessed without problems on the GPU? HINT: for an "elegant"
		// solution, you can check out the "alignas" C++ keyword.
		struct Parameters
		{
			uint32_t width;
			uint32_t height;
			uint32_t numPoints;
			glm::mat4 mvp;
		};

		glm::vec3 minimum, maximum;
		std::vector<Point> pointcloud;
		Framework::readPointCloud(argv[1], pointcloud, minimum, maximum);

		constexpr uint32_t width = 800, height = 800;
		// TODO: Description for the resources that go into our pipeline (and descriptor set):
		// - One uniform buffer for the rendering parameters (size of Parameters struct)
		// - One storage buffer for the point cloud (pointcloud.size() * size of Point struct)
		// - One storage buffer for the output image (width * height * size of uint32).
		// - All of them should be host-visible, host-coherent and device-local.

		vk::UniqueDescriptorSetLayout descriptorSetLayout;
		vk::UniqueDescriptorPool descriptorPool;
		vk::UniqueDescriptorSet descriptorSet;
		std::vector<vk::UniqueBuffer> buffers;
		std::vector<vk::UniqueDeviceMemory> bufferMemories;

		// TODO: As before, take all the necessary steps to have a descriptor set that references
		// the above created buffers. Make sure the bindings match the shader "primes.comp".

		glm::vec3 center = (minimum + maximum) * 0.5f;
		float span = glm::length(maximum - minimum);
		auto view = glm::lookAt(center + glm::vec3(-0.9f * span, 0, 0.15f * span), center, glm::vec3(0, 0, -1));
		auto projection = glm::perspective(45.f, ((float)width) / height, 0.1f, 2 * span);

		// Set rendering params 
		Parameters* pointcloudParams_mapped = (Parameters*)device->mapMemory(*bufferMemories[0], 0, VK_WHOLE_SIZE);
		pointcloudParams_mapped->width = width;
		pointcloudParams_mapped->height = height;
		pointcloudParams_mapped->numPoints = (uint32_t)pointcloud.size();
		pointcloudParams_mapped->mvp = projection * view;
		// Write point cloud data to GPU memory
		glm::vec4* pointcloud_mapped = (glm::vec4*)device->mapMemory(*bufferMemories[1], 0, VK_WHOLE_SIZE);
		std::memcpy(pointcloud_mapped, pointcloud.data(), pointcloud.size() * sizeof(Point));
		// Clear image to 0xFFFFFFFF
		uint32_t* image_mapped = (uint32_t*)device->mapMemory(*bufferMemories[2], 0, VK_WHOLE_SIZE);
		std::memset(image_mapped, 0xFF, sizeof(uint32_t) * width * height);

		vk::UniqueShaderModule shaderModule;
		vk::UniquePipelineLayout layout;
		vk::UniquePipelineCache cache;
		vk::UniquePipeline pipeline;

		Framework::setupComputePipeline("pointcloud.comp.spv", { *descriptorSetLayout }, *device, shaderModule, layout, cache, pipeline);

		vk::CommandBufferAllocateInfo allocateInfo(*commandPool, vk::CommandBufferLevel::ePrimary, 1);
		auto cmdBuffers = device->allocateCommandBuffersUnique(allocateInfo);

		vk::MemoryBarrier memoryBarrier(vk::AccessFlagBits::eShaderWrite, vk::AccessFlagBits::eHostRead);
		cmdBuffers[0]->begin(vk::CommandBufferBeginInfo{});
		cmdBuffers[0]->bindPipeline(vk::PipelineBindPoint::eCompute, *pipeline);
		cmdBuffers[0]->bindDescriptorSets(vk::PipelineBindPoint::eCompute, *layout, 0, *descriptorSet, {});

		// TODO: Pick the number of work groups we need to run. The group size should be 128.
		// Make sure we have enough work groups so there are at least as many threads as there
		// are points (pointcloud.size()).
		cmdBuffers[0]->dispatch(1, 1, 1);
		cmdBuffers[0]->pipelineBarrier(vk::PipelineStageFlagBits::eComputeShader, vk::PipelineStageFlagBits::eHost, {}, memoryBarrier, {}, {});
		cmdBuffers[0]->end();

		Framework::initDebugging();
		Framework::beginCapture();
		queue.submit(vk::SubmitInfo({}, {}, * cmdBuffers[0]));
		Framework::endCapture();

		device->waitIdle();

		std::vector<uint32_t> image(image_mapped, image_mapped + width * height);
		std::for_each(image.begin(), image.end(), [](uint32_t& rgbd) {
			rgbd = 0xFF000000 | ((rgbd & 0xF) << 4) | ((rgbd & 0xF0) << 8) | ((rgbd & 0xF00) << 12);
			});
		stbi_write_png("output.png", width, height, STBI_rgb_alpha, image.data(), 0);

		std::cout << "Program finished, please check the image "
			<< (std::filesystem::current_path() / std::filesystem::path("output.png")).string() << std::endl;
	}
	catch (Framework::NotImplemented e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

/*
==================================== Task 7 ====================================
0) Pass the path of an .obj file in the assets directory as a program argument.
1) Create your resources and descriptor sets.
2) Modify the C++ structs so that their contents can be read by the GPU.
3) Compute the number of required work groups to process one point per thread.
4) Complete the shader file pointcloud.comp to perform point cloud rendering.
5) Optional: What happens if you don't use atomics or add the depth to the color?
*/