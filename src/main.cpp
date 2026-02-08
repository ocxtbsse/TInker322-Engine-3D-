#define STB_IMAGE_IMPLEMENTATION
#include <STB/stb_image.h>

#include "Window.hpp"
#include "ShaderProgram.hpp"
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include "DeltaTime.hpp"
#include <vector>


const char* vSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 aTexCoord;
    layout (location = 2) in vec3 aNormal;


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
        float ambientStrength = 0.2;
        vec3 ambient = ambientStrength * lightColor;
              
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
            
        vec3 result = (ambient + diffuse) * texture(ourTexture, TexCoord).rgb;
        
        FragColor = vec4(result, 1.0);
    }

)";







float verticesCubeSigma[] = {

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f
};





unsigned int indices[] = {
    0, 1, 2, 2, 3, 0,    
    4, 5, 6, 6, 7, 4,      
    8, 9, 10, 10, 11, 8,   
    12, 13, 14, 14, 15, 12, 
    16, 17, 18, 18, 19, 16, 
    20, 21, 22, 22, 23, 20  
};


struct BoxCollider {
    glm::vec3* pos;
    glm::vec3 size;
    bool isStatic;
};



void resolveCollision(const BoxCollider& wall, BoxCollider& player, bool& grounded) {
    glm::vec3 delta = *player.pos - *wall.pos;

    glm::vec3 halfSizeWall = wall.size * 0.5f;
    glm::vec3 halfSizePlayer = player.size * 0.5f;

    float overlapX = (halfSizeWall.x + halfSizePlayer.x) - glm::abs(delta.x);
    float overlapY = (halfSizeWall.y + halfSizePlayer.y) - glm::abs(delta.y);
    float overlapZ = (halfSizeWall.z + halfSizePlayer.z) - glm::abs(delta.z);

    if (overlapX > 0 && overlapY > 0 && overlapZ > 0) {
        
        if (overlapX < overlapY && overlapX < overlapZ) {
            player.pos->x += (delta.x > 0) ? overlapX : -overlapX;
        } 
        else if (overlapY < overlapX && overlapY < overlapZ) {
            player.pos->y += (delta.y > 0) ? overlapY : -overlapY;
            if (delta.y > 0) {
                player.pos->y += overlapY;                
                grounded = true;
            }
                
        } 
        else {
            player.pos->z += (delta.z > 0) ? overlapZ : -overlapZ;
        }
    }
}








int main(){
    Window w1;
    if (w1.createWindow(800, 600, "My Engine") != 0) return -1;

    glewInit(); 

    ShaderProgram prog;
    if (prog.compileShader(vSource, fSource) != 0) return -2;

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCubeSigma), verticesCubeSigma, GL_STATIC_DRAW);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);


    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("wall.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        printf("Failed to load texture! Check path.\n");
    }
    stbi_image_free(data);



    glEnable(GL_DEPTH_TEST); 


    glm::vec3 cubePosition = glm::vec3(3.0f,0.0f,-3.0f);
    glm::vec3 cubePosition2 = glm::vec3(0.0f,2.0f,0.0f);


    glm::vec3 cubeScale = glm::vec3(100.0f,1.0f,100.0f);
    glm::vec3 cube2Scale = glm::vec3(1.0f,4.0f,1.0f);
    float gravity = 15.0f;



    float speed = 10.0f;

    double mouseX, mouseY;
    static double lastX = 400.0, lastY = 300.0;




    std::vector<BoxCollider> worldColliders;
    worldColliders.push_back({ &cubePosition, cubeScale, true }); 
    BoxCollider playerColl = { &cubePosition2, cube2Scale, false };



    glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 0.7f);    
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f); 

    float yaw   = -90.0f; 
    float pitch = 0.0f;
    glm::vec3 cs = glm::vec3(0.0f, 2.0f, 0.0f); 
    float velY = 0.0f;
    glfwSetInputMode(w1.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    while (!glfwWindowShouldClose(w1.window)) {
        Time::update();

        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        
        cameraPos = cubePosition2 + cs; 


        prog.use();
        glUniform1i(glGetUniformLocation(prog.ID, "ourTexture"), 0); 
        glm::vec3 lightPos(1.2f, 10.0f, 2.0f); 
        prog.setVec3("lightPos", lightPos);
        prog.setVec3("lightColor", glm::vec3(0.4f, 0.4f, 0.4f));



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



        glm::vec3 direction;
        float cosPitch = cos(glm::radians(pitch));

        direction.x = cos(glm::radians(yaw)) * cosPitch;
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cosPitch;

        cameraFront = glm::normalize(direction);



        glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));
        float cameraSpeed = speed * Time::deltaTime;


        glm::vec3 forward = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
        glm::vec3 right = glm::normalize(glm::cross(forward, cameraUp));
        
        
        if (glfwGetKey(w1.window, GLFW_KEY_W) == GLFW_PRESS) cubePosition2 += forward * cameraSpeed;
        if (glfwGetKey(w1.window, GLFW_KEY_S) == GLFW_PRESS) cubePosition2 -= forward * cameraSpeed;
        if (glfwGetKey(w1.window, GLFW_KEY_A) == GLFW_PRESS) cubePosition2 -= right * cameraSpeed;
        if (glfwGetKey(w1.window, GLFW_KEY_D) == GLFW_PRESS) cubePosition2 += right * cameraSpeed;
        

        
        velY -= gravity * Time::deltaTime; 
        cubePosition2.y += velY * Time::deltaTime;

        bool isGrounded = false; 
        for (auto& wall : worldColliders) {
            resolveCollision(wall, playerColl, isGrounded);
        }

        if (isGrounded) {
            if (velY < 0) velY = 0; 

            if (glfwGetKey(w1.window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                velY = 7.0f;
            }
        }
        
        

        
        
        

        glm::mat4 projection = glm::perspective(glm::radians(70.0f), 800.0f/600.0f, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos,cameraPos + cameraFront,cameraUp);
        
        
        prog.setMat4("view",view);
        prog.setMat4("projection",projection);
        
        
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0); // Активируем слот 0
        glBindTexture(GL_TEXTURE_2D, texture1);



        

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubePosition);
        model = glm::scale(model, cubeScale);
        prog.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); 
        
        
        
        
        
        
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glfwSwapBuffers(w1.window);
        glfwPollEvents();
    }
    glfwTerminate();

    return 0;
}
