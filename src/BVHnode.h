#ifndef BVHNODE_H
#define BVHNODE_H

#include "gl_env.h"
#include <cmath>
#include "ray.h"
#include "aabb.h"
#include "triangle.h"

struct BVHnode{
    // 12 bytes
    int ls, rs, triangleID; // 若triangleID为-1则非叶子，否则为叶子
    // 24 bytes
    AABB AABBbox;
};


#endif