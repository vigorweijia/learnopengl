cmake_minimum_required(VERSION 3.20)
project(learngl)

set(CMAKE_CXX_STANDARD 17)

include_directories(thirdparty/glm)
include_directories(thirdparty/glfw/include)
include_directories(thirdparty/glad/include)
link_directories(thirdparty/glfw/lib)
include_directories(thirdparty/assimp/include)
link_directories(thirdparty/assimp/lib)
include_directories(thirdparty/utf8cpp/source)

file(GLOB_RECURSE glad_src ./thirdparty/glad/src/*.c)
file(GLOB_RECURSE src ./src/*)

add_executable(learngl ${src} ${glad_src})
target_link_libraries(learngl glfw3 opengl32 assimp-vc142-mtd)