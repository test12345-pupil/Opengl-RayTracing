#ifndef SHAPE_BALL_H
#define SHAPE_BALL_H
#include "gl_env.h"
#include <cmath>
#include "shape.h"

struct Sphere: public Shape{
    glm::fvec3 center; // 球心
    double radius; // 半径
    HitResult getHitResult(Ray r){
        // 不考虑 r.start 在球内
        if(glm::dot(r.direction, center - r.start) <= 0){
            return HitResult();
        }else{
            double d_p = glm::dot(center - r.start, r.direction) / glm::dot(r.direction, r.direction);
            glm::fvec3 p = r.start + (float)d_p * r.direction;
            double d = glm::distance(p, center);
            if(d > radius){
                return HitResult();
            }else{
                double d1 = sqrt(radius * radius - d * d);
                HitResult result = {d_p - d1,
                    r.start + r.direction * float(d_p - d1),
                    material};
                result.material.normal = glm::normalize(result.hitPoint - center);
                return result;
            }
        }
    }
    Sphere(glm::fvec3 c, double r, glm::fvec3 color): center(c), radius(r){
        material.Color = color;
    }
};



#endif