add_executable(03_StorageBuffer main.cpp)

addGLSL(03_StorageBuffer shaders/fibonacci.comp)

target_include_directories(03_StorageBuffer PRIVATE Vulkan::Headers)
target_link_libraries(03_StorageBuffer Vulkan::Vulkan framework)