#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <../include/model.h>
#include <../include/shader.h>
#include <../include/stb_image.h>
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);

const unsigned int NUM_CUBES = 1000;
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 cameraPos   = glm::vec3(0.0f, 3.0f, 6.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, -0.3f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw   = -90.0f;
float pitch =  0.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool mouseCaptured = false;

float cubeVertices[] = {
    -0.5f,-0.5f,-0.5f,  0.0f,0.0f,-1.0f,  0.0f,0.0f,
     0.5f,-0.5f,-0.5f,  0.0f,0.0f,-1.0f,  1.0f,0.0f,
     0.5f, 0.5f,-0.5f,  0.0f,0.0f,-1.0f,  1.0f,1.0f,
     0.5f, 0.5f,-0.5f,  0.0f,0.0f,-1.0f,  1.0f,1.0f,
    -0.5f, 0.5f,-0.5f,  0.0f,0.0f,-1.0f,  0.0f,1.0f,
    -0.5f,-0.5f,-0.5f,  0.0f,0.0f,-1.0f,  0.0f,0.0f,

    -0.5f,-0.5f, 0.5f,  0.0f,0.0f,1.0f,   0.0f,0.0f,
     0.5f,-0.5f, 0.5f,  0.0f,0.0f,1.0f,   1.0f,0.0f,
     0.5f, 0.5f, 0.5f,  0.0f,0.0f,1.0f,   1.0f,1.0f,
     0.5f, 0.5f, 0.5f,  0.0f,0.0f,1.0f,   1.0f,1.0f,
    -0.5f, 0.5f, 0.5f,  0.0f,0.0f,1.0f,   0.0f,1.0f,
    -0.5f,-0.5f, 0.5f,  0.0f,0.0f,1.0f,   0.0f,0.0f,

    -0.5f, 0.5f, 0.5f, -1.0f,0.0f,0.0f,   1.0f,0.0f,
    -0.5f, 0.5f,-0.5f, -1.0f,0.0f,0.0f,   1.0f,1.0f,
    -0.5f,-0.5f,-0.5f, -1.0f,0.0f,0.0f,   0.0f,1.0f,
    -0.5f,-0.5f,-0.5f, -1.0f,0.0f,0.0f,   0.0f,1.0f,
    -0.5f,-0.5f, 0.5f, -1.0f,0.0f,0.0f,   0.0f,0.0f,
    -0.5f, 0.5f, 0.5f, -1.0f,0.0f,0.0f,   1.0f,0.0f,

     0.5f, 0.5f, 0.5f,  1.0f,0.0f,0.0f,   1.0f,0.0f,
     0.5f, 0.5f,-0.5f,  1.0f,0.0f,0.0f,   1.0f,1.0f,
     0.5f,-0.5f,-0.5f,  1.0f,0.0f,0.0f,   0.0f,1.0f,
     0.5f,-0.5f,-0.5f,  1.0f,0.0f,0.0f,   0.0f,1.0f,
     0.5f,-0.5f, 0.5f,  1.0f,0.0f,0.0f,   0.0f,0.0f,
     0.5f, 0.5f, 0.5f,  1.0f,0.0f,0.0f,   1.0f,0.0f,

    -0.5f,-0.5f,-0.5f,  0.0f,-1.0f,0.0f,  0.0f,1.0f,
     0.5f,-0.5f,-0.5f,  0.0f,-1.0f,0.0f,  1.0f,1.0f,
     0.5f,-0.5f, 0.5f,  0.0f,-1.0f,0.0f,  1.0f,0.0f,
     0.5f,-0.5f, 0.5f,  0.0f,-1.0f,0.0f,  1.0f,0.0f,
    -0.5f,-0.5f, 0.5f,  0.0f,-1.0f,0.0f,  0.0f,0.0f,
    -0.5f,-0.5f,-0.5f,  0.0f,-1.0f,0.0f,  0.0f,1.0f,

    -0.5f, 0.5f,-0.5f,  0.0f,1.0f,0.0f,   0.0f,1.0f,
     0.5f, 0.5f,-0.5f,  0.0f,1.0f,0.0f,   1.0f,1.0f,
     0.5f, 0.5f, 0.5f,  0.0f,1.0f,0.0f,   1.0f,0.0f,
     0.5f, 0.5f, 0.5f,  0.0f,1.0f,0.0f,   1.0f,0.0f,
    -0.5f, 0.5f, 0.5f,  0.0f,1.0f,0.0f,   0.0f,0.0f,
    -0.5f, 0.5f,-0.5f,  0.0f,1.0f,0.0f,   0.0f,1.0f
};

unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else std::cerr << "Failed to load texture: " << path << std::endl;
    stbi_image_free(data);
    return textureID;
}

int main() {
    srand((unsigned)time(nullptr));
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH,SCR_HEIGHT,"Cubes + Model + Lights",nullptr,nullptr);
    if(!window){glfwTerminate();return -1;}
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    glEnable(GL_DEPTH_TEST);

    Shader litModel(
        (std::string(PROJECT_ROOT) + "shaders/light_model_vertex.glsl").c_str(),
        (std::string(PROJECT_ROOT) + "shaders/blinn_phong.glsl").c_str()
    );
    Shader litCubes(
        (std::string(PROJECT_ROOT) + "shaders/light_cube_vertex.glsl").c_str(),
        (std::string(PROJECT_ROOT) + "shaders/blinn_phong.glsl").c_str()
    );
    Shader emissive(
        (std::string(PROJECT_ROOT) + "shaders/emissive_vert.glsl").c_str(),
        (std::string(PROJECT_ROOT) + "shaders/emissive_fragment.glsl").c_str()
    );
    Shader floorShader(
        (std::string(PROJECT_ROOT) + "shaders/floor_vertex.glsl").c_str(),
        (std::string(PROJECT_ROOT) + "shaders/floor_fragment.glsl").c_str()
    );

    Model myModel(std::string(PROJECT_ROOT) + "assets/lpshead/head.OBJ");
    Model lightSphere(std::string(PROJECT_ROOT) + "assets/sphere.obj");


    unsigned int VAO, VBO, instanceVBO;
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&instanceVBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    std::vector<glm::vec3> cubePositions;
    cubePositions.reserve(NUM_CUBES);
    float spread = std::sqrt((float)NUM_CUBES) / 50.0f;
    for (unsigned i=0;i<NUM_CUBES;++i){
        float x = ((rand()%200-100)/5.0f)*spread;
        float z = ((rand()%200-100)/5.0f)*spread;
        cubePositions.emplace_back(x,0.15f,z);
    }
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, cubePositions.size()*sizeof(glm::vec3), cubePositions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,sizeof(glm::vec3),(void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3,1);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    unsigned int cubeTex = loadTexture((std::string(PROJECT_ROOT) + "assets/dizhak(1)(1).jpg").c_str());

    float floorVertices[] = {
        -500.0f, 0.0f, -500.0f,
         500.0f, 0.0f, -500.0f,
         500.0f, 0.0f,  500.0f,
         500.0f, 0.0f,  500.0f,
        -500.0f, 0.0f,  500.0f,
        -500.0f, 0.0f, -500.0f
    };
    unsigned int floorVAO, floorVBO;
    glGenVertexArrays(1,&floorVAO);
    glGenBuffers(1,&floorVBO);
    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    unsigned int floorTex = loadTexture((std::string(PROJECT_ROOT) + "assets/floor.jpg").c_str());

    glm::vec3 dirLightDir = glm::normalize(glm::vec3(-0.2f,-1.0f,-0.3f));
    glm::vec3 dirLightCol = glm::vec3(1.0f,0.98f,0.9f);
    glm::vec3 pointPos[2] = {
        glm::vec3(-2.0f, 1.2f,  2.0f), // red
        glm::vec3( 3.0f, 0.8f, -3.0f)  // blue
    };

    glm::vec3 pointCol[2] = { glm::vec3(1.0f,0.25f,0.25f), glm::vec3(0.2f,0.7f,1.0f) };
    float attKc=1.0f, attKl=0.09f, attKq=0.032f;
    float cubeScale = 0.25f;
    const float lightMarkerScale = 0.02f;

    auto setLights = [&](Shader& sh, const glm::mat4& view, const glm::mat4& proj){
        sh.use();
        sh.setMat4("view", view);
        sh.setMat4("projection", proj);
        sh.setVec3("viewPos", cameraPos);
        sh.setInt("blinn", 1);
        sh.setFloat("shininess", 32.0f);
        sh.setVec3("specularColor", glm::vec3(0.4f));
        sh.setVec3("dirLight.direction", dirLightDir);
        sh.setVec3("dirLight.color", dirLightCol);
        for (int i=0;i<2;++i){
            std::string b = "pointLights[" + std::to_string(i) + "]";
            sh.setVec3(b + ".position",  pointPos[i]);
            sh.setVec3(b + ".color",     pointCol[i]);
            sh.setFloat(b + ".constant", attKc);
            sh.setFloat(b + ".linear",   attKl);
            sh.setFloat(b + ".quadratic",attKq);
        }
        sh.setVec3("spotLight.position",  cameraPos);
        sh.setVec3("spotLight.direction", cameraFront);
        sh.setVec3("spotLight.color", glm::vec3(1.0f));
        sh.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        sh.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
        sh.setFloat("spotLight.constant", attKc);
        sh.setFloat("spotLight.linear",   attKl);
        sh.setFloat("spotLight.quadratic",attKq);
    };

    while(!glfwWindowShouldClose(window)){
        float t = glfwGetTime();
        deltaTime = t - lastFrame;
        lastFrame = t;
        processInput(window);

        glClearColor(0.1f,0.15f,0.2f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 proj = glm::perspective(glm::radians(45.0f),(float)SCR_WIDTH/SCR_HEIGHT,0.1f,100.0f);

        setLights(litModel, view, proj);
        glm::mat4 M(1.0f);
        M = glm::scale(M, glm::vec3(7.0f));
        M = glm::translate(M, glm::vec3(0.0f,0.4f,0.0f));
        litModel.setMat4("model", M);
        myModel.Draw(litModel);

        setLights(litCubes, view, proj);
        litCubes.setFloat("cubeScale", cubeScale);
        litCubes.setInt("texture_diffuse1", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTex);
        glBindVertexArray(VAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, NUM_CUBES);
        glBindVertexArray(0);

        floorShader.use();
        floorShader.setMat4("view", view);
        floorShader.setMat4("projection", proj);
        floorShader.setInt("floorTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTex);
        glBindVertexArray(floorVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        emissive.use();
        emissive.setMat4("view", view);
        emissive.setMat4("projection", proj);
        for (int i=0;i<2;++i){
            glm::mat4 Lm(1.0f);
            Lm = glm::translate(Lm, pointPos[i]);
            Lm = glm::scale(Lm, glm::vec3(lightMarkerScale));
            emissive.setMat4("model", Lm);
            emissive.setVec3("emissiveColor", pointCol[i]);
            lightSphere.Draw(emissive);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);
    glDeleteBuffers(1,&instanceVBO);
    glfwTerminate();
    return 0;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height){ glViewport(0,0,width,height); }

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (!mouseCaptured) return;
    if (firstMouse) { lastX = xpos; lastY = ypos; firstMouse = false; }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos; lastY = ypos;
    float sensitivity = 0.1f;
    xoffset *= sensitivity; yoffset *= sensitivity;
    yaw += xoffset; pitch += yoffset;
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    glm::vec3 front;
    front.x = cos(glm::radians(yaw))*cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw))*cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window,true);
    if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT)==GLFW_PRESS && !mouseCaptured){
        mouseCaptured=true; firstMouse=true;
        glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
    } else if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT)==GLFW_RELEASE && mouseCaptured){
        mouseCaptured=false; glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
    }
    float speed = 4.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraPos += speed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraPos -= speed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraPos -= glm::normalize(glm::cross(cameraFront,cameraUp))*speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraPos += glm::normalize(glm::cross(cameraFront,cameraUp))*speed;
    if (glfwGetKey(window, GLFW_KEY_SPACE)==GLFW_PRESS) cameraPos += glm::vec3(0.0f,speed,0.0f);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)==GLFW_PRESS) cameraPos -= glm::vec3(0.0f,speed,0.0f);
}
