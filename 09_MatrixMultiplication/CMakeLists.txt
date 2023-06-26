add_executable(09_MatrixMultiplication main.cpp)

addGLSL(09_MatrixMultiplication shaders/matrixmult.comp)

target_include_directories(09_MatrixMultiplication PRIVATE Vulkan::Headers)
target_link_libraries(09_MatrixMultiplication Vulkan::Vulkan framework)