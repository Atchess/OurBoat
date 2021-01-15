#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <windows.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "../include/stb_image.h"
#include "../include/camera.h"
#include "../include/shader.h"
#include "../include/Ocean.h"
#include "../include/boat.h"
#include "../include/Skybox.h"
#include <vector>

#define PI M_PI


using namespace std;
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

void Imgui_Render();

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
Camera camera(glm::vec3(0.0f, 5.0f, -20.0f));
Skybox* pSky;
boat Boat;
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = false;
bool pause = true;
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

static float l = 0.0f;

glm::vec3 sunP = glm::vec3(50.0f, 50.0f, 50.0f);
static float sx = 0.0f;
static float sy = 0.0f;
static float sz = 0.0f;
ImFont* font1;
bool start = false;
bool isFan = false;

bool CameraFront = false;
int main()
{
    camera.MovementSpeed = 10.0f;
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_SAMPLES, 4);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    const char* glsl_version = "#version 130";

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Boat", nullptr, nullptr); // Windowed

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
        "../textures/skybox/skyboxLeft.png",
        "../textures/skybox/skyboxRight.png",
        "../textures/skybox/skyboxUp.png",
        "../textures/skybox/skyboxDown.png",
       "../textures/skybox/skyboxFront.png",
        "../textures/skybox/skyboxBack.png",
    };


    Shader skyboxShader("../shader/SkyboxShader.vs", "../shader/SkyboxShader.fs");
    Skybox skybox(skyboxPaths);
    pSky = &skybox;
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

#if defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }
        // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    font1 = io.Fonts->AddFontFromFileTTF("../font/Mitooh.TTF", 15.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    glm::mat4 oceanModel = glm::mat4(1.0f);
    oceanModel = glm::scale(oceanModel, glm::vec3(10, 1, 10));
    glm::mat4 modelInv = glm::transpose(glm::inverse(oceanModel));
    glm::mat4 view = camera.GetViewMatrix(Boat.getCenter(), CameraFront);
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
  
    double t = 0.25262233;
	// 渲染循环
	while (!glfwWindowShouldClose(window)) {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        double fps = 1 / deltaTime;
        //printf("fps:%f\n", fps);
        Sleep(30);

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::vec3 CamPos = camera.Position;
        view = camera.GetViewMatrix(Boat.getCenter(), CameraFront);
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        GLuint ReflectText = 1;
      
        //cout << " x:" << Boat.getFront().x << " y:" << Boat.getFront().y <<" z:"<<Boat.getFront().z << endl;
        //cout << " Position.x:" << Boat.getCenter().x << " Position.y:" << Boat.getCenter().y << " Position.z:" << Boat.getCenter().z << endl;
        if (start) {
           skybox.Draw(skyboxShader, view, projection);
            ocean.Update(t, oceanModel,modelInv, view, projection, CamPos, skybox.getCubeMap());
            camera.goWithBoat(Boat.Draw(view, projection, camera.Position,ocean.getLightPos(), ocean.getLightInfo(),ocean.getLightIntensity(),deltaTime, ocean.getVertices()));
        }
        else {
            skybox.Draw(skyboxShader, view, projection);
            ocean.Update(0, oceanModel, modelInv, view, projection, CamPos, skybox.getCubeMap());
            camera.goWithBoat(Boat.Draw(view, projection,camera.Position, ocean.getLightPos(), ocean.getLightInfo(), ocean.getLightIntensity(), deltaTime, ocean.getVertices()));
        }
        if (!pause) {
            if (fps > 10) {
                t += 0.0001 / 500.0 / (fps / 10);
            }
            else {
                t += 0.0001 / 500.0;
            }
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        if (abs(Boat.angle_side) >= 30.0f) {
            start = false;
            pause = true;
            isFan = true;
        }
        //cout << Boat.getCenter().x << " " << Boat.getCenter().y << " " << Boat.getCenter().z << endl;
        // Start the Dear ImGui frame
        Imgui_Render();

        skybox.setLightIntensity(l);
        ocean.setLightIntensity(l);
        ocean.setLightPos(sunP);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------

void Imgui_Render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    //if (show_demo_window)
    //	ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        static int counter = 0;

        ImGui::Begin("menu");                          // Create a window called "Hello, world!" and append into it.
        ImGui::PushFont(font1);
        if (ImGui::CollapsingHeader("Help")) {
            ImGui::BulletText("'The boat of friendship' by @Atchess @ZJUCS-zzh @tlxhlll");
        }
        //ImGui::PushFont(font3);
        ImGui::Text("Show something useful.");               // Display some text (you can use a format strings too)
        //ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        //ImGui::Checkbox("Another Window", &show_another_window);
        //ImGui::PopFont();
        //tjc
        ImGui::SliderFloat("light", &l, 0.0f, 10.0f);				//light

        //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
        ImGui::SliderFloat("sun_position_x", &sunP.x, -200.0f, 200.0f);
        ImGui::SliderFloat("sun_position_y", &sunP.y, -200.0f, 200.0f);
        ImGui::SliderFloat("sun_position_z", &sunP.z, -200.0f, 200.0f);
        //tjc
        //if (ImGui::Button("pause"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        //    pause = !pause;
        if (ImGui::Button("start")) {
            start = true;
            pause = false;
        }
        if (isFan) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "        LOSE!      ");
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Your Boat capsized ");
        }
        //ImGui::SameLine();
        //ImGui::Text("counter = %d", counter);
        ImGui::Text("the front/back speed of boat is %.3fkm/h", Boat.speed.x * 100);
        ImGui::Text("the right/left speed of boat is %.3fkm/h", Boat.speed.y * 100);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        float boot_rate = 0;
        ImGui::PopFont();
        //ImGui::Text("the boot front/back rate is %.3f", Boat.getSpeed1());
        //ImGui::Text("the boot right/left rate is %.3f", Boat.getSpeed2());
        ImGui::End();
    }

    // 3. Show another simple window.
    /*if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }*/

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
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
    // Some mode switches
    static bool isPolygon = false;
    static double lastPressedTime = 0.0;
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
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS
        && glfwGetTime() - lastPressedTime > 0.2) {
        //gDrawNormals = !gDrawNormals;
        lastPressedTime = glfwGetTime();
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        start = true;
        pause = false;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS
        && glfwGetTime() - lastPressedTime > 0.2) {
        start = !start;
        pause = !pause;
        lastPressedTime = glfwGetTime();
        if (isFan)isFan = false;
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS
        && glfwGetTime() - lastPressedTime > 0.2) {
        Boat.OutObj();
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS
        && glfwGetTime() - lastPressedTime > 0.2) {
        CameraFront = true;

    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS
        && glfwGetTime() - lastPressedTime > 0.2) {
        CameraFront = false;
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


