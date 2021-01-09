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
Camera camera(glm::vec3(0.0f, 10.0f, 0.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = false;

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

    // Setup some OpenGL options
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    Ocean ocean(256, 0.00002, 256, glm::vec2(64,32));

	glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	// 渲染循环
	while (!glfwWindowShouldClose(window)) {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        printf("fps:%f\n", 1 / deltaTime);
        Sleep(128);

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::vec3 CamPos = camera.Position;
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        GLuint ReflectText;
        ocean.Update(currentFrame, model, view, projection, CamPos, ReflectText);

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
    camera.ProcessMouseMovement(xOffset, yOffset);
}

void mouse_scroll_callback(GLFWwindow *window,double x,double y){
    camera.ProcessMouseScroll(y);
}
// utility function for loading a 2D texture from file
// ---------------------------------------------------
/*
GLuint loadTexture(char const * path)
{
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
*/


