#include "Engine.h"
#include "Window.hpp"


extern Window w1;
extern int startSceneCreateOnjects();  



int main(){
    if (init() != 0) return -1; 


    startSceneCreateOnjects();

    while (!glfwWindowShouldClose(w1.window)) {
        
        update();
        
        render();

    }
    glfwTerminate();
    return 0;
}
