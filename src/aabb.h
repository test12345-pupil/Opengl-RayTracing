#ifndef AABB_H
#define AABB_H

#include "gl_env.h"
#include <cmath>
#include "ray.h"
#include <algorithm>

struct AABB{
    glm::fvec3 d, u;
    AABB(): u(-INFINITY), d(INFINITY){}
    AABB(glm::fvec3 d, glm::fvec3 u): u(u), d(d){}

    float getSurface() const {
        glm::fvec3 len = u-d;
        return len.x*len.y+len.x*len.z+len.y*len.z;
    }
    static glm::fvec3 _min(glm::fvec3 a, glm::fvec3 b){
        return {std::min({a.x, b.x}), std::min({a.y, b.y}), std::min({a.z, b.z})};
    }
    static glm::fvec3 _max(glm::fvec3 a, glm::fvec3 b){
        return {std::max({a.x, b.x}), std::max({a.y, b.y}), std::max({a.z, b.z})};
    }
    bool testInsect(const Ray r) const {
        glm::fvec3 idir = glm::fvec3(1.0 / r.direction.x, 1.0 / r.direction.y, 1.0 / r.direction.z);

        glm::fvec3 in = (d - r.start) * idir;
        glm::fvec3 out = (u - r.start) * idir;

        glm::fvec3 tmax = max(in, out);
        glm::fvec3 tmin = min(in, out);

        float t1 = std::min(tmax.x, std::min(tmax.y, tmax.z));
        float t0 = std::max(tmin.x, std::max(tmin.y, tmin.z));

        return t0 <= t1 && t1 >= 0;
    }

};

#endif