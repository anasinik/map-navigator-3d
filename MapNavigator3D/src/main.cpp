#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <cmath>

#include "../include/util.hpp"
#include "../include/Camera.hpp"
#include "../include/Map3D.hpp"
#include "../include/Mesh.hpp"
#include "../include/Model.hpp"
#include "../include/Shader.hpp"
#include "../include/Overlay.hpp"

static bool mouseInsideIcon(double mx, double my, const Overlay& overlay, int windowHeight)
{
    float mouseY_OpenGL = windowHeight - my;

    return (mx >= overlay.iconX_px &&
        mx <= overlay.iconX_px + overlay.iconWidth_px &&
        mouseY_OpenGL >= overlay.iconY_px &&
        mouseY_OpenGL <= overlay.iconY_px + overlay.iconHeight_px);
}

static void toggleWalkingMode(bool& walkingMode, Camera& camera, glm::vec3& playerPos)
{
    walkingMode = !walkingMode;

    if (walkingMode) {
        camera.setPosition(camera.getPosition().x, 15.0f, camera.getPosition().z);
        playerPos = glm::vec3(0.0f, 0.5f, 0.0f);
    }
    else {
        camera.setPosition(camera.getPosition().x, 25.0f, camera.getPosition().z);
    }
}

static glm::vec3 screenToWorld(double mouseX, double mouseY, int screenW, int screenH,
    const glm::mat4& view, const glm::mat4& projection, float mapWidth, float mapHeight)
{
    float x = (2.0f * static_cast<float>(mouseX)) / screenW - 1.0f;
    float y = 1.0f - (2.0f * static_cast<float>(mouseY)) / screenH;

    glm::vec4 rayClip(x, y, -1.0f, 1.0f);
    glm::vec4 rayEye = glm::inverse(projection) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    glm::vec3 rayWorld = glm::vec3(glm::inverse(view) * rayEye);
    rayWorld = glm::normalize(rayWorld);

    glm::vec3 camPos = glm::inverse(view)[3];
    float t = -camPos.y / rayWorld.y;

    glm::vec3 intersection = camPos + rayWorld * t;

    intersection.x = glm::clamp(intersection.x, -mapWidth / 2, mapWidth / 2);
    intersection.z = glm::clamp(intersection.z, -mapHeight / 2, mapHeight / 2);

    return intersection;
}

static int findPinAtPosition(const std::vector<MeasurementPin>& pins, const glm::vec3& worldPos, float threshold = 0.5f)
{
    for (size_t i = 0; i < pins.size(); i++) {
        float dist = glm::distance(glm::vec2(pins[i].position.x, pins[i].position.z),
            glm::vec2(worldPos.x, worldPos.z));
        if (dist < threshold) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

static float recalculateTotalDistance(const std::vector<MeasurementPin>& pins)
{
    float total = 0.0f;

    if (pins.size() < 2) return 0.0f;
    
    for (size_t i = 0; i < pins.size() - 1; i++) {
        total += glm::distance(pins[i].position, pins[i + 1].position);
    }
    return total;
}

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

    GLFWcursor* compassCursor = loadImageToCursor("textures/compas.png");
    if (compassCursor) {
        glfwSetCursor(window, compassCursor);
    }

    Shader mapShader("shaders/map_shader.vert", "shaders/map_shader.frag");
    Shader modelShader("shaders/model_shader.vert", "shaders/model_shader.frag");
    Shader pinShader("shaders/pin_shader.vert", "shaders/pin_shader.frag");
    Shader lineShader("shaders/line_shader.vert", "shaders/line_shader.frag");

    unsigned int iconShader = createShader("shaders/icon.vert", "shaders/icon.frag");

    Model humanoid("models/humanoid.obj");

    Map3D map("textures/novi-sad-map.jpg", 40.0f, 40.0f);
    Camera camera;

    Overlay overlay("textures/pin.png", "textures/walking_icon.png", "textures/ruler.png");
    std::cout << "Overlay created successfully\n";
    overlay.loadFont("fonts/arial.ttf", 24);

    glm::vec3 playerPos(0.0f, 0.5f, 0.0f);
    float playerRotation = 0.0f;
    float walkedDistance = 0.0f;

    bool walkingMode = true;
    overlay.setWalkingMode(true);
    camera.setPosition(0.0f, 15.0f, 0.0f);

    std::vector<MeasurementPin> measurementPins;
    float totalMeasuredDistance = 0.0f;

    double lastTime = glfwGetTime();
    bool clickHandled = false;
    bool rWasPressed = false;

    int fbW, fbH;
    glfwGetFramebufferSize(window, &fbW, &fbH);

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        double now = glfwGetTime();
        float dt = static_cast<float>(now - lastTime);
        lastTime = now;

        // Camera movement
        float camSpeed = 10.0f * dt;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            camera.moveXZ(0.0f, -camSpeed);
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            camera.moveXZ(0.0f, camSpeed);
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            camera.moveXZ(-camSpeed, 0.0f);
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            camera.moveXZ(camSpeed, 0.0f);

        glm::vec3 camPos = camera.getPosition();
        camPos.x = glm::clamp(camPos.x, -20.0f, 20.0f);
        camPos.z = glm::clamp(camPos.z, -20.0f, 20.0f);
        camera.setPosition(camPos.x, camPos.y, camPos.z);

        // Player movement
        if (walkingMode) {
            float speed = 5.0f * dt;
            glm::vec3 moveDir(0.0f);

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                moveDir.z -= 1.0f;
                playerRotation = 180.0f;
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                moveDir.z += 1.0f;
                playerRotation = 0.0f;
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                moveDir.x -= 1.0f;
                playerRotation = -90.0f; 
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                moveDir.x += 1.0f;
                playerRotation = 90.0f;
            }

            if (glm::length(moveDir) > 0.0f) {
                moveDir = glm::normalize(moveDir);
                glm::vec3 oldPos = playerPos;
                playerPos += moveDir * speed;
                playerPos.x = glm::clamp(playerPos.x, -20.0f, 20.0f);
                playerPos.z = glm::clamp(playerPos.z, -20.0f, 20.0f);

                walkedDistance += glm::distance(oldPos, playerPos);
            }
        }

        // R key toggle
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !rWasPressed) {
            toggleWalkingMode(walkingMode, camera, playerPos);
            overlay.setWalkingMode(walkingMode);
            rWasPressed = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) {
            rWasPressed = false;
        }

        // Mouse input
        double mx, my;
        glfwGetCursorPos(window, &mx, &my);

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            if (!clickHandled) {
                if (mouseInsideIcon(mx, my, overlay, fbH)) { 
                    toggleWalkingMode(walkingMode, camera, playerPos);
                    overlay.setWalkingMode(walkingMode);
                }
                else if (!walkingMode) {
                    glm::mat4 view = camera.getViewMatrix();
                    glm::mat4 projection = camera.getProjectionMatrix(static_cast<float>(fbW) / fbH);

                    glm::vec3 worldPos = screenToWorld(mx, my, fbW, fbH, view, projection,
                        map.getWidth(), map.getHeight());

                    int pinIndex = findPinAtPosition(measurementPins, worldPos);

                    if (pinIndex != -1) {
                        measurementPins.erase(measurementPins.begin() + pinIndex);
                        totalMeasuredDistance = recalculateTotalDistance(measurementPins);
                    }
                    else {
                        MeasurementPin pin;
                        pin.position = worldPos;
                        pin.xNorm = (worldPos.x + map.getWidth() / 2) / map.getWidth();
                        pin.zNorm = (worldPos.z + map.getHeight() / 2) / map.getHeight();

                        if (!measurementPins.empty()) {
                            float dist = glm::distance(measurementPins.back().position, pin.position);
                            totalMeasuredDistance += dist;
                        }

                        measurementPins.push_back(pin);
                    }
                }
                clickHandled = true;
            }
        }
        else {
            clickHandled = false;
        }

        // Rendering
        glViewport(0, 0, fbW, fbH);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix(static_cast<float>(fbW) / fbH);

        // Map
        mapShader.use();
        mapShader.setMat4("uView", view);
        mapShader.setMat4("uProjection", projection);
        mapShader.setVec3("uLightPos", 0.0f, 20.0f, 0.0f);
        mapShader.setVec3("uLightColor", 1.0f, 1.0f, 1.0f);
        mapShader.setVec3("uViewPos", camera.getPosition());
        
        int numLights = walkingMode ? 0 : glm::min(static_cast<int>(measurementPins.size()), 100);
        mapShader.setInt("uNumRedLights", numLights);

        if (!walkingMode) {
            for (int i = 0; i < numLights; i++) {
                glm::vec3 lightPos = measurementPins[i].position + glm::vec3(0.0f, 2.0f, 0.0f);
                std::string uniformName = "uRedLightPositions[" + std::to_string(i) + "]";
                mapShader.setVec3(uniformName.c_str(), lightPos);
            }
        }

        glm::mat4 modelMap = glm::mat4(1.0f);
        mapShader.setMat4("uModel", modelMap);
        map.render(mapShader.ID);
        // Player
        if (walkingMode) {
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

            modelShader.setMat4("uModel", modelHuman);
            humanoid.Draw(modelShader);
        }

        if (!walkingMode && !measurementPins.empty()) {
            if (measurementPins.size() >= 2) {
                map.renderLines(lineShader.ID, measurementPins, view, projection);
            }

            map.renderPins(pinShader.ID, measurementPins, view, projection, camera.getPosition());
        }

        glDisable(GL_DEPTH_TEST);

        overlay.drawWalkIcon(iconShader, fbW, fbH);
        overlay.drawRulerIcon(iconShader, fbW, fbH);

        overlay.drawFilledRect(static_cast<float>(fbW - 420), static_cast<float>(fbH - 120),
            400, 100, 0, 0, 0, 0.7f, fbW, fbH);

        if (walkingMode) {
            overlay.drawText("MODE: WALKING", static_cast<float>(fbW - 400), 40.0f,
                1, 1, 1, 1, fbW, fbH);

            overlay.drawText(("DISTANCE: " + std::to_string(static_cast<int>(walkedDistance)) + " px").c_str(),
                static_cast<float>(fbW - 400), 70.0f, 1, 1, 1, 1, fbW, fbH);
        }
        else {
            overlay.drawText("MODE: MEASUREMENT", static_cast<float>(fbW - 400), 40.0f,
                1, 1, 1, 1, fbW, fbH);

            overlay.drawText(("TOTAL: " + std::to_string(static_cast<int>(totalMeasuredDistance)) + " px").c_str(),
                static_cast<float>(fbW - 400), 70.0f, 1, 1, 1, 1, fbW, fbH);

            overlay.drawText(("PINS: " + std::to_string(measurementPins.size())).c_str(),
                static_cast<float>(fbW - 400), 100.0f, 1, 1, 1, 1, fbW, fbH);
        }

        overlay.drawFilledRect(20, 20, 350, 50, 0, 0, 0, 0.7f, fbW, fbH);

        overlay.drawText("ANA SINIK, SV11/2022", 40, static_cast<float>(fbH) - 45.0f, 1, 1, 1, 1, fbW, fbH);

        glEnable(GL_DEPTH_TEST);
        glfwSwapBuffers(window);
        glfwPollEvents();

        while (glfwGetTime() - lastTime < 1.0 / 75.0) {}
    }

    if (compassCursor) {
        glfwDestroyCursor(compassCursor);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}