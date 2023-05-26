#ifndef __UTILS_H__
#define __UTILS_H__

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"

#include <iostream>
#include <filesystem>
#include <vector>
#include <string>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

unsigned int loadTexture(const char* path);

unsigned int loadCubemap(const std::vector<std::string>& faces);

#endif