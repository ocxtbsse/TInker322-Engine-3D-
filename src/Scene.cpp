#include "Scene.hpp"
#include <vector>
#include <iostream>

extern int meshLoadUlta(GameObject& targetObj,char* modelName); 
std::vector<GameObject> sceneObjects;
std::vector<Camera> sceneCamers;

int startSceneCreateOnjects(){
    Camera cam;
    cam.position = glm::vec3(0.0f, 0.0f, -2.0f);
    cam.front = glm::vec3(0.0f, 0.0f, -1.0f);
    cam.up = glm::vec3(0.0f, 1.0f, 0.0f); 
    pushBackCamera(cam);



    GameObject cube;
    cube.mesh.modelName = "model.obj";
    meshLoadUlta(cube,cube.mesh.modelName);
    cube.position = glm::vec3(0.0f,0.0f,-2.0f);
    cube.rotation = glm::vec3(0.0f, 0.0f, 0.0f); 
    cube.scale = glm::vec3(10.0f,10.0f,10.0f);
    pushBackObject(cube);


    GameObject cube2;
    cube2.mesh.modelName = "model.obj";
    meshLoadUlta(cube2,cube2.mesh.modelName);
    cube2.position = glm::vec3(0.0f,0.0f,-2.0f);
    cube2.rotation = glm::vec3(0.0f, 0.0f, 0.0f); 
    cube2.scale = glm::vec3(10.0f,10.0f,10.0f);
    pushBackObject(cube2);









    return 0;
}








void pushBackObject(GameObject PtrGMO){
    sceneObjects.push_back(PtrGMO);    
}


void pushBackCamera(Camera PtrCam){
    sceneCamers.push_back(PtrCam);
}


