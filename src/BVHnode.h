#ifndef BVHNODE_H
#define BVHNODE_H

#include "gl_env.h"
#include <cmath>
#include "ray.h"
#include "aabb.h"
#include "triangle.h"

struct BVHnode{
    // 非叶子数据
    int ls, rs;
    AABB AABBbox;
    // 叶子数据
    Triangle leaf;

    //上面数据只有一个用到，可以使用union，不过很多CE没有用
    bool isleaf;
};


#endif