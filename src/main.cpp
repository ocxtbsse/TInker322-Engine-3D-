#define GLM_ENABLE_EXPERIMENTAL
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



float coinVertices[] = {

     0.00f,  0.00f,  0.05f,      0.50f, 0.50f,      0.0f, 0.0f, 1.0f, 
     0.00f,  0.50f,  0.05f,      0.50f, 1.00f,      0.0f, 0.0f, 1.0f, 
     0.25f,  0.43f,  0.05f,      0.75f, 0.93f,      0.0f, 0.0f, 1.0f, 
     0.43f,  0.25f,  0.05f,      0.93f, 0.75f,      0.0f, 0.0f, 1.0f, 
     0.50f,  0.00f,  0.05f,      1.00f, 0.50f,      0.0f, 0.0f, 1.0f,
     0.43f, -0.25f,  0.05f,      0.93f, 0.25f,      0.0f, 0.0f, 1.0f,
     0.25f, -0.43f,  0.05f,      0.75f, 0.07f,      0.0f, 0.0f, 1.0f,
     0.00f, -0.50f,  0.05f,      0.50f, 0.00f,      0.0f, 0.0f, 1.0f,
    -0.25f, -0.43f,  0.05f,      0.25f, 0.07f,      0.0f, 0.0f, 1.0f,
    -0.43f, -0.25f,  0.05f,      0.07f, 0.25f,      0.0f, 0.0f, 1.0f,
    -0.50f,  0.00f,  0.05f,      0.00f, 0.50f,      0.0f, 0.0f, 1.0f,
    -0.43f,  0.25f,  0.05f,      0.07f, 0.75f,      0.0f, 0.0f, 1.0f,
    -0.25f,  0.43f,  0.05f,      0.25f, 0.93f,      0.0f, 0.0f, 1.0f, 


     0.00f,  0.00f, -0.05f,      0.50f, 0.50f,      0.0f, 0.0f, -1.0f,
     0.00f,  0.50f, -0.05f,      0.50f, 1.00f,      0.0f, 0.0f, -1.0f,
     0.25f,  0.43f, -0.05f,      0.75f, 0.93f,      0.0f, 0.0f, -1.0f,
     0.43f,  0.25f, -0.05f,      0.93f, 0.75f,      0.0f, 0.0f, -1.0f,
     0.50f,  0.00f, -0.05f,      1.00f, 0.50f,      0.0f, 0.0f, -1.0f,
     0.43f, -0.25f, -0.05f,      0.93f, 0.25f,      0.0f, 0.0f, -1.0f,
     0.25f, -0.43f, -0.05f,      0.75f, 0.07f,      0.0f, 0.0f, -1.0f,
     0.00f, -0.50f, -0.05f,      0.50f, 0.00f,      0.0f, 0.0f, -1.0f,
    -0.25f, -0.43f, -0.05f,      0.25f, 0.07f,      0.0f, 0.0f, -1.0f,
    -0.43f, -0.25f, -0.05f,      0.07f, 0.25f,      0.0f, 0.0f, -1.0f,
    -0.50f,  0.00f, -0.05f,      0.00f, 0.50f,      0.0f, 0.0f, -1.0f,
    -0.43f,  0.25f, -0.05f,      0.07f, 0.75f,      0.0f, 0.0f, -1.0f,
    -0.25f,  0.43f, -0.05f,      0.25f, 0.93f,      0.0f, 0.0f, -1.0f
};

unsigned int coinIndices[] = {
   
    0, 1, 2,  0, 2, 3,  0, 3, 4,  0, 4, 5,  0, 5, 6,  0, 6, 7,
    0, 7, 8,  0, 8, 9,  0, 9, 10, 0, 10, 11, 0, 11, 12, 0, 12, 1,
    
    13, 15, 14, 13, 16, 15, 13, 17, 16, 13, 18, 17, 13, 19, 18, 13, 20, 19,
    13, 21, 20, 13, 22, 21, 13, 23, 22, 13, 24, 23, 13, 25, 24, 13, 14, 25,
    
    1, 14, 15, 1, 15, 2,   2, 15, 16, 2, 16, 3,   3, 16, 17, 3, 17, 4,
    4, 17, 18, 4, 18, 5,   5, 18, 19, 5, 19, 6,   6, 19, 20, 6, 20, 7,
    7, 20, 21, 7, 21, 8,   8, 21, 22, 8, 22, 9,   9, 22, 23, 9, 23, 10,
    10, 23, 24, 10, 24, 11, 11, 24, 25, 11, 25, 12, 12, 25, 14, 12, 14, 1
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

struct PlayerCollider {
    glm::vec3* pos;
    glm::vec3 size;
};

struct HardBoxCollider {
    glm::vec3 pos;
    glm::mat4 rotMat;
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



void resolveQuakeCollision(const HardBoxCollider& wall, PlayerCollider& player, bool& grounded) {
    glm::vec3 axes[3];
    axes[0] = glm::vec3(wall.rotMat[0]);
    axes[1] = glm::vec3(wall.rotMat[1]);
    axes[2] = glm::vec3(wall.rotMat[2]);

    glm::vec3 delta = *player.pos - wall.pos;

    float minOverlap = 10000.0f; // :)
    glm::vec3 bestNormal(0.0f);

    for (int i = 0; i < 3; i++) {
        float dist = glm::dot(delta, axes[i]);
        
        float combinedSize = (wall.size[i] + player.size[i]) * 0.5f;
        
        float overlap = combinedSize - glm::abs(dist);

        if (overlap <= 0.0f) return;

        if (overlap < minOverlap) {
            minOverlap = overlap;
            bestNormal = (dist > 0.0f) ? axes[i] : -axes[i];
        }
    }

    *player.pos += bestNormal * minOverlap;

    if (glm::dot(bestNormal, glm::vec3(0, 1, 0)) > 0.5f) {
        grounded = true;
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


  //монетка такая уоуоуоу ahahaaasduhaudsahhuAHSHAUSHUAHSUASHAs
    unsigned int VBO2, VAO2, EBO2;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);
    glGenBuffers(1, &EBO2);

    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coinVertices), coinVertices, GL_STATIC_DRAW);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(coinIndices), coinIndices, GL_STATIC_DRAW);




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


    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    data = stbi_load("images.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);



    unsigned int texture3;
    glGenTextures(1, &texture3);
    glBindTexture(GL_TEXTURE_2D, texture3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    data = stbi_load("med.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);



    glEnable(GL_DEPTH_TEST); 


    glm::vec3 cubePosition = glm::vec3(3.0f,0.0f,-3.0f);
    glm::vec3 cubePosition2 = glm::vec3(0.0f,4.0f,0.0f);

    glm::vec3 cubePosition3 = glm::vec3(53.0f,9.0f,-3.0f);
    

    glm::vec3 cubePosition4 = glm::vec3(-40.0f,9.0f,-3.0f);


    glm::vec3 cubePosition5 = glm::vec3(10.0f,9.0f,-50.0f);


    glm::vec3 cubePosition6 = glm::vec3(10.0f,9.0f,40.0f);



    glm::vec3 ispiTaniePos = glm::vec3(15.0f,6.0f,0.0f);


    glm::vec3 cubeScale = glm::vec3(100.0f,1.0f,100.0f);
    glm::vec3 cube2Scale = glm::vec3(1.0f,4.0f,1.0f);
    glm::vec3 cube3Scale = glm::vec3(1.0f,100.0f,100.0f);
    glm::vec3 cube4Scale = glm::vec3(100.0f,100.0f,1.0f);


    glm::vec3 ispiTanieScale = glm::vec3(100.0f,2.0f,10.0f);

    
    glm::vec3 CoinScale = glm::vec3(10.0f,10.0f,10.0f);
    glm::vec3 CoinPosition = glm::vec3(45.0f,25.0f,30.0f);
    glm::vec3 CoinPosition2 = glm::vec3(15.0f,25.0f,30.0f);
    glm::vec3 CoinFScale = glm::vec3(11.0f,11.0f,23.0f);



    glm::mat4 rampaRot = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0, 0, 1));


    float gravity = 30.0f;



    float speed = 30.0f;


    float tryaskaSpeed = 1.0f;
    float amplitude = 0.6f;


    double mouseX, mouseY;
    static double lastX = 400.0, lastY = 300.0;


    
    
    std::vector<HardBoxCollider> worldColliders;
    glm::mat4 noRot = glm::mat4(1.0f); 
    worldColliders.push_back({cubePosition,noRot, cubeScale, true }); 
    worldColliders.push_back({cubePosition3,noRot,cube3Scale, true });
    worldColliders.push_back({cubePosition4, noRot,cube3Scale, true });
    worldColliders.push_back({cubePosition5, noRot,cube4Scale, true });
    worldColliders.push_back({cubePosition6, noRot,cube4Scale, true });
    worldColliders.push_back({CoinPosition, noRot,CoinScale, true });

    worldColliders.push_back({CoinPosition2, noRot,CoinScale, true });



    worldColliders.push_back({ispiTaniePos,rampaRot,ispiTanieScale, true });


    std::vector<HardBoxCollider> world;

    glm::mat4 rMat = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0, 0, 1));
    glm::mat4 rMatCoin = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1));

    world.push_back({ glm::vec3(5, 0, 0), rMat, glm::vec3(10, 1, 10), true });

    PlayerCollider pColl = { &cubePosition2, cube2Scale};




    



    glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 0.7f);    
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f); 

    float yaw   = -90.0f; 
    float pitch = 0.0f;
    glm::vec3 cs = glm::vec3(0.0f, 2.0f, 0.0f); 
    float velY = 0.0f;






    glm::mat4 model2 = glm::mat4(1.0f);
    model2 = glm::translate(model2,cubePosition3);
    model2 = glm::scale(model2,cube3Scale);

    glm::mat4 model3 = glm::mat4(1.0f);
    model3 = glm::translate(model3,cubePosition4);
    model3 = glm::scale(model3,cube3Scale);


    glm::mat4 model4 = glm::mat4(1.0f);
    model4 = glm::translate(model4,cubePosition5);
    model4 = glm::scale(model4,cube4Scale);


    glm::mat4 model5 = glm::mat4(1.0f);
    model5 = glm::translate(model5,cubePosition6);
    model5 = glm::scale(model5,cube4Scale);


    glm::mat4 model6 = glm::mat4(1.0f);
    model6 = glm::translate(model6,ispiTaniePos);
    
    model6 = model6 * rMat; 


    model6 = glm::scale(model6,ispiTanieScale);





    glm::mat4 coin = glm::mat4(1.0f);
    coin = glm::translate(coin,CoinPosition);
    coin = glm::rotate(coin, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    coin = glm::scale(coin,CoinFScale);


    glm::mat4 coin2 = glm::mat4(1.0f);
    coin2 = glm::translate(coin2,CoinPosition2);
    coin2 = glm::rotate(coin2, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    coin2 = glm::scale(coin2,CoinFScale);





    glfwSetInputMode(w1.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    while (!glfwWindowShouldClose(w1.window)) {
        Time::update();

        
        glClearColor(0.2f, 0.3f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        float time = (float)glfwGetTime();
        float sway = sin(time * tryaskaSpeed) * amplitude;
        cameraPos = cubePosition2 + cs + glm::vec3(0.0f, sway, 0.0f);  


        prog.use();
        glUniform1i(glGetUniformLocation(prog.ID, "ourTexture"), 0); 
        glm::vec3 lightPos(0.0f, 10.0f, 0.0f); 
        prog.setVec3("lightPos", lightPos);
        prog.setVec3("lightColor", glm::vec3(0.7f, 0.7f, 1.0f));



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
        
        
        if (glfwGetKey(w1.window, GLFW_KEY_W) == GLFW_PRESS){
            cubePosition2 += forward * cameraSpeed;
            tryaskaSpeed = 10.0f;
        }
        else if (glfwGetKey(w1.window, GLFW_KEY_S) == GLFW_PRESS){
            cubePosition2 -= forward * cameraSpeed;
            tryaskaSpeed = 10.0f;

        } else{
            tryaskaSpeed = 1.0f;
        }




        if (glfwGetKey(w1.window, GLFW_KEY_A) == GLFW_PRESS){
            cubePosition2 -= right * cameraSpeed;
            tryaskaSpeed = 10.0f;

        }
        if (glfwGetKey(w1.window, GLFW_KEY_D) == GLFW_PRESS){
            cubePosition2 += right * cameraSpeed;
            tryaskaSpeed = 10.0f;
        } 

        
        velY -= gravity * Time::deltaTime; 
        cubePosition2.y += velY * Time::deltaTime;

        bool isGrounded = false; 
        for (auto& wall : worldColliders) {
            resolveQuakeCollision(wall, pColl, isGrounded);
        }




        if (isGrounded) {
            if (velY < 0) velY = 0;

            if (glfwGetKey(w1.window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                velY = 15.0f;
            }
        }
        
        

        
        
        

        glm::mat4 projection = glm::perspective(glm::radians(70.0f), 800.0f/600.0f, 0.1f, 1000.0f);
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
        

        glBindTexture(GL_TEXTURE_2D, texture2);
        prog.setMat4("model", model2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); 



        prog.setMat4("model", model3);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); 


        prog.setMat4("model", model4);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        prog.setMat4("model", model5);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        prog.setMat4("model", model6);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);



        glBindVertexArray(VAO2);
        prog.setMat4("model",coin);
        glDrawElements(GL_TRIANGLES, sizeof(coinIndices)/sizeof(unsigned int), GL_UNSIGNED_INT, 0);



        prog.setMat4("model",coin2);
        glDrawElements(GL_TRIANGLES, sizeof(coinIndices)/sizeof(unsigned int), GL_UNSIGNED_INT, 0);


        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glfwSwapBuffers(w1.window);
        glfwPollEvents();
    }
    glfwTerminate();

    return 0;
}
