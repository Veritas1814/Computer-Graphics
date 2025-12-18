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
#include <cmath>

#include <../include/model.h>
#include <../include/shader.h>
#include <../include/stb_image.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

enum ShadingMode {
    MODE_PBR = 0,
    MODE_BLINN = 1,
    MODE_EMISSIVE = 2
};

// Function Prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
void renderQuad();
void renderPBRSphere();

// Constants
const unsigned int NUM_CUBES  = 1000;
const unsigned int SCR_WIDTH  = 1920;
const unsigned int SCR_HEIGHT = 1080;

// Camera & Input Globals
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

// Settings
float outlineWidth = 0.005f;
bool enableOutline = true;
glm::vec3 outlineColor = glm::vec3(0.0f, 0.0f, 0.0f);
bool enableDirLight    = true;
bool enablePointLights = true;
bool enableSpotLight   = false;

// HDR Settings
bool hdr = true;
float exposure = 1.0f;

// PBR Settings
int currentShadingMode = MODE_PBR;
bool useNormalMap = true;
glm::vec3 pbrSpherePos(-3.0f, 1.0f, 0.0f);

// Cube Data
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

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH,SCR_HEIGHT,"HW5 PBR + HDR",nullptr,nullptr);
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

    // SHADERS
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
    Shader hdrShader(
        (std::string(PROJECT_ROOT) + "shaders/hdr_vertex.glsl").c_str(),
        (std::string(PROJECT_ROOT) + "shaders/hdr_fragment.glsl").c_str()
    );
    Shader pbrShader(
        (std::string(PROJECT_ROOT) + "shaders/pbr_vertex.glsl").c_str(),
        (std::string(PROJECT_ROOT) + "shaders/pbr_fragment.glsl").c_str()
    );

    // MODELS
    Model myModel(std::string(PROJECT_ROOT) + "assets/lpshead/head.OBJ");
    Model lightSphere(std::string(PROJECT_ROOT) + "assets/sphere.obj");

    // PBR TEXTURES
    unsigned int pbrAlbedo    = loadTexture((std::string(PROJECT_ROOT) + "assets/MetalCorrodedHeavy001/MetalCorrodedHeavy001_COL_2K_METALNESS.jpg").c_str());
    unsigned int pbrNormal    = loadTexture((std::string(PROJECT_ROOT) + "assets/MetalCorrodedHeavy001/MetalCorrodedHeavy001_NRM_2K_METALNESS.jpg").c_str());
    unsigned int pbrMetallic  = loadTexture((std::string(PROJECT_ROOT) + "assets/MetalCorrodedHeavy001/MetalCorrodedHeavy001_METALNESS_2K_METALNESS.jpg").c_str());
    unsigned int pbrRoughness = loadTexture((std::string(PROJECT_ROOT) + "assets/MetalCorrodedHeavy001/MetalCorrodedHeavy001_ROUGHNESS_2K_METALNESS.jpg").c_str());

    pbrShader.use();
    pbrShader.setInt("albedoMap", 10);
    pbrShader.setInt("normalMap", 11);
    pbrShader.setInt("metallicMap", 12);
    pbrShader.setInt("roughnessMap", 13);

    // BUFFERS (Cubes, Floor, Transparent)
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

    // Transparent Setup
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

    // Light Setup
    glm::vec3 dirLightDir = glm::normalize(glm::vec3(-0.2f,-1.0f,-0.3f));
    glm::vec3 dirLightCol = glm::vec3(1.0f,0.98f,0.9f);

    float dirLightYaw   = -24.0f;
    float dirLightPitch = -30.0f;
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

    // SHADOW MAP FBO
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

    // HDR FBO SETUP
    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    unsigned int colorBuffer;
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);

    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "HDR Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Settings
    bool useCellShading = true;
    bool usePCF = true;
    bool useComparisonSampler = true;
    float shadowBias = 0.0005f;
    float dirLightOrthoSize = 25.0f;
    float nearPlane = 1.0f;
    float farPlane  = 60.0f;
    float transparentAlpha = 0.4f;
    glm::vec3 transparentTint(0.3f, 0.8f, 1.0f);

    hdrShader.use();
    hdrShader.setInt("hdrBuffer", 0);

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
        sh.setVec3("dirLight.color", enableDirLight ? dirLightCol : glm::vec3(0.0f));

        for (int i=0;i<2;++i){
            std::string base = "pointLights[" + std::to_string(i) + "]";
            sh.setVec3(base + ".position",  pointPos[i]);
            sh.setVec3(base + ".color", enablePointLights ? pointCol[i] : glm::vec3(0.0f));
            sh.setFloat(base + ".constant", attKc);
            sh.setFloat(base + ".linear",   attKl);
            sh.setFloat(base + ".quadratic",attKq);
        }

        sh.setVec3("spotLight.position",  cameraPos);
        sh.setVec3("spotLight.direction", cameraFront);
        sh.setVec3("spotLight.color", enableSpotLight ? glm::vec3(1.0f) : glm::vec3(0.0f));
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

        // 1. SHADOW PASS
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

        // Shadow for PBR Sphere
        {
            glm::mat4 modelPBR = glm::mat4(1.0f);
            modelPBR = glm::translate(modelPBR, pbrSpherePos);
            modelPBR = glm::scale(modelPBR, glm::vec3(0.5f));
            shadowGeneric.setMat4("model", modelPBR);
            renderPBRSphere();
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

        // 2. HDR LIGHTING PASS
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glViewport(0,0,SCR_WIDTH,SCR_HEIGHT);
        glClearColor(0.1f,0.15f,0.2f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glDepthMask(GL_TRUE);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);

        // Draw Main Model
        setLights(litModel, view, proj, lightSpaceMatrix, 1.0f);
        {
            glm::mat4 M(1.0f);
            M = glm::scale(M, glm::vec3(7.0f));
            M = glm::translate(M, glm::vec3(0.0f,0.4f,0.0f));
            litModel.setMat4("model", M);
            myModel.Draw(litModel);
        }

        // Draw Cubes
        setLights(litCubes, view, proj, lightSpaceMatrix, 1.0f);
        litCubes.setFloat("cubeScale", cubeScale);
        litCubes.setInt("texture_diffuse1", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTex);
        glBindVertexArray(cubeVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, NUM_CUBES);
        glBindVertexArray(0);

        // Setup Geometry Matrix
        glm::mat4 modelPBR = glm::mat4(1.0f);
        modelPBR = glm::translate(modelPBR, pbrSpherePos);
        modelPBR = glm::scale(modelPBR, glm::vec3(0.5f));

        if (currentShadingMode == MODE_PBR)
        {
            pbrShader.use();
            pbrShader.setMat4("projection", proj);
            pbrShader.setMat4("view", view);
            pbrShader.setMat4("model", modelPBR);
            pbrShader.setVec3("viewPos", cameraPos);
            pbrShader.setBool("useNormalMap", useNormalMap);

            pbrShader.setVec3("dirLight.direction", dirLightDir);
            pbrShader.setVec3("dirLight.color", enableDirLight ? dirLightCol * 2.0f : glm::vec3(0.0f));

            for (int i=0; i<2; ++i){
                std::string base = "pointLights[" + std::to_string(i) + "]";
                pbrShader.setVec3(base + ".position",  pointPos[i]);
                pbrShader.setVec3(base + ".color", enablePointLights ? pointCol[i] * 5.0f : glm::vec3(0.0f));
                pbrShader.setFloat(base + ".constant", attKc);
                pbrShader.setFloat(base + ".linear",   attKl);
                pbrShader.setFloat(base + ".quadratic",attKq);
            }

            pbrShader.setVec3("spotLight.position",  cameraPos);
            pbrShader.setVec3("spotLight.direction", cameraFront);
            pbrShader.setVec3("spotLight.color", enableSpotLight ? glm::vec3(5.0f) : glm::vec3(0.0f));
            pbrShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
            pbrShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
            pbrShader.setFloat("spotLight.constant", 1.0f);
            pbrShader.setFloat("spotLight.linear",   0.09f);
            pbrShader.setFloat("spotLight.quadratic",0.032f);

            // Bind PBR Textures
            glActiveTexture(GL_TEXTURE10); glBindTexture(GL_TEXTURE_2D, pbrAlbedo);
            glActiveTexture(GL_TEXTURE11); glBindTexture(GL_TEXTURE_2D, pbrNormal);
            glActiveTexture(GL_TEXTURE12); glBindTexture(GL_TEXTURE_2D, pbrMetallic);
            glActiveTexture(GL_TEXTURE13); glBindTexture(GL_TEXTURE_2D, pbrRoughness);

            renderPBRSphere();

        }
        else if (currentShadingMode == MODE_BLINN)
        {
            setLights(litModel, view, proj, lightSpaceMatrix, 1.0f);
            litModel.setMat4("model", modelPBR);

            // Use Albedo as Diffuse for Blinn
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, pbrAlbedo);
            litModel.setInt("texture_diffuse1", 0);

            renderPBRSphere();
        }
        else if (currentShadingMode == MODE_EMISSIVE)
        {
            emissive.use();
            emissive.setMat4("view", view);
            emissive.setMat4("projection", proj);
            emissive.setMat4("model", modelPBR);

            emissive.setVec3("emissiveColor", glm::vec3(1.0f, 1.0f, 1.0f)); // White Tint
            emissive.setInt("texture_diffuse1", 0);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, pbrAlbedo); // Bind the Rusty Texture

            renderPBRSphere();
        }
        // Draw Outlines
        if (enableOutline) {
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00);
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

        // Draw Floor
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

        // Draw Emissive Lights
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

        // Draw Transparent
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

        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Back to default framebuffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        hdrShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffer);
        hdrShader.setBool("hdr", hdr);
        hdrShader.setFloat("exposure", exposure);
        renderQuad();

        // IMGUI
        ImGui::Begin("Settings");

        ImGui::Text("Shading Mode (For Sphere)");
        if(ImGui::RadioButton("PBR (Physically Based)", currentShadingMode == MODE_PBR)) currentShadingMode = MODE_PBR;
        if(ImGui::RadioButton("Blinn-Phong", currentShadingMode == MODE_BLINN)) currentShadingMode = MODE_BLINN;
        if(ImGui::RadioButton("Emissive (Debug)", currentShadingMode == MODE_EMISSIVE)) currentShadingMode = MODE_EMISSIVE;
        ImGui::Separator();

        ImGui::Text("HDR Controls");
        ImGui::Checkbox("Enable HDR Tone Mapping", &hdr);
        ImGui::SliderFloat("Exposure", &exposure, 0.1f, 5.0f);
        ImGui::Separator();

        ImGui::Text("PBR Object");
        ImGui::DragFloat3("PBR Pos", (float*)&pbrSpherePos, 0.1f);
        if(currentShadingMode == MODE_PBR) {
            ImGui::Checkbox("Use Normal Map", &useNormalMap);
        }
        ImGui::Separator();

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
    glDeleteFramebuffers(1, &hdrFBO);
    glDeleteTextures(1, &colorBuffer);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    (void)window;
    glViewport(0,0,width,height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    (void)window;
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

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

unsigned int pbrSphereVAO = 0;
unsigned int pbrIndexCount = 0;
void renderPBRSphere()
{
    if (pbrSphereVAO == 0)
    {
        glGenVertexArrays(1, &pbrSphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359f;

        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow)
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        pbrIndexCount = static_cast<unsigned int>(indices.size());

        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }

            // Fixes the "Flat Normal Map" issue
            float theta = uv[i].x * 2.0f * PI;
            glm::vec3 T = glm::vec3(-std::sin(theta), 0.0f, std::cos(theta));
            T = glm::normalize(T - normals[i] * glm::dot(normals[i], T));

            data.push_back(T.x); data.push_back(T.y); data.push_back(T.z);

            glm::vec3 B = glm::cross(normals[i], T);
            data.push_back(B.x); data.push_back(B.y); data.push_back(B.z);
        }

        glBindVertexArray(pbrSphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        GLsizei stride = 14 * sizeof(float);
        glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1); glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2); glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3); glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(4); glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)(11 * sizeof(float)));
    }

    glBindVertexArray(pbrSphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, pbrIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}