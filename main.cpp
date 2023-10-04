#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "basic_camera.h"
#include "fan.h"
#include "cylinders.h"
#include <iostream>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1500;
const unsigned int SCR_HEIGHT = 800;

// modelling transform
float rotateAngle_X = 0.0;
float rotateAngle_Y = 0.0;
float rotateAngle_Z = 0.0;
float rotateAxis_X = 0.0;
float rotateAxis_Y = 0.0;
float rotateAxis_Z = 1.0;
float translate_X = 0.0;
float translate_Y = 0.0;
float translate_Z = 0.0;
float scale_X = 1.0;
float scale_Y = 1.0;
float scale_Z = 1.0;
bool fan_turn = false;
bool rotate_around = false;

// camera
Camera camera(glm::vec3(-3.0f, 2.5f, 4.3f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//Eye position
float eyeX = 0.0, eyeY = 1.0, eyeZ = 4.1;
float lookAtX = 0.0, lookAtY = 0.0, lookAtZ = 0.0;
glm::vec3 V = glm::vec3(0.0f, 1.0f, 0.0f);
BasicCamera basic_camera(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, V);

// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;

glm::mat4 transforamtion(float tx, float ty, float tz, float rx, float ry, float rz, float sx, float sy, float sz) {
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    translateMatrix = glm::translate(identityMatrix, glm::vec3(tx, ty, tz));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rx), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rz), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(sx, sy, sz));
    model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    return model;
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CSE 4208: Computer Graphics Laboratory", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    
    //axis line 
    float cube_vertices2[] = {
        -0.25f, -0.25f, -0.25f, 0.0f, 0.0f, 0.0f,
        0.25f, -0.25f, -0.25f, 0.0f, 0.0f, 0.0f,
        0.25f, 0.25f, -0.25f, 0.0f, 0.0f, 0.0f,
        -0.25f, 0.25f, -0.25f, 0.0f, 0.0f, 0.0f,
        -0.25f, -0.25f, 0.25f, 0.3f, 0.8f, 0.5f,
        0.25f, -0.25f, 0.25f, 0.5f, 0.4f, 0.3f,
        0.25f, 0.25f, 0.25f, 0.2f, 0.7f, 0.3f,
        -0.25f, 0.25f, 0.25f, 0.6f, 0.2f, 0.8f,
    };
    float cube_vertices[] = {
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,

        0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,

        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.5f, 1.0f, 0.0f, 1.0f
    };
    unsigned int cube_indices[] = {
        0, 3, 2,
        2, 1, 0,

        4, 5, 7,
        7, 6, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };
    float floor[] = {
        0.0f, 0.0f, 0.0f, 0.69f, 0.69f, 0.69f,
        0.5f, 0.0f, 0.0f, 0.69f, 0.69f, 0.69f,
        0.5f, 0.5f, 0.0f, 0.69f, 0.69f, 0.69f,
        0.0f, 0.5f, 0.0f, 0.69f, 0.69f, 0.69f,

        0.5f, 0.0f, 0.0f, 0.69f, 0.69f, 0.69f,
        0.5f, 0.5f, 0.0f, 0.69f, 0.69f, 0.69f,
        0.5f, 0.0f, 0.5f, 0.69f, 0.69f, 0.69f,
        0.5f, 0.5f, 0.5f, 0.69f, 0.69f, 0.69f,

        0.0f, 0.0f, 0.5f, 0.69f, 0.69f, 0.69f,
        0.5f, 0.0f, 0.5f, 0.69f, 0.69f, 0.69f,
        0.5f, 0.5f, 0.5f, 0.69f, 0.69f, 0.69f,
        0.0f, 0.5f, 0.5f, 0.69f, 0.69f, 0.69f,

        0.0f, 0.0f, 0.5f, 0.69f, 0.69f, 0.69f,
        0.0f, 0.5f, 0.5f, 0.69f, 0.69f, 0.69f,
        0.0f, 0.5f, 0.0f, 0.69f, 0.69f, 0.69f,
        0.0f, 0.0f, 0.0f, 0.69f, 0.69f, 0.69f,

        0.5f, 0.5f, 0.5f, 0.69f, 0.69f, 0.69f,
        0.5f, 0.5f, 0.0f, 0.69f, 0.69f, 0.69f,
        0.0f, 0.5f, 0.0f, 0.69f, 0.69f, 0.69f,
        0.0f, 0.5f, 0.5f, 0.69f, 0.69f, 0.69f,

        0.0f, 0.0f, 0.0f, 0.69f, 0.69f, 0.69f,
        0.5f, 0.0f, 0.0f, 0.69f, 0.69f, 0.69f,
        0.5f, 0.0f, 0.5f, 0.69f, 0.69f, 0.69f,
        0.0f, 0.0f, 0.5f, 0.69f, 0.69f, 0.69f
    };
    float wall1[] = {
        0.0f, 0.0f, 0.0f, 0.92f, 0.91f, 0.83f,
        0.5f, 0.0f, 0.0f, 0.92f, 0.91f, 0.83f,
        0.5f, 0.5f, 0.0f, 0.92f, 0.91f, 0.83f,
        0.0f, 0.5f, 0.0f, 0.92f, 0.91f, 0.83f,

        0.5f, 0.0f, 0.0f, 0.92f, 0.91f, 0.83f,
        0.5f, 0.5f, 0.0f, 0.92f, 0.91f, 0.83f,
        0.5f, 0.0f, 0.5f, 0.92f, 0.91f, 0.83f,
        0.5f, 0.5f, 0.5f, 0.92f, 0.91f, 0.83f,

        0.0f, 0.0f, 0.5f, 0.92f, 0.91f, 0.83f,
        0.5f, 0.0f, 0.5f, 0.92f, 0.91f, 0.83f,
        0.5f, 0.5f, 0.5f, 0.92f, 0.91f, 0.83f,
        0.0f, 0.5f, 0.5f, 0.92f, 0.91f, 0.83f,

        0.0f, 0.0f, 0.5f, 0.92f, 0.91f, 0.83f,
        0.0f, 0.5f, 0.5f, 0.92f, 0.91f, 0.83f,
        0.0f, 0.5f, 0.0f, 0.92f, 0.91f, 0.83f,
        0.0f, 0.0f, 0.0f, 0.92f, 0.91f, 0.83f,

        0.5f, 0.5f, 0.5f, 0.92f, 0.91f, 0.83f,
        0.5f, 0.5f, 0.0f, 0.92f, 0.91f, 0.83f,
        0.0f, 0.5f, 0.0f, 0.92f, 0.91f, 0.83f,
        0.0f, 0.5f, 0.5f, 0.92f, 0.91f, 0.83f,

        0.0f, 0.0f, 0.0f, 0.92f, 0.91f, 0.83f,
        0.5f, 0.0f, 0.0f, 0.92f, 0.91f, 0.83f,
        0.5f, 0.0f, 0.5f, 0.92f, 0.91f, 0.83f,
        0.0f, 0.0f, 0.5f, 0.92f, 0.91f, 0.83f
    };
    float wall2[] = {
        0.0f, 0.0f, 0.0f, 0.70, 0.77, 0.75,
        0.5f, 0.0f, 0.0f, 0.70, 0.77, 0.75,
        0.5f, 0.5f, 0.0f, 0.70, 0.77, 0.75,
        0.0f, 0.5f, 0.0f, 0.70, 0.77, 0.75,

        0.5f, 0.0f, 0.0f, 0.70, 0.77, 0.75,
        0.5f, 0.5f, 0.0f, 0.70, 0.77, 0.75,
        0.5f, 0.0f, 0.5f, 0.70, 0.77, 0.75,
        0.5f, 0.5f, 0.5f, 0.70, 0.77, 0.75,

        0.0f, 0.0f, 0.5f, 0.70, 0.77, 0.75,
        0.5f, 0.0f, 0.5f, 0.70, 0.77, 0.75,
        0.5f, 0.5f, 0.5f, 0.70, 0.77, 0.75,
        0.0f, 0.5f, 0.5f, 0.70, 0.77, 0.75,

        0.0f, 0.0f, 0.5f, 0.70, 0.77, 0.75,
        0.0f, 0.5f, 0.5f, 0.70, 0.77, 0.75,
        0.0f, 0.5f, 0.0f, 0.70, 0.77, 0.75,
        0.0f, 0.0f, 0.0f, 0.70, 0.77, 0.75,

        0.5f, 0.5f, 0.5f, 0.70, 0.77, 0.75,
        0.5f, 0.5f, 0.0f, 0.70, 0.77, 0.75,
        0.0f, 0.5f, 0.0f, 0.70, 0.77, 0.75,
        0.0f, 0.5f, 0.5f, 0.70, 0.77, 0.75,

        0.0f, 0.0f, 0.0f, 0.70, 0.77, 0.75,
        0.5f, 0.0f, 0.0f, 0.70, 0.77, 0.75,
        0.5f, 0.0f, 0.5f, 0.70, 0.77, 0.75,
        0.0f, 0.0f, 0.5f, 0.70, 0.77, 0.75,
    };
    float box[] = {
        0.0f, 0.0f, 0.0f, 0.647f, 0.165f, 0.165f,
        0.5f, 0.0f, 0.0f, 0.647f, 0.165f, 0.165f,
        0.5f, 0.5f, 0.0f, 0.647f, 0.165f, 0.165f,
        0.0f, 0.5f, 0.0f, 0.647f, 0.165f, 0.165f,

        0.5f, 0.0f, 0.0f, 0.647f, 0.165f, 0.165f,
        0.5f, 0.5f, 0.0f, 0.647f, 0.165f, 0.165f,
        0.5f, 0.0f, 0.5f, 0.647f, 0.165f, 0.165f,
        0.5f, 0.5f, 0.5f, 0.647f, 0.165f, 0.165f,

        0.0f, 0.0f, 0.5f, 0.647f, 0.165f, 0.165f,
        0.5f, 0.0f, 0.5f, 0.647f, 0.165f, 0.165f,
        0.5f, 0.5f, 0.5f, 0.647f, 0.165f, 0.165f,
        0.0f, 0.5f, 0.5f, 0.647f, 0.165f, 0.165f,

        0.0f, 0.0f, 0.5f, 0.647f, 0.165f, 0.165f,
        0.0f, 0.5f, 0.5f, 0.647f, 0.165f, 0.165f,
        0.0f, 0.5f, 0.0f, 0.647f, 0.165f, 0.165f,
        0.0f, 0.0f, 0.0f, 0.647f, 0.165f, 0.165f,

        0.5f, 0.5f, 0.5f, 0.647f, 0.165f, 0.165f,
        0.5f, 0.5f, 0.0f, 0.647f, 0.165f, 0.165f,
        0.0f, 0.5f, 0.0f, 0.647f, 0.165f, 0.165f,
        0.0f, 0.5f, 0.5f, 0.647f, 0.165f, 0.165f,

        0.0f, 0.0f, 0.0f, 0.647f, 0.165f, 0.165f,
        0.5f, 0.0f, 0.0f, 0.647f, 0.165f, 0.165f,
        0.5f, 0.0f, 0.5f, 0.647f, 0.165f, 0.165f,
        0.0f, 0.0f, 0.5f, 0.647f, 0.165f, 0.165f
    };
    float box2[] = {
        0.0f, 0.0f, 0.0f, 0,0,0,
        0.5f, 0.0f, 0.0f, 0,0,0,
        0.5f, 0.5f, 0.0f, 0,0,0,
        0.0f, 0.5f, 0.0f, 0,0,0,

        0.5f, 0.0f, 0.0f, 0,0,0,
        0.5f, 0.5f, 0.0f, 0,0,0,
        0.5f, 0.0f, 0.5f, 0,0,0,
        0.5f, 0.5f, 0.5f, 0,0,0,

        0.0f, 0.0f, 0.5f, 0,0,0,
        0.5f, 0.0f, 0.5f, 0,0,0,
        0.5f, 0.5f, 0.5f, 0,0,0,
        0.0f, 0.5f, 0.5f, 0,0,0,

        0.0f, 0.0f, 0.5f, .41, .24, .12,
        0.0f, 0.5f, 0.5f, .41, .24, .12,
        0.0f, 0.5f, 0.0f, .41, .24, .12,
        0.0f, 0.0f, 0.0f, .41, .24, .12,

        0.5f, 0.5f, 0.5f, .41, .24, .12,
        0.5f, 0.5f, 0.0f, .41, .24, .12,
        0.0f, 0.5f, 0.0f, .41, .24, .12,
        0.0f, 0.5f, 0.5f, .41, .24, .12,

        0.0f, 0.0f, 0.0f, 0,0,0,
        0.5f, 0.0f, 0.0f, 0,0,0,
        0.5f, 0.0f, 0.5f, 0,0,0,
        0.0f, 0.0f, 0.5f, 0,0,0
    };
    float ac[] = {
        0.0f, 0.0f, 0.0f,0.2f, 0.2f, 0.2f,
        0.25f, 0.0f, 0.0f, 0.2f, 0.2f, 0.2f,
        0.25f, 0.25f, 0.0f, 0.2f, 0.2f, 0.2f,
        0.0f, 0.5f, 0.0f, 0.2f, 0.2f, 0.2f,

        0.25f, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f,
        0.25f, 0.25f, 0.0f, 0.8f, 0.8f, 0.8f,
        0.25f, 0.0f, 0.5f, 0.8f, 0.8f, 0.8f,
        0.25f, 0.25f, 0.5f, 0.8f, 0.8f, 0.8f,

        0.0f, 0.0f, 0.5f, 0.2f, 0.2f, 0.2f,
        0.25f, 0.0f, 0.5f, 0.2f, 0.2f, 0.2f,
        0.25f, 0.25f, 0.5f, 0.2f, 0.2f, 0.2f,
        0.0f, 0.5f, 0.5f, 0.2f, 0.2f, 0.2f,

        0.0f, 0.0f, 0.5f, 0.2f, 0.2f, 0.2f,
        0.0f, 0.5f, 0.5f, 0.2f, 0.2f, 0.2f,
        0.0f, 0.5f, 0.0f, 0.2f, 0.2f, 0.2f,
        0.0f, 0.0f, 0.0f, 0.2f, 0.2f, 0.2f,

        0.25f, 0.25f, 0.5f, 0.2f, 0.2f, 0.2f,
        0.25f, 0.25f, 0.0f, 0.2f, 0.2f, 0.2f,
        0.0f, 0.5f, 0.0f, 0.2f, 0.2f, 0.2f,
        0.0f, 0.5f, 0.5f, 0.2f, 0.2f, 0.2f,

        0.0f, 0.0f, 0.0f, 0.2f, 0.2f, 0.2f,
        0.25f, 0.0f, 0.0f, 0.2f, 0.2f, 0.2f,
        0.25f, 0.0f, 0.5f, 0.2f, 0.2f, 0.2f,
        0.0f, 0.0f, 0.5f, 0.2f, 0.2f, 0.2f
    };
    float ceiling[] = {
        0.0f, 0.0f, 0.0f, 0.95f, 0.95f, 0.95f,
        0.5f, 0.0f, 0.0f, 0.95f, 0.95f, 0.95f,
        0.5f, 0.5f, 0.0f, 0.95f, 0.95f, 0.95f,
        0.0f, 0.5f, 0.0f, 0.95f, 0.95f, 0.95f,

        0.5f, 0.0f, 0.0f, 0.95f, 0.95f, 0.95f,
        0.5f, 0.5f, 0.0f, 0.95f, 0.95f, 0.95f,
        0.5f, 0.0f, 0.5f, 0.95f, 0.95f, 0.95f,
        0.5f, 0.5f, 0.5f, 0.95f, 0.95f, 0.95f,

        0.0f, 0.0f, 0.5f, 0.95f, 0.95f, 0.95f,
        0.5f, 0.0f, 0.5f, 0.95f, 0.95f, 0.95f,
        0.5f, 0.5f, 0.5f, 0.95f, 0.95f, 0.95f,
        0.0f, 0.5f, 0.5f, 0.95f, 0.95f, 0.95f,

        0.0f, 0.0f, 0.5f, 0.95f, 0.95f, 0.95f,
        0.0f, 0.5f, 0.5f, 0.95f, 0.95f, 0.95f,
        0.0f, 0.5f, 0.0f, 0.95f, 0.95f, 0.95f,
        0.0f, 0.0f, 0.0f, 0.95f, 0.95f, 0.95f,

        0.5f, 0.5f, 0.5f, 0.95f, 0.95f, 0.95f,
        0.5f, 0.5f, 0.0f, 0.95f, 0.95f, 0.95f,
        0.0f, 0.5f, 0.0f, 0.95f, 0.95f, 0.95f,
        0.0f, 0.5f, 0.5f, 0.95f, 0.95f, 0.95f,

        0.0f, 0.0f, 0.0f, 0.95f, 0.95f, 0.95f,
        0.5f, 0.0f, 0.0f, 0.95f, 0.95f, 0.95f,
        0.5f, 0.0f, 0.5f, 0.95f, 0.95f, 0.95f,
        0.0f, 0.0f, 0.5f, 0.95f, 0.95f, 0.95f
    };
    float lamp_ver[] = {
       

       0.5f, 0.0f, 0.0f, 0.96, .89, .26,
       0.4f, 0.5f, 0.1f, 0.96, .89, .26,
       0.5f, 0.0f, 0.5f, 0.96, .52, .26,
       0.4f, 0.5f, 0.4f, 0.96, .52, .26,

       0.0f, 0.0f, 0.0f, 0.96, .52, .26,
       0.5f, 0.0f, 0.0f, 0.96, .52, .26,
       0.1f, 0.5f, 0.1f, 0.96, .89, .26,
       0.4f, 0.5f, 0.1f, 0.96, .89, .26,

       0.0f, 0.0f, 0.5f, 0.96, .52, .26,
       0.5f, 0.0f, 0.5f, 0.96, .52, .26,
       0.4f, 0.5f, 0.4f, 0.96, .89, .26,
       0.1f, 0.5f, 0.4f, 0.96, .89, .26,

       0.0f, 0.0f, 0.5f, 0.96, .52, .26,
       0.1f, 0.5f, 0.4f, 0.96, .52, .26,
       0.1f, 0.5f, 0.1f, 0.96, .89, .26,
       0.0f, 0.0f, 0.0f, 0.96, .89, .26,

       0.4f, 0.5f, 0.4f, 0,0,0,
       0.4f, 0.5f, 0.1f, 0,0,0,
       0.1f, 0.5f, 0.1f, 0,0,0,
       0.1f, 0.5f, 0.4f, 0,0,0,

       0.0f, 0.0f, 0.0f, 0.96, .89, .26,
       0.5f, 0.0f, 0.0f, 0.96, .89, .26,
       0.5f, 0.0f, 0.5f, 0.96, .89, .26,
       0.0f, 0.0f, 0.5f, 0.96, .89, .26,
    };
    float fan_holder[] = {
        0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,

        0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,

        0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,

        0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,

        0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f
    };
    float fan_pivot[] = {
        0.0f, 0.0f, 0.0f, .44f, .22f, .05f,
        0.5f, 0.0f, 0.0f, .44f, .22f, .05f,
        0.5f, 0.5f, 0.0f, .44f, .22f, .05f,
        0.0f, 0.5f, 0.0f, .44f, .22f, .05f,

        0.5f, 0.0f, 0.0f, .44f, .22f, .05f,
        0.5f, 0.5f, 0.0f, .44f, .22f, .05f,
        0.5f, 0.0f, 0.5f, .44f, .22f, .05f,
        0.5f, 0.5f, 0.5f, .44f, .22f, .05f,

        0.0f, 0.0f, 0.5f, .44f, .22f, .05f,
        0.5f, 0.0f, 0.5f, .44f, .22f, .05f,
        0.5f, 0.5f, 0.5f, .44f, .22f, .05f,
        0.0f, 0.5f, 0.5f, .44f, .22f, .05f,

        0.0f, 0.0f, 0.5f, .44f, .22f, .05f,
        0.0f, 0.5f, 0.5f, .44f, .22f, .05f,
        0.0f, 0.5f, 0.0f, .44f, .22f, .05f,
        0.0f, 0.0f, 0.0f, .44f, .22f, .05f,

        0.5f, 0.5f, 0.5f, .44f, .22f, .05f,
        0.5f, 0.5f, 0.0f, .44f, .22f, .05f,
        0.0f, 0.5f, 0.0f, .44f, .22f, .05f,
        0.0f, 0.5f, 0.5f, .44f, .22f, .05f,

        0.0f, 0.0f, 0.0f, .44f, .22f, .05f,
        0.5f, 0.0f, 0.0f, .44f, .22f, .05f,
        0.5f, 0.0f, 0.5f, .44f, .22f, .05f,
        0.0f, 0.0f, 0.5f, .44f, .22f, .05f
    };
    float fan_blade[] = {
        0.0f, 0.0f, 0.0f, .0f, .0f, .42f,
        0.5f, 0.0f, 0.0f, .0f, .0f, .42f,
        0.5f, 0.5f, 0.0f, .0f, .0f, .42f,
        0.0f, 0.5f, 0.0f, .0f, .0f, .42f,

        0.5f, 0.0f, 0.0f, .0f, .0f, .42f,
        0.5f, 0.5f, 0.0f, .0f, .0f, .42f,
        0.5f, 0.0f, 0.5f, .0f, .0f, .42f,
        0.5f, 0.5f, 0.5f, .0f, .0f, .42f,

        0.0f, 0.0f, 0.5f, .0f, .0f, .42f,
        0.5f, 0.0f, 0.5f, .0f, .0f, .42f,
        0.5f, 0.5f, 0.5f, .0f, .0f, .42f,
        0.0f, 0.5f, 0.5f, .0f, .0f, .42f,

        0.0f, 0.0f, 0.5f, .0f, .0f, .42f,
        0.0f, 0.5f, 0.5f, .0f, .0f, .42f,
        0.0f, 0.5f, 0.0f, .0f, .0f, .42f,
        0.0f, 0.0f, 0.0f, .0f, .0f, .42f,

        0.5f, 0.5f, 0.5f, .0f, .0f, .42f,
        0.5f, 0.5f, 0.0f, .0f, .0f, .42f,
        0.0f, 0.5f, 0.0f, .0f, .0f, .42f,
        0.0f, 0.5f, 0.5f, .0f, .0f, .42f,

        0.0f, 0.0f, 0.0f, .0f, .0f, .42f,
        0.5f, 0.0f, 0.0f, .0f, .0f, .42f,
        0.5f, 0.0f, 0.5f, .0f, .0f, .42f,
        0.0f, 0.0f, 0.5f, .0f, .0f, .42f
    };
    float glass[] = {
        0.0f, 0.0f, 0.0f, 0.53f, 0.8f, 0.98f,
        0.5f, 0.0f, 0.0f, 0.53f, 0.8f, 0.98f,
        0.5f, 0.5f, 0.0f, 0.53f, 0.8f, 0.98f,
        0.0f, 0.5f, 0.0f, 0.53f, 0.8f, 0.98f,

        0.5f, 0.0f, 0.0f, 0.53f, 0.8f, 0.98f,
        0.5f, 0.5f, 0.0f, 0.53f, 0.8f, 0.98f,
        0.5f, 0.0f, 0.5f, 0.53f, 0.8f, 0.98f,
        0.5f, 0.5f, 0.5f, 0.53f, 0.8f, 0.98f,

        0.0f, 0.0f, 0.5f, 0.53f, 0.8f, 0.98f,
        0.5f, 0.0f, 0.5f, 0.53f, 0.8f, 0.98f,
        0.5f, 0.5f, 0.5f, 0.53f, 0.8f, 0.98f,
        0.0f, 0.5f, 0.5f, 0.53f, 0.8f, 0.98f,

        0.0f, 0.0f, 0.5f, 0.53f, 0.8f, 0.98f,
        0.0f, 0.5f, 0.5f, 0.53f, 0.8f, 0.98f,
        0.0f, 0.5f, 0.0f, 0.53f, 0.8f, 0.98f,
        0.0f, 0.0f, 0.0f, 0.53f, 0.8f, 0.98f,

        0.5f, 0.5f, 0.5f, 0.53f, 0.8f, 0.98f,
        0.5f, 0.5f, 0.0f, 0.53f, 0.8f, 0.98f,
        0.0f, 0.5f, 0.0f, 0.53f, 0.8f, 0.98f,
        0.0f, 0.5f, 0.5f, 0.53f, 0.8f, 0.98f,

        0.0f, 0.0f, 0.0f, 0.53f, 0.8f, 0.98f,
        0.5f, 0.0f, 0.0f, 0.53f, 0.8f, 0.98f,
        0.5f, 0.0f, 0.5f, 0.53f, 0.8f, 0.98f,
        0.0f, 0.0f, 0.5f, 0.53f, 0.8f, 0.98f
    };
    float cabinate[] = {
        0.0f, 0.0f, 0.0f, 0.29f, 0.0f, 0.29f,
        0.5f, 0.0f, 0.0f, 0.29f, 0.0f, 0.29f,
        0.5f, 0.5f, 0.0f, 0.29f, 0.0f, 0.29f,
        0.0f, 0.5f, 0.0f, 0.29f, 0.0f, 0.29f,

        0.5f, 0.0f, 0.0f, 0.29f, 0.0f, 0.29f,
        0.5f, 0.5f, 0.0f, 0.29f, 0.0f, 0.29f,
        0.5f, 0.0f, 0.5f, 0.29f, 0.0f, 0.29f,
        0.5f, 0.5f, 0.5f, 0.29f, 0.0f, 0.29f,

        0.0f, 0.0f, 0.5f, 0.29f, 0.0f, 0.29f,
        0.5f, 0.0f, 0.5f, 0.29f, 0.0f, 0.29f,
        0.5f, 0.5f, 0.5f, 0.29f, 0.0f, 0.29f,
        0.0f, 0.5f, 0.5f, 0.29f, 0.0f, 0.29f,

        0.0f, 0.0f, 0.5f, 0.29f, 0.0f, 0.29f,
        0.0f, 0.5f, 0.5f, 0.29f, 0.0f, 0.29f,
        0.0f, 0.5f, 0.0f, 0.29f, 0.0f, 0.29f,
        0.0f, 0.0f, 0.0f, 0.29f, 0.0f, 0.29f,

        0.5f, 0.5f, 0.5f, 0.29f, 0.0f, 0.29f,
        0.5f, 0.5f, 0.0f, 0.29f, 0.0f, 0.29f,
        0.0f, 0.5f, 0.0f, 0.29f, 0.0f, 0.29f,
        0.0f, 0.5f, 0.5f, 0.29f, 0.0f, 0.29f,

        0.0f, 0.0f, 0.0f, 0.29f, 0.0f, 0.29f,
        0.5f, 0.0f, 0.0f, 0.29f, 0.0f, 0.29f,
        0.5f, 0.0f, 0.5f, 0.29f, 0.0f, 0.29f,
        0.0f, 0.0f, 0.5f, 0.29f, 0.0f, 0.29f
    };
    float tv1[] = {
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,

        0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
    };


    unsigned int VBOCA, VAOCA, EBOCA;
    glGenVertexArrays(1, &VAOCA);
    glGenBuffers(1, &VBOCA);
    glGenBuffers(1, &EBOCA);
    glBindVertexArray(VAOCA);
    glBindBuffer(GL_ARRAY_BUFFER, VBOCA);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cabinate), cabinate, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOCA);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    unsigned int VBOT, VAOT, EBOT;
    glGenVertexArrays(1, &VAOT);
    glGenBuffers(1, &VBOT);
    glGenBuffers(1, &EBOT);
    glBindVertexArray(VAOT);
    glBindBuffer(GL_ARRAY_BUFFER, VBOT);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ceiling), ceiling, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOT);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    
    unsigned int VBOLMP, VAOLMP, EBOLMP;
    glGenVertexArrays(1, &VAOLMP);
    glGenBuffers(1, &VBOLMP);
    glGenBuffers(1, &EBOLMP);
    glBindVertexArray(VAOLMP);
    glBindBuffer(GL_ARRAY_BUFFER, VBOLMP);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lamp_ver), lamp_ver, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOLMP);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ac), ac, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    unsigned int VBOG, VAOG, EBOG;
    glGenVertexArrays(1, &VAOG);
    glGenBuffers(1, &VBOG);
    glGenBuffers(1, &EBOG);
    glBindVertexArray(VAOG);
    glBindBuffer(GL_ARRAY_BUFFER, VBOG);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor), floor, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOG);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    unsigned int VBOW, VAOW, EBOW;
    glGenVertexArrays(1, &VAOW);
    glGenBuffers(1, &VBOW);
    glGenBuffers(1, &EBOW);
    glBindVertexArray(VAOW);
    glBindBuffer(GL_ARRAY_BUFFER, VBOW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wall1), wall1, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    unsigned int VBOW1, VAOW1, EBOW1;
    glGenVertexArrays(1, &VAOW1);
    glGenBuffers(1, &VBOW1);
    glGenBuffers(1, &EBOW1);
    glBindVertexArray(VAOW1);
    glBindBuffer(GL_ARRAY_BUFFER, VBOW1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wall2), wall2, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOW1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    unsigned int VBOC, VAOC, EBOC;
    glGenVertexArrays(1, &VAOC);
    glGenBuffers(1, &VBOC);
    glGenBuffers(1, &EBOC);
    glBindVertexArray(VAOC);
    glBindBuffer(GL_ARRAY_BUFFER, VBOC);
    glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOC);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    unsigned int VBOC2, VAOC2, EBOC2;
    glGenVertexArrays(1, &VAOC2);
    glGenBuffers(1, &VBOC2);
    glGenBuffers(1, &EBOC2);
    glBindVertexArray(VAOC2);
    glBindBuffer(GL_ARRAY_BUFFER, VBOC2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(box2), box2, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOC2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    unsigned int VBOTV, VAOTV, EBOTV;
    glGenVertexArrays(1, &VAOTV);
    glGenBuffers(1, &VBOTV);
    glGenBuffers(1, &EBOTV);
    glBindVertexArray(VAOTV);
    glBindBuffer(GL_ARRAY_BUFFER, VBOTV);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tv1), tv1, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOTV);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    //Fan
    unsigned int VBOF1, VAOF1, EBOF1;
    glGenVertexArrays(1, &VAOF1);
    glGenBuffers(1, &VBOF1);
    glGenBuffers(1, &EBOF1);
    glBindVertexArray(VAOF1);
    glBindBuffer(GL_ARRAY_BUFFER, VBOF1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fan_holder), fan_holder, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOF1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    unsigned int VBOF2, VAOF2, EBOF2;
    glGenVertexArrays(1, &VAOF2);
    glGenBuffers(1, &VBOF2);
    glGenBuffers(1, &EBOF2);
    glBindVertexArray(VAOF2);
    glBindBuffer(GL_ARRAY_BUFFER, VBOF2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fan_pivot), fan_pivot, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOF2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    unsigned int VBOF3, VAOF3, EBOF3;
    glGenVertexArrays(1, &VAOF3);
    glGenBuffers(1, &VBOF3);
    glGenBuffers(1, &EBOF3);
    glBindVertexArray(VAOF3);
    glBindBuffer(GL_ARRAY_BUFFER, VBOF3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fan_blade), fan_blade, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOF3);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);
    

    unsigned int VBOS, VAOS, EBOS;
    glGenVertexArrays(1, &VAOS);
    glGenBuffers(1, &VBOS);
    glGenBuffers(1, &EBOS);
    glBindVertexArray(VAOS);
    glBindBuffer(GL_ARRAY_BUFFER, VBOS);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glass), glass, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOS);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    int i = 0;


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader
        ourShader.use();
        glm::mat4 model,rotate,scale;
        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        //glm::mat4 view = basic_camera.createViewMatrix();
        ourShader.setMat4("view", view);


        //Floor
        model = transforamtion(0, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 20, 0.1, 20);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOG);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //Ceiling
        model = transforamtion(0, 5, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 20, 0.1, 20);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOT);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //Wall1
        model = transforamtion(0, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 20, 10, 0.1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOW);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(0, 0, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 20, 10, 0.1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOW);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //Wall2
        model = transforamtion(10, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.1, 10, 20);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOW1);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //AC
        model = transforamtion(10, 4, 5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -5, 2, 6);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //TV Cabinate
        model = transforamtion(6, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -6, 2, 2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(6.05, 1, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -6.2, .15, 2.12);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(6.05, .5, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -6.2, .15, 2.12);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(6.05, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -6.2, .15, 2.12);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        //tv
        model = transforamtion(5.60, 1.4, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -4.5, 2.75, 0.2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //tv box
        model = transforamtion(5.60, 1.075, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -1, 0.2, 0.7);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //tv remote
        model = transforamtion(4.5, 1.075, 0.5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -1, 0.1, 0.2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        //Almirah
        //base of almirah
        model = transforamtion(10, 0.25, .5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -2.5, 6.5, 6);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //head of almirah
        model = transforamtion(10, 3.5, 0.4, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -2.54, .5, 6.4);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //foot of almirah
        model = transforamtion(10, 0, 0.4, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -2.54, .5, 6.4);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //left palla
        model = transforamtion(8.75, .5, 0.635, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -0.1, 5.6, 2.5);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //right palla
        model = transforamtion(8.75, .5, 3.365, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -0.1, 5.6, -2.5);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //left handle
        model = transforamtion(8.75, 2, 1.65, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -0.2, 0.7, -0.1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //right handle
        model = transforamtion(8.75, 2, 2.35, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -0.2, 0.7, -0.1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        


        //sofa 1
        //boshar base
        model = transforamtion(10, 0, 5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -3, 1.5, 6);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //pichoner base
        model = transforamtion(10, 0, 5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -1, 4, 6);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //left er corner
        model = transforamtion(10, 0, 5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -3, 3, -1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //black cover
        model = transforamtion(10, 1.5, 5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -3, 0.1, -1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(8.5, 0, 5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -.1, 3.1, -1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //right er corner
        model = transforamtion(10, 0, 8, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -3, 3, 1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(10, 1.5, 8, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -3, 0.1, 1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(8.5, 0, 8, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -.1, 3.1, 1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //cover
        model = transforamtion(10, 0.75, 5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -3, 0.5, 6);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOW);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        //paposh 
        // nicher layer    
        model = transforamtion(5.8, 0, 3, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -6, 0.2, 3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        // nicher layer    
        model = transforamtion(5.7, 0.1, 3.05, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -5.6, 0.05, 2.8);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        

        //Lamp
        model = transforamtion(8.5, 1.75, 9, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1.2, 2.35, 1.2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOLMP);
        glDrawElements(GL_TRIANGLES, 96, GL_UNSIGNED_INT, 0);

        model = transforamtion(8.8, 0, 9.3, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .15, 4, .15);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(8.5, 0, 9, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1.2, 0.5, 1.2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 96, GL_UNSIGNED_INT, 0);
   
        
        
        //sofa 2
        //boshar base
        model = transforamtion(6.6, 0, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -9, 1.5, -3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //black cover
        model = transforamtion(6.6, 1.5, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1, 0.1, -3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        model = transforamtion(6.6, 0, 8.5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1, 3.1, -.1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //right er corner
        model = transforamtion(6.6, 0, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1, 3, -3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //left er corner
        model = transforamtion(2.1, 0, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -1, 3, -3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //black cover
        model = transforamtion(2.1, 1.5, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -1, 0.1, -3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        model = transforamtion(2.1, 0, 8.5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -1, 3.1, -.1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //pichoner base
        model = transforamtion(6.6, 0, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -9, 4, -1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //bed sheet
        model = transforamtion(6.6, 0.75, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -9, 0.5, -3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOW);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //windows
        //pordar hanger
        model = transforamtion(2.65, 4, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 8.4, 1, -.5);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //black portion
        model = transforamtion(3, 1.5, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 7, 5, -.15);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(3.15, 1.65, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 3.05, 4.5, -.151);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOS);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(4.825, 1.65, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 3.05, 4.5, -.151);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOS);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);



        //Tea table
        model = transforamtion(5.8, 0.6, 6.2, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -6, .8, 3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //samner left paaaa
        model = transforamtion(5.8, 0.0, 6.2, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -.3, 1.2, .3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //samner right paaaa
        model = transforamtion(2.8, 0.0, 6.2, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .3, 1.2, .3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //pichoner left paa
        model = transforamtion(5.8, 0.0, 7.7, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -.3, 1.2, -.3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //pichoner right paa
        model = transforamtion(2.8, 0.0, 7.7, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .3, 1.2, -.3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //uporer part
        model = transforamtion(5.85, 1, 6.15, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -6.2, .1, 3.2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        //wallmat left
        model = transforamtion(1, 1.5, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 2.5, 5,.15);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(1.075, 1.65, 0.075, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 2.2, 4.4, .05);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        

        //Fan
        //uporer part
        model = transforamtion(5, 5, 5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1, -.2, 1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF1);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //hanger
        model = transforamtion(5.2125, 4.9, 5.2125, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .15, -1, .15);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //nicher part
        model = transforamtion(5, 4.4, 5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1, -.33, 1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF1);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        Fan fan;
        ourShader = fan.local_rotation(ourShader, VAOF3, i);

        if (fan_turn)
            i += 5;
        if (rotate_around)
           camera.ProcessKeyboard(Y_LEFT, deltaTime);
        //nicher part
        model = transforamtion(5, 4.225, 5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1, -.05, 1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF1);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
    glDeleteVertexArrays(1, &VAOCA);
    glDeleteBuffers(1, &VBOCA);
    glDeleteBuffers(1, &EBOCA);

    glDeleteVertexArrays(1, &VAOT);
    glDeleteBuffers(1, &VBOT);
    glDeleteBuffers(1, &EBOT);

    glDeleteVertexArrays(1, &VAOG);
    glDeleteBuffers(1, &VBOG);
    glDeleteBuffers(1, &EBOG);

    glDeleteVertexArrays(1, &VAOW);
    glDeleteBuffers(1, &VBOW);
    glDeleteBuffers(1, &EBOW);

    glDeleteVertexArrays(1, &VAOW1);
    glDeleteBuffers(1, &VBOW1);
    glDeleteBuffers(1, &EBOW1);

    glDeleteVertexArrays(1, &VAOC);
    glDeleteBuffers(1, &VBOC);
    glDeleteBuffers(1, &EBOC);

    glDeleteVertexArrays(1, &VAOC2);
    glDeleteBuffers(1, &VBOC2);
    glDeleteBuffers(1, &EBOC2);

    glDeleteVertexArrays(1, &VAOTV);
    glDeleteBuffers(1, &VBOTV);
    glDeleteBuffers(1, &EBOTV);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        camera.ProcessKeyboard(P_UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        camera.ProcessKeyboard(P_DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        camera.ProcessKeyboard(Y_LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        camera.ProcessKeyboard(Y_RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        camera.ProcessKeyboard(R_LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        camera.ProcessKeyboard(R_RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        if (!fan_turn) {
            fan_turn = true;
        }
        else {
            fan_turn = false;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        if (!rotate_around) {
            rotate_around = true;
        }
        else {
            rotate_around = false;
        }
    
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
