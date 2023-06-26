add_executable(05_EdgeDetector main.cpp)

addGLSL(05_EdgeDetector shaders/sobel.comp)

target_include_directories(05_EdgeDetector PRIVATE Vulkan::Headers)
target_link_libraries(05_EdgeDetector Vulkan::Vulkan framework)