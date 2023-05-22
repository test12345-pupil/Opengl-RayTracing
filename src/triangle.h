#ifndef SHAPE_TRIANGLE_H
#define SHAPE_TRIANGLE_H
#include "gl_env.h"
#include "aabb.h"
#include "shape.h"
#include <algorithm>

struct Triangle: public Shape{
    public:
    glm::fvec3 a,b,c;
    glm::fvec2 ta,tb,tc;
    
    Triangle(){}
    Triangle(glm::fvec3 a, glm::fvec3 b, glm::fvec3 c, 
        glm::fvec2 ta, glm::fvec2 tb, glm::fvec2 tc, 
        glm::fvec3 n, int tex):
        a(a), b(b), c(c), ta(ta), tb(tb), tc(tc){
        material.normal = glm::normalize(glm::cross(b - a, c - a));
        // material.normal = n;
    }
    Triangle(glm::fvec3 a, glm::fvec3 b, glm::fvec3 c, 
        glm::fvec3 color):a(a), b(b), c(c){
        material.normal = glm::normalize(glm::cross(b - a, c - a));
        material.Color = color;
        ta.x = NAN;
    }
    HitResult getHitResult(Ray r){
        glm::fvec3 D = r.direction, N = material.normal;
        if(glm::dot(D,N) > 0.0f) N = -N;
        float coef = glm::dot(a - r.start, N) / glm::dot(D, N);
        if(coef < 1e-4) return {};
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
        if(!std::isnan(ta.x)){
            res.material.Color = 0.75f * (res.hitPoint + glm::fvec3(1)) ;
        }
        return res;
    }
    AABB getAABBbox(){
        return {_min(a, b, c), _max(a, b, c)};
    }
    static glm::fvec3 _min(glm::fvec3 a, glm::fvec3 b){
        return {std::min({a.x, b.x}), std::min({a.y, b.y}), std::min({a.z, b.z})};
    }
    static glm::fvec3 _max(glm::fvec3 a, glm::fvec3 b){
        return {std::max({a.x, b.x}), std::max({a.y, b.y}), std::max({a.z, b.z})};
    }
    static glm::fvec3 _min(glm::fvec3 a, glm::fvec3 b, glm::fvec3 c){
        return {std::min({a.x, b.x, c.x}), std::min({a.y, b.y, c.y}), std::min({a.z, b.z, c.z})};
    }
    static glm::fvec3 _max(glm::fvec3 a, glm::fvec3 b, glm::fvec3 c){
        return {std::max({a.x, b.x, c.x}), std::max({a.y, b.y, c.y}), std::max({a.z, b.z, c.z})};
    }
};

AABB combine(AABB a, AABB b){
    return {Triangle::_min(a.d, b.d), Triangle::_max(a.u, b.u)};
}


#endif