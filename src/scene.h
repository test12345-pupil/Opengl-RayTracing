#ifndef SCENE_H
#define SCENE_H

#include "gl_env.h"
#include <vector>
#include "shape.h"
#include "camera.h"
#include <cmath>
#include "randomtools.h"

struct Scene{
    std::vector<Shape*>shapes;
	Camera cam;

    float alphaH, alphaW;
    float tg_alphaH, tg_alphaW;

    void addShape(Shape *p){
        shapes.push_back(p);
    }

    glm::vec3 RayTrace(Ray r, int depth = 0){
        const static float RAYTRACE_DIE_PROB = 0.1;

        HitResult result;
        for(Shape *s : shapes){
            HitResult res = s->getHitResult(r);
            if((!std::isnan(res.distance) && res.distance > 1e-4) &&
               (std::isnan(result.distance) || res.distance < result.distance)){
                    result = res;
                }
        }
        if(std::isnan(result.distance)) return {0,0,0};
        if(result.material.isLighter) return result.material.Color;
        if(randf() < RAYTRACE_DIE_PROB) return {0,0,0};
        float _r = randf(), C = abs(glm::dot(-r.direction, result.material.normal)) / (1-RAYTRACE_DIE_PROB);
        glm::fvec3 dir = randomDirection(result.material.normal);
        if(_r < result.material.reflectRate){ // 镜面反射
            glm::vec3 _c = RayTrace({result.hitPoint, 
                glm::mix(glm::reflect(-1.0f * r.direction, result.material.normal) , dir, result.material.reflectRough)}, 
                depth + 1);
            return _c * C;
        }else if(_r < result.material.reflectRate + result.material.refractRate){ // 折射
            glm::vec3 _c = RayTrace({result.hitPoint, 
                glm::mix(glm::refract(-1.0f * r.direction, result.material.normal, result.material.refractRate) , dir, result.material.reflectRough)},
                depth + 1);
            return _c * C;

        }else{ // 漫反射
            glm::vec3 _c = RayTrace({result.hitPoint, dir}, depth + 1);

            return result.material.Color * _c * C;
        }
    }

    glm::vec3 sampleOnce(float x, float y){
        assert(0<=x && x<=1 && 0<=y && y<=1);
        x=x*2-1, y=y*2-1;
        
        return RayTrace(Ray{cam.getPos(), glm::normalize(cam.getDir() - cam.getAxisY() * x / tg_alphaH - cam.getAxisX() * y / tg_alphaW)});
    }

    // 处理上一帧的摄像机变化，返回是否有变动
    bool handleCameraChange(GLFWwindow* window, float dt, int width, int height){
        bool changed = false;

        const static float delta_angle1 = 0.5;

        bool W = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
        bool A = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
        bool S = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
        bool D = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
        bool SP = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        bool LC = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;

        float d_fb = delta_angle1 * dt * (W?(S?0:1):(S?-1:0));
        float d_lr = -delta_angle1 * dt * (A?(D?0:-1):(D?1:0));
        float d_ud = delta_angle1 * dt * (SP?(LC?0:1):(LC?-1:0));
        
        cam.handle_eye(d_fb, d_lr, d_ud);

        if((W^S)|(A^D)|(SP^LC)) changed = true;

        static bool alt_holding = false;

        if(glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS){
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            glfwSetCursorPos(window, .5*width, .5*height);
            xpos -= .5*width, ypos -= .5*height;
            
            static const float delta_angle2 = 0.005;
            if(alt_holding){
                cam.handle_dir(xpos * delta_angle2, -ypos * delta_angle2);
                if(fabs(xpos) + fabs(ypos) > 1e-5)changed = true;
            }else{
                alt_holding = true;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            }
        }else if(alt_holding){
            alt_holding = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        return changed;
    }
 
};



#endif