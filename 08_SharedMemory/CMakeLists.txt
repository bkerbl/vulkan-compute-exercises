add_executable(08_SharedMemory main.cpp)

addGLSL(08_SharedMemory shaders/communicate.comp)

target_include_directories(08_SharedMemory PRIVATE Vulkan::Headers)
target_link_libraries(08_SharedMemory Vulkan::Vulkan framework)