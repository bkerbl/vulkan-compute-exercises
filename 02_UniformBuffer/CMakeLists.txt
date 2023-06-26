add_executable(02_UniformBuffer main.cpp)

addGLSL(02_UniformBuffer shaders/print.comp)

target_include_directories(02_UniformBuffer PRIVATE Vulkan::Headers)
target_link_libraries(02_UniformBuffer Vulkan::Vulkan framework)