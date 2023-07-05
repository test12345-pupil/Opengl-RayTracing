#ifndef MATERIAL_H
#define MATERIAL_H
#include "gl_env.h"

struct Material{ 
    glm::fvec3 Color; // 表面颜色
    glm::fvec3 normal; // 法向量，应该始终normalized

    float reflectRate; // 反射光占比
    float reflectRough; // 粗糙程度

    float refractRate; // 折射光占比
    float refractAngle; // 折射率
    float refractRough; // 折射粗糙度
    
    float isLighter; // 是否为光源, 0/1
    
    Material(){
        isLighter = 0;
        reflectRate = 0;
        refractRate = 0;
    }
};



#endif