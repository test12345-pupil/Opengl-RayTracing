#ifndef RANDOMTOOLS_H
#define RANDOMTOOLS_H


#include "gl_env.h"
#include <random>


std::uniform_real_distribution<float> _rf(0.0,1.0);
std::default_random_engine generator;
float randf() {return _rf(generator);}

float randf(float l, float r) {
    return _rf(generator) * (r-l) + l;
}

std::uniform_int_distribution<int> _ri(0, INT_MAX);

int randi(int r) {
    return _ri(generator) % r;
}


glm::vec3 randomDirection(glm::vec3 norm){
    glm::vec3 d;
    do{
        d = glm::vec3(randf(-1,1),randf(-1,1),randf(-1,1));
    }while(glm::dot(d,d) > 1.0);
    return glm::normalize(norm + glm::normalize(d));
}

#endif