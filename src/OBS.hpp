#pragma once


#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <vector>
#include "Matrix.h"

struct GameObject{





    VectorS position;
    VectorS scale;
    VectorS rotate;


    char* tag;
    bool visible = true;


    Matrix getModelMatrix4x4(){
        Matrix model;
        model.set_transform(position.vec3[X],position.vec3[Y],position.vec3[Z]);
        model.rotateMat4x4(rotate.vec3[X] * DEGRAD,rotate.vec3[Y] * DEGRAD,rotate.vec3[Z] * DEGRAD);
        model.scaleMat4x4(scale.vec3[X],scale.vec3[Y],scale.vec3[Z]);
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
};









struct Camera{
    VectorS position;
    VectorS front;
    VectorS up;




    

    
    int screenWidth = 800;
    int screenHeight = 600;
    
    
    
    float FOV = 70.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;


    Matrix getProectionMat4x4(){
        float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
        Matrix projection;
        projection.perspective(FOV,aspectRatio,nearPlane,farPlane);
        return projection;
    }






    Matrix getCameraViewMat4x4(){
        Matrix view;


        VectorS target;
        target.vec3[0] = this->position.vec3[0] + this->front.vec3[0];
        target.vec3[1] = this->position.vec3[1] + this->front.vec3[1];
        target.vec3[2] = this->position.vec3[2] + this->front.vec3[2];

        view.lookAt(this->position, target, this->up);

        return view;

    }
};
