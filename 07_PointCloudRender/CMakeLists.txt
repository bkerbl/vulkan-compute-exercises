add_executable(07_PointCloudRender main.cpp)

addGLSL(07_PointCloudRender shaders/pointcloud.comp)

target_include_directories(07_PointCloudRender PRIVATE Vulkan::Headers)
target_link_libraries(07_PointCloudRender Vulkan::Vulkan framework)