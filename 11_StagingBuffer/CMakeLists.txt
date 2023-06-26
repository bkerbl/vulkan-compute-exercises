add_executable(11_StagingBuffer main.cpp)

addGLSL(11_StagingBuffer shaders/reduce_sequential.comp)

target_include_directories(11_StagingBuffer PRIVATE Vulkan::Headers)
target_link_libraries(11_StagingBuffer Vulkan::Vulkan framework)