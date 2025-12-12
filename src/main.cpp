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
#include <algorithm>

#include <../include/model.h>
#include <../include/shader.h>
#include <../include/stb_image.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);

const unsigned int NUM_CUBES  = 1000;
const unsigned int SCR_WIDTH  = 1920;
const unsigned int SCR_HEIGHT = 1080;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 cameraPos   = glm::vec3(0.0f, 3.0f, 6.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, -0.3f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw   = -90.0f;
float pitch =  0.0f;
float lastX = SCR_WIDTH  / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse    = true;
bool mouseCaptured = false;
float outlineWidth = 0.005f;
bool enableOutline = true;
glm::vec3 outlineColor = glm::vec3(0.0f, 0.0f, 0.0f);
bool enableDirLight    = true;
bool enablePointLights = true;
bool enableSpotLight   = false;
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
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        std::cerr << "Failed to load texture: " << path << std::endl;
    }
    stbi_image_free(data);
    return textureID;
}

struct TransparentMesh {
    glm::vec3 position;
    glm::vec3 scale;
};

int main() {
    srand((unsigned)time(nullptr));

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    bool useCellShading = true;
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH,SCR_HEIGHT,"HW4 Shadows + Transparency",nullptr,nullptr);
    if(!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glFrontFace(GL_CCW);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

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
    Shader shadowGeneric(
        (std::string(PROJECT_ROOT) + "shaders/shadow_depth_generic_vs.glsl").c_str(),
        (std::string(PROJECT_ROOT) + "shaders/shadow_depth_fs.glsl").c_str()
    );
    Shader shadowCubes(
        (std::string(PROJECT_ROOT) + "shaders/shadow_depth_cubes_vs.glsl").c_str(),
        (std::string(PROJECT_ROOT) + "shaders/shadow_depth_fs.glsl").c_str()
    );
    Shader transparentShader(
        (std::string(PROJECT_ROOT) + "shaders/transparent_vertex.glsl").c_str(),
        (std::string(PROJECT_ROOT) + "shaders/transparent_fragment.glsl").c_str()
    );
    Shader outlineModelShader(
    (std::string(PROJECT_ROOT) + "shaders/outline_model_vert.glsl").c_str(),
    (std::string(PROJECT_ROOT) + "shaders/outline_frag.glsl").c_str()
);
    Shader outlineCubeShader(
        (std::string(PROJECT_ROOT) + "shaders/outline_cube_vert.glsl").c_str(),
        (std::string(PROJECT_ROOT) + "shaders/outline_frag.glsl").c_str()
    );

    Model myModel(std::string(PROJECT_ROOT) + "assets/lpshead/head.OBJ");
    Model lightSphere(std::string(PROJECT_ROOT) + "assets/sphere.obj");

    unsigned int cubeVAO, cubeVBO, instanceVBO;
    glGenVertexArrays(1,&cubeVAO);
    glGenBuffers(1,&cubeVBO);
    glGenBuffers(1,&instanceVBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    std::vector<glm::vec3> cubePositions;
    cubePositions.reserve(NUM_CUBES);
    float spread = std::sqrt((float)NUM_CUBES) / 20.0f;
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
    unsigned int floorTex = loadTexture((std::string(PROJECT_ROOT) + "assets/floor.jpg").c_str());

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

    float transparentVertices[] = {
        -0.5f, 0.0f, 0.0f,  0.0f,0.0f,
         0.5f, 0.0f, 0.0f,  1.0f,0.0f,
         0.5f, 1.0f, 0.0f,  1.0f,1.0f,
         0.5f, 1.0f, 0.0f,  1.0f,1.0f,
        -0.5f, 1.0f, 0.0f,  0.0f,1.0f,
        -0.5f, 0.0f, 0.0f,  0.0f,0.0f
    };
    unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1,&transparentVAO);
    glGenBuffers(1,&transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    std::vector<TransparentMesh> transparentMeshes;
    transparentMeshes.push_back({ glm::vec3(-1.5f, 0.0f,  2.0f), glm::vec3(2.0f, 2.5f, 1.0f) });
    transparentMeshes.push_back({ glm::vec3( 2.5f, 0.0f, -1.0f), glm::vec3(2.0f, 2.5f, 1.0f) });
    transparentMeshes.push_back({ glm::vec3( 0.0f, 0.0f,  3.5f), glm::vec3(3.0f, 2.0f, 1.0f) });

    glm::vec3 dirLightDir = glm::normalize(glm::vec3(-0.2f,-1.0f,-0.3f));
    glm::vec3 dirLightCol = glm::vec3(1.0f,0.98f,0.9f);

    float dirLightYaw   = -60.0f;
    float dirLightPitch = -45.0f;
    glm::vec3 pointPos[2] = {
        glm::vec3(-2.0f, 1.2f,  2.0f),
        glm::vec3( 3.0f, 0.8f, -3.0f)
    };
    glm::vec3 pointCol[2] = {
        glm::vec3(1.0f,0.25f,0.25f),
        glm::vec3(0.2f,0.7f,1.0f)
    };
    float attKc=1.0f, attKl=0.09f, attKq=0.032f;
    float cubeScale = 0.25f;
    const float lightMarkerScale = 0.02f;

    const unsigned int SHADOW_WIDTH = 2048;
    const unsigned int SHADOW_HEIGHT = 2048;
    unsigned int depthMapFBO, depthMap;
    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24,
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0f,1.0f,1.0f,1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    bool usePCF = true;
    bool useComparisonSampler = true;
    float shadowBias = 0.0005f;
    float dirLightOrthoSize = 25.0f;
    float nearPlane = 1.0f;
    float farPlane  = 60.0f;

    float transparentAlpha = 0.4f;
    glm::vec3 transparentTint(0.3f, 0.8f, 1.0f);

    auto setLights = [&](Shader& sh, const glm::mat4& view,
                         const glm::mat4& proj,
                         const glm::mat4& lightSpaceMatrix,
                         float materialAlpha)
    {
        sh.use();
        sh.setBool("cellShading", useCellShading);
        sh.setMat4("view", view);
        sh.setMat4("projection", proj);
        sh.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        sh.setVec3("viewPos", cameraPos);

        sh.setInt("blinn", 1);
        sh.setFloat("shininess", 32.0f);
        sh.setVec3("specularColor", glm::vec3(0.4f));

        sh.setVec3("dirLight.direction", dirLightDir);
        if (enableDirLight)
            sh.setVec3("dirLight.color", dirLightCol);
        else
            sh.setVec3("dirLight.color", glm::vec3(0.0f));

        for (int i=0;i<2;++i){
            std::string base = "pointLights[" + std::to_string(i) + "]";
            sh.setVec3(base + ".position",  pointPos[i]);
            if (enablePointLights)
                sh.setVec3(base + ".color", pointCol[i]);
            else
                sh.setVec3(base + ".color", glm::vec3(0.0f));

            sh.setFloat(base + ".constant", attKc);
            sh.setFloat(base + ".linear",   attKl);
            sh.setFloat(base + ".quadratic",attKq);
        }

        sh.setVec3("spotLight.position",  cameraPos);
        sh.setVec3("spotLight.direction", cameraFront);
        if (enableSpotLight)
            sh.setVec3("spotLight.color", glm::vec3(1.0f));
        else
            sh.setVec3("spotLight.color", glm::vec3(0.0f));

        sh.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        sh.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
        sh.setFloat("spotLight.constant", attKc);
        sh.setFloat("spotLight.linear",   attKl);
        sh.setFloat("spotLight.quadratic",attKq);

        sh.setInt("shadowMap", 3);
        sh.setInt("shadowMapCmp", 3);
        sh.setBool("usePCF", usePCF);
        sh.setBool("useComparisonSampler", useComparisonSampler);
        sh.setFloat("shadowBias", shadowBias);
        sh.setFloat("materialAlpha", materialAlpha);
    };

    while(!glfwWindowShouldClose(window)){
        float t = glfwGetTime();
        deltaTime = t - lastFrame;
        lastFrame = t;

        processInput(window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 proj = glm::perspective(glm::radians(45.0f),
                                          (float)SCR_WIDTH/(float)SCR_HEIGHT,
                                          0.1f, 100.0f);

        glm::vec3 dl;
        dl.x = cos(glm::radians(dirLightYaw)) * cos(glm::radians(dirLightPitch));
        dl.y = sin(glm::radians(dirLightPitch));
        dl.z = sin(glm::radians(dirLightYaw)) * cos(glm::radians(dirLightPitch));
        dirLightDir = glm::normalize(dl);
        glm::vec3 lightPos = -dirLightDir * 25.0f;
        glm::mat4 lightProj = glm::ortho(-dirLightOrthoSize, dirLightOrthoSize,
                                         -dirLightOrthoSize, dirLightOrthoSize,
                                         nearPlane, farPlane);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f,1.0f,0.0f));
        glm::mat4 lightSpaceMatrix = lightProj * lightView;

        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
                        useComparisonSampler ? GL_COMPARE_REF_TO_TEXTURE : GL_NONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        useComparisonSampler ? GL_LINEAR : GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                        useComparisonSampler ? GL_LINEAR : GL_NEAREST);

        glViewport(0,0,SHADOW_WIDTH,SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(2.0f, 4.0f);

        shadowGeneric.use();
        shadowGeneric.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        {
            glm::mat4 M(1.0f);
            M = glm::scale(M, glm::vec3(7.0f));
            M = glm::translate(M, glm::vec3(0.0f,0.4f,0.0f));
            //M = glm::rotate(M, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
            shadowGeneric.setMat4("model", M);
            myModel.Draw(shadowGeneric);
        }

        {
            glm::mat4 M(1.0f);
            shadowGeneric.setMat4("model", M);
            glBindVertexArray(floorVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }

        shadowCubes.use();
        shadowCubes.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        shadowCubes.setFloat("cubeScale", cubeScale);
        glBindVertexArray(cubeVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, NUM_CUBES);
        glBindVertexArray(0);

        glDisable(GL_POLYGON_OFFSET_FILL);
        glDisable(GL_CULL_FACE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0,0,SCR_WIDTH,SCR_HEIGHT);
        glClearColor(0.1f,0.15f,0.2f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glDepthMask(GL_TRUE);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        setLights(litModel, view, proj, lightSpaceMatrix, 1.0f);
        {
            glm::mat4 M(1.0f);
            M = glm::scale(M, glm::vec3(7.0f));
            M = glm::translate(M, glm::vec3(0.0f,0.4f,0.0f));
            //M = glm::rotate(M, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
            litModel.setMat4("model", M);
            myModel.Draw(litModel);
        }

        setLights(litCubes, view, proj, lightSpaceMatrix, 1.0f);
        litCubes.setFloat("cubeScale", cubeScale);
        litCubes.setInt("texture_diffuse1", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTex);
        glBindVertexArray(cubeVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, NUM_CUBES);
        glBindVertexArray(0);
        if (enableOutline) {
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00); // Disable writing to stencil
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            outlineModelShader.use();
            outlineModelShader.setMat4("view", view);
            outlineModelShader.setMat4("projection", proj);
            outlineModelShader.setFloat("outlineWidth", outlineWidth);
            outlineModelShader.setVec3("outlineColor", outlineColor);

            {
                glm::mat4 M(1.0f);
                M = glm::scale(M, glm::vec3(7.0f));
                M = glm::translate(M, glm::vec3(0.0f, 0.4f, 0.0f));
                outlineModelShader.setMat4("model", M);
                myModel.Draw(outlineModelShader);
            }
            glCullFace(GL_BACK);
            outlineCubeShader.use();
            outlineCubeShader.setMat4("view", view);
            outlineCubeShader.setMat4("projection", proj);
            outlineCubeShader.setFloat("outlineWidth", outlineWidth * 20.0f);
            outlineCubeShader.setFloat("cubeScale", cubeScale);
            outlineCubeShader.setVec3("outlineColor", outlineColor);

            glBindVertexArray(cubeVAO);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 36, NUM_CUBES);
            glBindVertexArray(0);

            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
        }
        glDisable(GL_CULL_FACE);
        setLights(floorShader, view, proj, lightSpaceMatrix, 1.0f);
        floorShader.setInt("floorTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTex);
        {
            glm::mat4 M(1.0f);
            floorShader.setMat4("model", M);
            glBindVertexArray(floorVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }

        emissive.use();
        emissive.setMat4("view", view);
        emissive.setMat4("projection", proj);
        if (enablePointLights) {
            for (int i=0;i<2;++i){
                glm::mat4 Lm(1.0f);
                Lm = glm::translate(Lm, pointPos[i]);
                Lm = glm::scale(Lm, glm::vec3(lightMarkerScale));
                emissive.setMat4("model", Lm);
                emissive.setVec3("emissiveColor", pointCol[i]);
                lightSphere.Draw(emissive);
            }
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);

        setLights(transparentShader, view, proj, lightSpaceMatrix, transparentAlpha);
        transparentShader.setVec3("tintColor", transparentTint);

        std::vector<std::pair<float, TransparentMesh>> sorted;
        sorted.reserve(transparentMeshes.size());
        for (auto& tm : transparentMeshes) {
            float dist2 = glm::length(cameraPos - tm.position);
            sorted.emplace_back(dist2, tm);
        }
        std::sort(sorted.begin(), sorted.end(),
                  [](const auto& a, const auto& b){ return a.first > b.first; });

        glBindVertexArray(transparentVAO);
        for (const auto& pair : sorted) {
            const TransparentMesh& tm = pair.second;
            glm::mat4 M(1.0f);
            M = glm::translate(M, tm.position);
            M = glm::scale(M, tm.scale);
            transparentShader.setMat4("model", M);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glBindVertexArray(0);

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);

        ImGui::Begin("Shadow Settings");
        ImGui::Checkbox("Enable Cell Shading", &useCellShading);
        ImGui::Checkbox("Enable Outline", &enableOutline);
        ImGui::SliderFloat("Outline Width", &outlineWidth, 0.001f, 0.1f);
        ImGui::ColorEdit3("Outline Color", (float*)&outlineColor);
        ImGui::Text("Light Controls:");
        ImGui::Checkbox("Dir Light (Sun)", &enableDirLight);
        ImGui::Checkbox("Point Lights", &enablePointLights);
        ImGui::Checkbox("Spot Light (Cam)", &enableSpotLight);
        ImGui::Separator();
        ImGui::SliderFloat("Bias", &shadowBias, 0.00001f, 0.005f, "%.5f");
        ImGui::Checkbox("Use PCF (SW)", &usePCF);
        ImGui::Checkbox("Use Comparison Sampler", &useComparisonSampler);
        ImGui::SliderFloat("DirLight Ortho Size", &dirLightOrthoSize, 5.0f, 80.0f);
        ImGui::SliderFloat("Transparent Alpha", &transparentAlpha, 0.05f, 0.9f);
        ImGui::SliderFloat("DirLight Yaw",   &dirLightYaw,   -180.0f, 180.0f);
        ImGui::SliderFloat("DirLight Pitch", &dirLightPitch, -89.0f,  -5.0f);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1,&cubeVAO);
    glDeleteBuffers(1,&cubeVBO);
    glDeleteBuffers(1,&instanceVBO);
    glDeleteVertexArrays(1,&floorVAO);
    glDeleteBuffers(1,&floorVBO);
    glDeleteVertexArrays(1,&transparentVAO);
    glDeleteBuffers(1,&transparentVBO);
    glDeleteFramebuffers(1, &depthMapFBO);
    glDeleteTextures(1, &depthMap);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0,0,width,height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (!mouseCaptured) return;
    if (firstMouse) {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }
    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;
    lastX = (float)xpos;
    lastY = (float)ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw))*cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw))*cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window,true);

    if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT)==GLFW_PRESS && !mouseCaptured){
        mouseCaptured=true;
        firstMouse=true;
        glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
    } else if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT)==GLFW_RELEASE && mouseCaptured){
        mouseCaptured=false;
        glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        enableDirLight = true;
        enablePointLights = true;
        enableSpotLight = true;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        enableDirLight = true;
        enablePointLights = true;
        enableSpotLight = false;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        enableDirLight = true;
        enablePointLights = false;
        enableSpotLight = false;
    }

    float speed = 4.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += speed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= speed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront,cameraUp))*speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront,cameraUp))*speed;
    if (glfwGetKey(window, GLFW_KEY_SPACE)==GLFW_PRESS)
        cameraPos += glm::vec3(0.0f,speed,0.0f);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)==GLFW_PRESS)
        cameraPos -= glm::vec3(0.0f,speed,0.0f);
}