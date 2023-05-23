#ifndef SHAPE_H
#define SHAPE_H

#include "gl_env.h"
#include "hitresult.h"
#include "ray.h"
#include "material.h"


struct Shape{
    int ___;// 前8字节是虚函数表，用int做12bytes对齐
    Material material;
    virtual HitResult getHitResult(Ray r) = 0;
};



#endif