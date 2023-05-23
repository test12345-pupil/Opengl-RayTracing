#ifndef CAMERA_H
#define CAMERA_H


#include "gl_env.h"
#include <iostream>
#include <vector>
#include <string>
#ifndef M_PI
#define M_PI (3.1415926535897932)
#endif

template<class T>
void doRestrict(T&x, T D, T U){
    assert(D<=U);
    if(x<D)x=D;
    else if(x>U)x=U;
}

struct Camera{
    private:
    glm::fvec3 position; // 相机位置
    glm::fvec3 direction; // 相机注视方向（注视点为pos+dir），始终normalized
    glm::fvec3 up; // 上向量，固定为 (0,1,0)
    glm::fvec3 axisY; // 屏幕左右向量
    glm::fvec3 axisX; // 屏幕上下向量
    
    bool axis_available;

    void recalculateAxis(){
        axis_available = true;
        axisY = glm::normalize(glm::cross(direction, up));
        axisX = glm::normalize(glm::cross(direction, axisY));
    }

    public:
    float fov;
    Camera(): position(0.0f, 0.0f, 0.0f), direction(0.0f, 0.0f, 1.0f), up(0.0f, 1.0f, 0.0f), fov(60.0), axis_available(false) {}
    void setPosition(float x,float y,float z){position = glm::fvec3(x,y,z);}
    void setDirection(float x,float y,float z){direction = glm::fvec3(x,y,z);}
    glm::mat4 get_view() const {
        return glm::lookAt(position, position + direction, up);
    }
    const glm::fvec3& getAxisX(){
        if(axis_available) return axisX;
        recalculateAxis();
        return axisX;
    }
    const glm::fvec3& getAxisY(){
        if(axis_available) return axisY;
        recalculateAxis();
        return axisY;
    }
    const glm::fvec3& getUp() const {
        return up;
    }
    const glm::fvec3& getDir() const {
        return direction;
    }
    const glm::fvec3& getPos() const {
        return position;
    }
    void handle_dir(float dx, float dy){ 
        /** 
         * 鼠标操作，修改direction
         */
        glm::fvec3 axis = getAxisY();
        
        glm::quat tmp = glm::cross(glm::angleAxis(dy, axis), glm::angleAxis(dx, up));
        tmp = glm::normalize(tmp);

        direction = glm::rotate(tmp, direction);

        axis_available = false;
    }
    void handle_eye(float d_fb, float d_lr, float d_ud){ 
        /**
         * d_fb 前后移动
         * d_lr 左右移动
         * d_ud 上下移动
         */
        position += direction * d_fb
            +  glm::normalize(glm::cross(direction, up)) * d_lr
            +  up * d_ud;
            
        axis_available = false;
    }
};

#endif