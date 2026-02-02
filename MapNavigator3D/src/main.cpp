#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "../include/Util.hpp"

int main() {
	
    if (!glfwInit()) return endProgram("GLFW init failed");

    // Window setup
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	return 0;
}