#pragma once


#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <vector>

struct GameObject{
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;

    glm::mat4 getModelMatrix() {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        model = glm::scale(model, scale);
        
        return model;
    }


    struct Mesh{
        char* modelName;
        unsigned int VAOTM;
        unsigned int indicesSize;
    } mesh;



    struct Material{
        char* textureName;
        unsigned int textureID;
    } material;


    struct Collider{//скоро

    } collider;
};


struct Camera{
    
    glm::vec3 position;    
    glm::vec3 front;
    glm::vec3 up;
    

    
    int screenWidth = 800;
    int screenHeight = 600;
    
    
    
    float FOV = 70.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    glm::mat4 getCameraProjcetionMatrix() {
        float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
        glm::mat4 projection = glm::perspective(glm::radians(FOV), aspectRatio, nearPlane, farPlane);
        return projection;
    }

    glm::mat4 getCameraViewMatrix(){
        glm::mat4 view = glm::lookAt(position,position + front,up);;
        return view;
    }
};
