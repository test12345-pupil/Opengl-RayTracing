#include "gl_env.h"
#include <omp.h>

#include <cmath>
#include <cstring>

#include "scene.h"
#include "randomtools.h"
#include "triangle.h"
#include "sphere.h"

const int WIDTH = 512, HEIGHT = 512;
const float C = 25;
glm::fvec3 screen[HEIGHT][WIDTH];
// 按照WIDTH*HEIGHT划分像素，与当前窗口大小无关

Scene scene;

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		return;
	}
}

const glm::fvec3 COL_RED(1, 0.5, 0.5),
	COL_GREEN(0.5, 1, 0.5),
	COL_BLUE(0.5, 0.5, 1),
	COL_YELLOW(1.0, 1.0, 0.1),
	COL_CYAN(0.1, 1.0, 1.0),
	COL_MAGENTA(1.0, 0.1, 1.0),
	COL_GRAY(0.5, 0.5, 0.5),
	COL_WHITE(1, 1, 1);


int main(void) {
	scene.cam.setPosition(0,0,0.8);
	scene.cam.setDirection(0,0.1,-1);

	scene.alphaW = M_PI / 4;
	scene.alphaH = scene.alphaW / WIDTH * HEIGHT;
	scene.tg_alphaW = tanf(scene.alphaW);
	scene.tg_alphaH = tanf(scene.alphaH);

	// 光源
	Triangle lamp1(glm::fvec3(0.3, 1-1e-5, 0.3), glm::fvec3(-0.3, 1-1e-5, 0.3), glm::fvec3(-0.3, 1-1e-5, -0.3), COL_WHITE);
	lamp1.material.isLighter = true;
	scene.addShape(&lamp1);

	Triangle lamp2(glm::fvec3(0.3, 1-1e-5, 0.3), glm::fvec3(-0.3, 1-1e-5, -0.3), glm::fvec3(0.3, 1-1e-5, -0.3), COL_WHITE);
	lamp2.material.isLighter = true;
	scene.addShape(&lamp2);
	
	// 底部背景板
	scene.addShape(new Triangle(glm::fvec3(1, -1, 1), glm::fvec3(-1, -1, -1), glm::fvec3(-1, -1, 1), COL_GRAY));
	scene.addShape(new Triangle(glm::fvec3(1, -1, 1), glm::fvec3(1, -1, -1), glm::fvec3(-1, -1, -1), COL_GRAY));

	// 顶部背景板
	scene.addShape(new Triangle(glm::fvec3(1, 1, 1), glm::fvec3(-1, 1, 1), glm::fvec3(-1, 1, -1), COL_GRAY));
	scene.addShape(new Triangle(glm::fvec3(1, 1, 1), glm::fvec3(-1, 1, -1), glm::fvec3(1, 1, -1), COL_GRAY));

	// 前面背景板
	scene.addShape(new Triangle(glm::fvec3(1, -1, -1), glm::fvec3(-1, 1, -1), glm::fvec3(-1, -1, -1), COL_GRAY));
	scene.addShape(new Triangle(glm::fvec3(1, -1, -1), glm::fvec3(1, 1, -1), glm::fvec3(-1, 1, -1), COL_GRAY));

	// 左边背景板
	scene.addShape(new Triangle(glm::fvec3(-1, -1, -1), glm::fvec3(-1, 1, 1), glm::fvec3(-1, -1, 1), COL_GRAY));
	scene.addShape(new Triangle(glm::fvec3(-1, -1, -1), glm::fvec3(-1, 1, -1), glm::fvec3(-1, 1, 1), COL_GRAY));

	// 右边背景板
	scene.addShape(new Triangle(glm::fvec3(1, 1, 1), glm::fvec3(1, -1, -1), glm::fvec3(1, -1, 1), COL_GRAY));
	scene.addShape(new Triangle(glm::fvec3(1, -1, -1), glm::fvec3(1, 1, 1), glm::fvec3(1, 1, -1), COL_GRAY));

	// 后面背景板
	scene.addShape(new Triangle(glm::fvec3(1, -1, 1), glm::fvec3(-1, 1, 1), glm::fvec3(-1, -1, 1), COL_GRAY));
	scene.addShape(new Triangle(glm::fvec3(1, -1, 1), glm::fvec3(1, 1, 1), glm::fvec3(-1, 1, 1), COL_GRAY));


	{
		Sphere*sphere = new Sphere(glm::fvec3(0.7, -0.7, -0.5), 0.2, COL_RED);
		sphere->material.reflectRate = 0.2;
		sphere->material.reflectRough = 0.1;
		sphere->material.refractRate = 0.3;
		sphere->material.refractRough = 0.1;
		sphere->material.refractAngle = 1.0;
		scene.addShape(sphere);
	}

	{
		Sphere*sphere = new Sphere(glm::fvec3(0.3, -0.7, -0.6), 0.3, COL_BLUE);
		sphere->material.reflectRate = 0.9;
		sphere->material.reflectRough = 0;
		sphere->material.refractRate = 0;
		scene.addShape(sphere);
	}

	{
		Sphere*sphere = new Sphere(glm::fvec3(-0.3, -0.7, -0.5), 0.2, COL_CYAN);
		sphere->material.reflectRate = 0;
		sphere->material.reflectRough = 0.9;
		sphere->material.refractRate = 0.9;
		sphere->material.refractAngle = 1;
		scene.addShape(sphere);
	}

	{
		Sphere*sphere = new Sphere(glm::fvec3(-0.7, -0.7, -0.5), 0.2, COL_YELLOW);
		scene.addShape(sphere);
	}

    GLFWwindow* window;
 
    if (!glfwInit()) return -1;
 

    window = glfwCreateWindow(WIDTH, HEIGHT, "Path Tracing Result", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
 
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);


	const int SAMPLE_PER_FRAME = 1;
	
	int nsamples = 0;

    while (!glfwWindowShouldClose(window)) {
        static float passed_time = 0, passed_time_last, delta_time;
        passed_time_last = passed_time;
        passed_time = (float) glfwGetTime();
        delta_time = passed_time - passed_time_last;


		int width , height;
		glfwGetWindowSize(window, &width, &height);
		
		if(scene.handleCameraChange(window, delta_time, width, height)){
			nsamples = 0;
			memset(screen, 0, sizeof screen);
		}

		
		omp_set_num_threads(64); // 线程个数
		#pragma omp parallel for

		for(int i=0; i<HEIGHT; ++i){
			for(int j=0; j<WIDTH; ++j){
				for(int id=0; id<SAMPLE_PER_FRAME; ++id){
					screen[i][j]+=scene.sampleOnce(1.0*i/HEIGHT + randf(0, 1.0/HEIGHT), 1.0*j/WIDTH + randf(0, 1.0/WIDTH));
				}
			}
		}
		nsamples += SAMPLE_PER_FRAME;

		glClearColor (0.0, 0.0, 0.0, 0.0); 
		glClear(GL_COLOR_BUFFER_BIT);

		glPointSize(2.0f);
		glBegin(GL_POINTS);

		auto drawPixel = [&](int i, int j, float r, float g, float b){
			glColor3f(r * C, g * C, b * C);
			glVertex2f(2.0*i/HEIGHT - 1, 2.0*j/WIDTH - 1);
		};

		for(int i=0; i<HEIGHT; ++i){
			for(int j=0; j<WIDTH; ++j){
				drawPixel(i, j, screen[i][j].x / nsamples, screen[i][j].y / nsamples, screen[i][j].z / nsamples);
			}
		}
	
		glEnd();
		
        glfwSwapBuffers(window);

        glfwPollEvents();

		printf("FPS: %.10lf\n", 1 / delta_time);
    }
 
    glfwTerminate();
    return 0;
}