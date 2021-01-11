#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <windows.h>
#include "../include/stb_image.h"

#include "../include/carema.h"
#include "../include/shader.h"
#include "../include/Ocean.h"
#include "../include/boat.h"
#include "../include/Skybox.h"
#include <vector>

#define PI M_PI


using namespace std;

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_pos_callback(GLFWwindow *window,double x,double y);
void mouse_scroll_callback(GLFWwindow *window,double x,double y);
void processInput(GLFWwindow *window);
void loadTexture(GLuint& texture, const char *filename);
GLuint loadCubemap(vector<const GLchar*> faces);
void drawSpere(GLfloat r, GLuint& VAO, GLuint& VBO);

// settings
const GLuint SCR_WIDTH = 800;
const GLuint SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 10.0f, -20.0f));
boat Boat;
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = false;
bool pause = false;
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_SAMPLES, 4);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr); // Windowed

	if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_pos_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);

    // Options
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::vector<std::string> skyboxPaths = {
        "wave/textures/skybox/skyboxLeft.png",
        "wave/textures/skybox/skyboxRight.png",
        "wave/textures/skybox/skyboxUp.png",
        "wave/textures/skybox/skyboxDown.png",
        "wave/textures/skybox/skyboxFront.png",
        "wave/textures/skybox/skyboxBack.png"
    };

    Shader skyboxShader("wave/shader/SkyboxShader.vs", "wave/shader/SkyboxShader.fs");
    Skybox skybox(skyboxPaths);
    Ocean ocean(1024, 0.00002, 256, glm::vec2(64, 32));
    // Necessary OpenGL Parameters
    glEnable(GL_DEPTH_TEST);
    // Enable gamma correction
    glEnable(GL_FRAMEBUFFER_SRGB);
    // Enable anti-aliasing
    glEnable(GL_MULTISAMPLE);
    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Boat.init();
	glm::mat4 oceanModel = glm::mat4(1.0f);
    oceanModel = glm::scale(oceanModel, glm::vec3(10, 1, 10));
    glm::mat4 modelInv = glm::transpose(glm::inverse(oceanModel));
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    double t = 0.25262233;
	// 渲染循环
	while (!glfwWindowShouldClose(window)) {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        double fps = 1 / deltaTime;
        printf("fps:%f\n", fps);
        //Sleep(128);

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::vec3 CamPos = camera.Position;
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        
        if (!pause) {
            skybox.Draw(skyboxShader, view, projection);
            ocean.Update(t, oceanModel, modelInv, view, projection, CamPos, skybox.getCubeMap());
            Boat.Draw(view, projection, deltaTime, ocean.getVertices());
            //camera.goWithBoat(Boat.Draw(view, projection, deltaTime, ocean.getVertices()), Boat.getRunFront(), Boat.getRight());
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            if (fps > 10) {
                t += 0.1 / 500.0 / (fps / 10); 
            } else {
                t += 0.1 / 500.0;
            }
        } else {
            skybox.Draw(skyboxShader, view, projection);
            ocean.Update(t, oceanModel, modelInv, view, projection, CamPos, skybox.getCubeMap());
            Boat.Draw(view, projection, deltaTime, ocean.getVertices());
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}
void processInput(GLFWwindow *window){
    static bool isPolygon = false;
    static double lastPressedTime = 0.0;
    if (!pause) {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE)==GLFW_PRESS)
            glfwSetWindowShouldClose(window,true);
        if(glfwGetKey(window, GLFW_KEY_W))
            camera.processKeyboard(FORWARD, deltaTime);
        if(glfwGetKey(window, GLFW_KEY_S))
            camera.processKeyboard(BACKWARD, deltaTime);
        if(glfwGetKey(window, GLFW_KEY_A))
            camera.processKeyboard(LEFT, deltaTime);
        if(glfwGetKey(window, GLFW_KEY_D))
            camera.processKeyboard(RIGHT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            Boat.ProcessKeyboard(GOFORWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            Boat.ProcessKeyboard(GOBACKWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            Boat.ProcessKeyboard(TURNLEFT, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            Boat.ProcessKeyboard(TURNRIGHT, deltaTime);
        }
	    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
	    	pause = false;
	    }


        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS
            && glfwGetTime() - lastPressedTime > 0.2) {
            if (isPolygon) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                isPolygon = false;
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                isPolygon = true;
            }
            lastPressedTime = glfwGetTime();
        }
    }

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS
        && glfwGetTime() - lastPressedTime > 0.2) {
        pause = !pause;
        lastPressedTime = glfwGetTime();
    }
}
void mouse_pos_callback(GLFWwindow *window,double x,double y){
    if(!firstMouse){
        lastX=x;
        lastY=y;
        firstMouse=true;
    }
    //std::cout<<x<< " "<<y<<std::endl;
    float xOffset=x-lastX;
    float yOffset=y-lastY;
    lastX=x;
    lastY=y;
    if (!pause)
        camera.ProcessMouseMovement(xOffset, yOffset);
}

void mouse_scroll_callback(GLFWwindow *window,double x,double y){
    camera.ProcessMouseScroll(y);
}