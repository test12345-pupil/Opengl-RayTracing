#ifndef SHAPE_TRIANGLE_H
#define SHAPE_TRIANGLE_H
#include "gl_env.h"
#include "shape.h"

struct Triangle: public Shape{
    public:
    glm::fvec3 a,b,c;
    Triangle(glm::fvec3 a, glm::fvec3 b, glm::fvec3 c, glm::fvec3 color):a(a), b(b), c(c){
        material.normal = glm::normalize(glm::cross(b - a, c - a));
        material.Color = color;
    }
    HitResult getHitResult(Ray r){
        glm::fvec3 D = r.direction, N = material.normal;
        if(glm::dot(D,N) > 0.0f) N=-N;
        float coef = glm::dot(a - r.start, N) / glm::dot(D, N);
        if(coef < 1e-4) return HitResult();
        glm::fvec3 X = r.start + D * coef;
        glm::fvec3 c1 = glm::cross(b-a, X-a);
        glm::fvec3 c2 = glm::cross(c-b, X-b);
        glm::fvec3 c3 = glm::cross(a-c, X-c);
        if(glm::dot(c1, material.normal)<0 || glm::dot(c2, material.normal)<0 || glm::dot(c3, material.normal)<0) return HitResult();
        HitResult res(
            glm::distance(X, r.start),
            X,
            material);
        res.material.normal = N;
        return res;
    }
};



#endif