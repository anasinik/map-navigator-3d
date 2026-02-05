#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "../include/util.hpp"
#include "../include/Camera.hpp"
#include "../include/Map3D.hpp"
#include "../include/Mesh.hpp"
#include "../include/Model.hpp"
#include "../include/Shader.hpp"

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

    Shader mapShader("shaders/map_shader.vert", "shaders/map_shader.frag");

    Shader modelShader("shaders/model_shader.vert", "shaders/model_shader.frag");

    Model humanoid("models/humanoid.obj");
    
    Map3D map("textures/novi-sad-map.jpg", 40.0f, 40.0f);
    Camera camera;

    glm::vec3 playerPos(0.0f, 0.5f, 0.0f);
    float playerRotation = 0.0f;

    double lastTime = glfwGetTime();
    int fbW, fbH;
    glfwGetFramebufferSize(window, &fbW, &fbH);

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        double now = glfwGetTime();
        float dt = float(now - lastTime);
        lastTime = now;
        float speed = 5.0f * dt;

        float camSpeed = 10.0f * dt;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            camera.moveXZ(0.0f, -camSpeed);

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            camera.moveXZ(0.0f, camSpeed);

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            camera.moveXZ(-camSpeed, 0.0f);

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            camera.moveXZ(camSpeed, 0.0f);


        glm::vec3 moveDir(0.0f);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            moveDir.z -= 1.0f;
            playerRotation = 0.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            moveDir.z += 1.0f;
            playerRotation = 180.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            moveDir.x -= 1.0f;
            playerRotation = 90.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            moveDir.x += 1.0f;
            playerRotation = -90.0f;
        }

        glm::vec3 camPos = camera.getPosition();
        camPos.x = glm::clamp(camPos.x, -20.0f, 20.0f);
        camPos.z = glm::clamp(camPos.z, -20.0f, 20.0f);
        camera.setPosition(camPos.x, camPos.y, camPos.z);


        if (glm::length(moveDir) > 0.0f) {
            moveDir = glm::normalize(moveDir);
            playerPos += moveDir * speed;

            playerPos.x = glm::clamp(playerPos.x, -20.0f, 20.0f);
            playerPos.z = glm::clamp(playerPos.z, -20.0f, 20.0f);
        }

       
        glViewport(0, 0, fbW, fbH);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix((float)fbW / fbH);

        mapShader.use();
        mapShader.setMat4("uView", view);
        mapShader.setMat4("uProjection", projection);
        mapShader.setVec3("uLightPos", 0.0f, 20.0f, 0.0f);
        mapShader.setVec3("uLightColor", 1.0f, 1.0f, 1.0f);
        mapShader.setVec3("uViewPos", camera.getPosition());

        glm::mat4 modelMap = glm::mat4(1.0f);
        mapShader.setMat4("uModel", modelMap);
        mapShader.setInt("uTexture", 0);

        map.render(mapShader.ID);

        modelShader.use();
        modelShader.setMat4("uView", view);
        modelShader.setMat4("uProjection", projection);
        modelShader.setVec3("uLightPos", 0.0f, 20.0f, 0.0f);
        modelShader.setVec3("uLightColor", 1.0f, 1.0f, 1.0f);
        modelShader.setVec3("uViewPos", camera.getPosition());

        glm::mat4 modelHuman = glm::mat4(1.0f);
        modelHuman = glm::translate(modelHuman, playerPos);
        modelHuman = glm::rotate(modelHuman, glm::radians(playerRotation),
            glm::vec3(0.0f, 1.0f, 0.0f));
        modelHuman = glm::scale(modelHuman, glm::vec3(0.015f));

        glBindTexture(GL_TEXTURE_2D, 0);
        modelShader.setMat4("uModel", modelHuman);
        humanoid.Draw(modelShader);

        glfwSwapBuffers(window);
        glfwPollEvents();

        while (glfwGetTime() - lastTime < 1.0 / 75.0) {}
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}