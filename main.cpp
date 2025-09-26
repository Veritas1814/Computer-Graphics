#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);

// declaring usfull consts
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 4.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw   = -90.0f;
float pitch =  0.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool mouseCaptured = false;
//end

// Declaring shaders
const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

// Fragment shader A (bright pulsating green-blue)
const char* fragmentShaderSourceA = R"(
#version 330 core
out vec4 FragColor;
uniform float iTime;
void main() {
    float green = 0.5 + 0.5 * sin(iTime * 2.0);
    float blue  = 0.5 + 0.5 * cos(iTime * 1.5);
    FragColor = vec4(0.2, green, blue, 1.0);
}
)";

// Fragment shader B (pulsating red with alpha change)
const char* fragmentShaderSourceB = R"(
#version 330 core
out vec4 FragColor;
uniform float iTime;
void main() {
    float red   = 0.5 + 0.5 * cos(iTime * 1.2);
    float alpha = 0.5 + 0.5 * sin(iTime * 2.5);
    FragColor = vec4(red, 0.2, 0.8, alpha);
}
)";
// End of Shaders

float cubeVertices[] = {
    // front
    -0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,
     0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f,-0.5f, 0.5f,
    // back
    -0.5f,-0.5f,-0.5f, -0.5f, 0.5f,-0.5f,  0.5f, 0.5f,-0.5f,
     0.5f, 0.5f,-0.5f,  0.5f,-0.5f,-0.5f, -0.5f,-0.5f,-0.5f,
    // left
    -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,-0.5f, -0.5f,-0.5f,-0.5f,
    -0.5f,-0.5f,-0.5f, -0.5f,-0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
    // right
     0.5f, 0.5f, 0.5f,  0.5f,-0.5f, 0.5f,  0.5f,-0.5f,-0.5f,
     0.5f,-0.5f,-0.5f,  0.5f, 0.5f,-0.5f,  0.5f, 0.5f, 0.5f,
    // top
    -0.5f, 0.5f,-0.5f, -0.5f, 0.5f, 0.5f,  0.5f, 0.5f, 0.5f,
     0.5f, 0.5f, 0.5f,  0.5f, 0.5f,-0.5f, -0.5f, 0.5f,-0.5f,
    // bottom
    -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f,-0.5f, 0.5f,
     0.5f,-0.5f, 0.5f, -0.5f,-0.5f, 0.5f, -0.5f,-0.5f,-0.5f
};

// Created functions for better code
unsigned int compileShader(GLenum type, const char* src) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    return shader;
}

unsigned int createProgram(const char* fragSrc) {
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragSrc);
    unsigned int prog = glCreateProgram();

    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Window", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD\n";
        return -1;
    }

    // seting vertexes
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int programA = createProgram(fragmentShaderSourceA);
    unsigned int programB = createProgram(fragmentShaderSourceB);

    // Depth test
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        //For frame-independent
        float curFrame = glfwGetTime();
        deltaTime = curFrame - lastFrame;
        lastFrame = curFrame;
        processInput(window);

        glClearColor(0.2f,0.3f,0.3f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //rotation
        float x = 1 * cos(curFrame * 1);
        float y = 1 * sin(curFrame * 1);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x, y, 0.0f));
        model = glm::rotate(model, curFrame * glm::radians(50.0f), glm::vec3(1.0f, 1.0f, 0.0f));

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos+cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f),(float)SCR_WIDTH/SCR_HEIGHT,0.1f,100.0f);

        glBindVertexArray(VAO);

        //For deviding cube sides for 2 triangles with different shaders
        for(int i=0; i<12; i++) {
            unsigned int prog = (i % 2 == 0) ? programA : programB;
            glUseProgram(prog);

            int modelLoc = glGetUniformLocation(prog,"model");
            int viewLoc  = glGetUniformLocation(prog,"view");
            int projLoc  = glGetUniformLocation(prog,"projection");
            int timeLoc  = glGetUniformLocation(prog,"iTime");

            glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(model));
            glUniformMatrix4fv(viewLoc,1,GL_FALSE,glm::value_ptr(view));
            glUniformMatrix4fv(projLoc,1,GL_FALSE,glm::value_ptr(projection));
            glUniform1f(timeLoc, curFrame);

            glDrawArrays(GL_TRIANGLES, i*3, 3);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);
    glDeleteProgram(programA);
    glDeleteProgram(programB);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
//For window resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0,0,width,height);
}

// Tracks mouse movement
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    //To ignore mouse movement on edges(had stupid behaviour)
    if (!mouseCaptured) return;

    //prevents camera jump
    if (firstMouse) {
        lastX=xpos;
        lastY=ypos;
        firstMouse=false;
    }

    //offset
    float xoffset=xpos-lastX;
    float yoffset=lastY-ypos;
    lastX=xpos;
    lastY=ypos;

    //sensa
    float sensitivity=0.1f;
    xoffset*=sensitivity;
    yoffset*=sensitivity;

    //camera angles
    yaw+=xoffset;
    pitch+=yoffset;

    //not fliping
    if(pitch>89.0f) pitch=89.0f;
    if(pitch<-89.0f) pitch=-89.0f;

    //converting to direction vecotr
    glm::vec3 front;
    front.x=cos(glm::radians(yaw))*cos(glm::radians(pitch));
    front.y=sin(glm::radians(pitch));
    front.z=sin(glm::radians(yaw))*cos(glm::radians(pitch));
    cameraFront=glm::normalize(front);
}
//input for camera
void processInput(GLFWwindow* window) {
    //close window
    if(glfwGetKey(window,GLFW_KEY_ESCAPE)==GLFW_PRESS) {
        glfwSetWindowShouldClose(window,true);
    }
    //Mouse capture toggle (right mouse button)
    if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT)==GLFW_PRESS && !mouseCaptured){
        mouseCaptured=true; firstMouse=true;
        glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
    }
    else if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT)==GLFW_RELEASE && mouseCaptured){
        mouseCaptured=false;
        glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
    }

    //speed of camera
    float cameraSpeed=2.5f*deltaTime;

    //Movement controls (WASD + vertical)
    if(glfwGetKey(window,GLFW_KEY_W)==GLFW_PRESS) {
        cameraPos+=cameraSpeed*cameraFront;
    }
    if(glfwGetKey(window,GLFW_KEY_S)==GLFW_PRESS) {
        cameraPos-=cameraSpeed*cameraFront;
    }
    if(glfwGetKey(window,GLFW_KEY_A)==GLFW_PRESS) {
        cameraPos-=glm::normalize(glm::cross(cameraFront,cameraUp))*cameraSpeed;
    }
    if(glfwGetKey(window,GLFW_KEY_D)==GLFW_PRESS) {
        cameraPos+=glm::normalize(glm::cross(cameraFront,cameraUp))*cameraSpeed;
    }
    if(glfwGetKey(window,GLFW_KEY_SPACE)==GLFW_PRESS) {
        cameraPos+=glm::vec3(0.0f,cameraSpeed,0.0f);
    }
    if(glfwGetKey(window,GLFW_KEY_LEFT_SHIFT)==GLFW_PRESS){
    cameraPos-=glm::vec3(0.0f,cameraSpeed,0.0f);
    }
}