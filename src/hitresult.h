#ifndef HITRESULT_H
#define HITRESULT_H
#include <glm/glm.hpp>
#include "material.h"

// 记录光线与场景的交点
struct HitResult{
    double distance; // 与交点距离，若为nan则无效
    glm::fvec3 hitPoint; // 光线命中点
    Material material; // 命中点的材质
    HitResult(){
        distance = NAN;
    }
    HitResult(double distance, glm::vec3 hitPoint, Material material):
        distance(distance), hitPoint(hitPoint), material(material){}
};

#endif