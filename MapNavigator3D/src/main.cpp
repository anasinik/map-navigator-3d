#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "../include/util.hpp"
#include "../include/Camera.hpp"
#include "../include/Map3D.hpp"

int main() {
    if (!glfwInit()) return endProgram("GLFW init failed");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height,
        "MapNavigator3D", primary, NULL);
    if (!window) return endProgram("Window creation failed");

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
        return endProgram("GLEW init failed");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);

    unsigned int shader3D = createShader("shaders/vertex_shader_3d.vert",
        "shaders/fragment_shader_3d.frag");

    Map3D map("textures/novi-sad-map.jpg", 40.0f, 40.0f);

    Camera camera;
    camera.setPosition(0.0f, 15.0f, 15.0f);

    double lastTime = glfwGetTime();

    int fbW, fbH;
    glfwGetFramebufferSize(window, &fbW, &fbH);
    std::cout << "Framebuffer: " << fbW << "x" << fbH << std::endl;
    std::cout << "Aspect ratio: " << (float)fbW / fbH << std::endl;


    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        double now = glfwGetTime();
        float dt = float(now - lastTime);
        lastTime = now;

        float speed = 10.0f * dt;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            camera.moveBy(0, 0, -speed);
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            camera.moveBy(0, 0, speed);
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            camera.moveBy(-speed, 0, 0);
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            camera.moveBy(speed, 0, 0);

        glViewport(0, 0, fbW, fbH);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader3D);
        glUniform1i(glGetUniformLocation(shader3D, "uTexture"), 0);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix((float)fbW / fbH);

        glUniformMatrix4fv(glGetUniformLocation(shader3D, "uModel"),
            1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shader3D, "uView"),
            1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader3D, "uProjection"),
            1, GL_FALSE, glm::value_ptr(projection));

        // Svetlo
        glUniform3f(glGetUniformLocation(shader3D, "uLightPos"), 0.0f, 20.0f, 0.0f);
        glUniform3f(glGetUniformLocation(shader3D, "uLightColor"), 1.0f, 1.0f, 1.0f);
        glUniform3fv(glGetUniformLocation(shader3D, "uViewPos"),
            1, glm::value_ptr(camera.getPosition()));

        map.render(shader3D);

        glfwSwapBuffers(window);
        glfwPollEvents();

        while (glfwGetTime() - lastTime < 1.0 / 75.0) {}
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}