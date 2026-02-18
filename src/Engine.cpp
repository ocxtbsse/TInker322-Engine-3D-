#define STB_IMAGE_IMPLEMENTATION
#include "Engine.h"
#include "Window.hpp"
#include "ShaderProgram.hpp"
#include <STB/stb_image.h>
#include "OBS.hpp"
#include "Scene.hpp"
#include "DeltaTime.hpp"

#include "modelImport.hpp"

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <math.h>

// fixed bug
const char* vSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    layout (location = 2) in vec2 aTexCoord;


    out vec2 TexCoord;
    out vec3 Normal;
    out vec3 FragPos;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main() {
        FragPos = vec3(model * vec4(aPos, 1.0));


        Normal = mat3(transpose(inverse(model))) * aNormal;
        

        gl_Position = projection * view * model * vec4(aPos, 1.0);
        TexCoord = aTexCoord;
    }
)";

const char* fSource = R"(
    #version 330 core
    out vec4 FragColor;
    
    in vec2 TexCoord;
    in vec3 Normal;
    in vec3 FragPos;
    
    uniform sampler2D ourTexture;
    
    uniform vec3 lightPos;
    uniform vec3 lightColor;
    uniform vec3 viewPos;
    
    void main() {
        float ambientStrength = 0.5;
        vec3 ambient = ambientStrength * lightColor;
              
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
            
        vec3 result = (ambient + diffuse) * texture(ourTexture, TexCoord).rgb;
        
        FragColor = vec4(result, 1.0);
    }

)";






Window w1;
ShaderProgram prog;















int meshLoadUlta(GameObject& targetObj,char* modelName){
    ModelLoader modelLoad;
    std::cout << "[DEBUG] Loading model: " << modelName << std::endl;
    if(!modelLoad.loadModel(modelName)){
        std::cout << "[ERROR] Failed to load: " << modelName << std::endl;
        return 1;
    }

    
    std::cout << "[SUCCESS] Vertices: " << modelLoad.vertices.size() 
              << " | Indices: " << modelLoad.indices.size() << std::endl;

    unsigned int VBO3, VAO3, EBO3;
    glGenVertexArrays(1, &VAO3);
    glGenBuffers(1, &VBO3);
    glGenBuffers(1, &EBO3);

    glBindVertexArray(VAO3);
    glBindBuffer(GL_ARRAY_BUFFER, VBO3);
    
    glBufferData(GL_ARRAY_BUFFER, modelLoad.vertices.size() * sizeof(Vertex), modelLoad.vertices.data(), GL_STATIC_DRAW);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO3);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, modelLoad.indices.size() * sizeof(unsigned int), modelLoad.indices.data(), GL_STATIC_DRAW);


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);


    
    targetObj.mesh.VAOTM = VAO3;
    targetObj.mesh.indicesSize = static_cast<unsigned int>(modelLoad.indices.size());
    targetObj.mesh.modelName = modelName;
    std::cout << "[DEBUG] VAO generated: " << VAO3 << " | indicesSize: " << targetObj.mesh.indicesSize << std::endl;
    
    return 0;
}


unsigned int textureLoad(const char* path){
    unsigned int tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);
    return tex;
}







int init(){
    if (w1.createWindow(800, 600, "My Engine") != 0) return -1;
    
    glewInit();
    glEnable(GL_DEPTH_TEST);

    if (prog.compileShader(vSource, fSource) != 0) return -2;



    glfwSetInputMode(w1.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);



    return 0;
}


int render(){
    glClearColor(0.1f, 0.1f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    prog.use();

    glUniform1i(glGetUniformLocation(prog.ID, "ourTexture"), 0); 
    glm::vec3 lightPos(0.0f, 100.0f, 0.0f); 
    prog.setVec3("lightPos", lightPos);
    prog.setVec3("lightColor", glm::vec3(0.8f, 0.8f, 1.2f));





    for(auto& cams : sceneCamers){
        prog.setMat4x4("view",cams.getCameraViewMat4x4());
        prog.setMat4x4("projection",cams.getProectionMat4x4());
    }






    
    
    
    for(auto& obj : sceneObjects){
        if(obj.visible){
            glBindTexture(GL_TEXTURE_2D, obj.material.textureID);
            glBindVertexArray(obj.mesh.VAOTM);
            prog.setMat4x4("model", obj.getModelMatrix4x4());
            glDrawElements(GL_TRIANGLES, obj.mesh.indicesSize, GL_UNSIGNED_INT, 0);
        }
    }
    
    
    




    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);

    glfwSwapBuffers(w1.window);
    glfwPollEvents();
    return 0;
}

float speed = 20.0f;

double mouseX, mouseY;
static double lastX = 800 / 2, lastY = 600 / 2;

float yaw   = -90.0f; 
float pitch = 0.0f;











float velY = 0.0f;
float gravity = 20.0f;

int update(){
    Time::update();
    if (Time::deltaTime > 0.1f) Time::deltaTime = 0.1f;
    Camera& cam = sceneCamers[0];


    glfwGetCursorPos(w1.window, &mouseX, &mouseY);
    float xoffset = (float)(mouseX - lastX);
    float yoffset = (float)(lastY - mouseY);
    lastX = mouseX;
    lastY = mouseY;

    float sensitivity = 10.0f; 
    yaw   += xoffset * sensitivity * Time::deltaTime;
    pitch += yoffset * sensitivity * Time::deltaTime;


    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;



    VectorS direction;
    float cosPitch = cos(pitch * DEGRAD);



    

    direction.vec3[X] = cos(yaw * DEGRAD) * cosPitch;
    direction.vec3[Y] = sin(pitch * DEGRAD);

    direction.vec3[Z] = sin(yaw * DEGRAD) * cosPitch;

    cam.front.vec3[X] = direction.vec3[0];
    cam.front.vec3[Y] = direction.vec3[1];
    cam.front.vec3[Z] = direction.vec3[2];
    cam.front.normalize();

    VectorS forward;
    forward.vec3[X] = cam.front.vec3[0];
    forward.vec3[Y] = 0.0f;
    forward.vec3[Z] = cam.front.vec3[2];
    forward.normalize();



    VectorS right;
    right = forward.cross(cam.up);
    right.normalize();


    float cameraSpeed = speed * Time::deltaTime;

    if(glfwGetKey(w1.window, GLFW_KEY_W) == GLFW_PRESS){
        cam.position.vec3[X] += forward.vec3[X] * cameraSpeed;
        cam.position.vec3[Y] += forward.vec3[Y] * cameraSpeed;
        cam.position.vec3[Z] += forward.vec3[Z] * cameraSpeed;

    }
    if(glfwGetKey(w1.window, GLFW_KEY_S) == GLFW_PRESS){
        cam.position.vec3[X] -= forward.vec3[X] * cameraSpeed;
        cam.position.vec3[Y] -= forward.vec3[Y] * cameraSpeed;
        cam.position.vec3[Z] -= forward.vec3[Z] * cameraSpeed;
    }

     if(glfwGetKey(w1.window, GLFW_KEY_D) == GLFW_PRESS){
        cam.position.vec3[X] += right.vec3[X] * cameraSpeed;
        cam.position.vec3[Y] += right.vec3[Y] * cameraSpeed;
        cam.position.vec3[Z] += right.vec3[Z] * cameraSpeed;
    }
    if(glfwGetKey(w1.window, GLFW_KEY_A) == GLFW_PRESS){
        cam.position.vec3[X] -= right.vec3[X] * cameraSpeed;
        cam.position.vec3[Y] -= right.vec3[Y] * cameraSpeed;
        cam.position.vec3[Z] -= right.vec3[Z] * cameraSpeed;
    }

    return 0;
}
