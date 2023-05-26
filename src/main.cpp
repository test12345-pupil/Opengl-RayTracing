#include "gl_env.h"

// #define DEBUG

#include <cmath>
#include <cstring>
#include <iostream>
#include <functional>
#include <fstream>

#include "scene.h"
#include "randomtools.h"
#include "triangle.h"
#include "sphere.h"
#include <wtypes.h>

#include "objloader.h"
#include "stb_image.h"
#include "Shader.h"


#ifdef DEBUG
#define RENDERDOC
#include "renderdoc_app.h"

RENDERDOC_API_1_1_2 *rdoc_api = NULL;

#endif


void glCheckError_(const char *file, int line) // https://www.geeksforgeeks.org/error-handling-in-opengl/
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
		exit(0);
    }
    return;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

const int width = 1000, height = 1000;
glm::fvec3 screen[height][width];
// 按照width*height划分像素，与当前窗口大小无关

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
	printf("%d\n",sizeof(glm::fmat3x2));
	#ifdef RENDERDOC
	// At init, on windows
	if(HMODULE mod = GetModuleHandleA("renderdoc.dll"))
	{
		pRENDERDOC_GetAPI RENDERDOC_GetAPI =
			(pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
		int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void **)&rdoc_api);
		assert(ret == 1);
	}
	#endif

	// == begin 设置相机
	scene.cam.setPosition(0,0,0.8);
	scene.cam.setDirection(0,0.1,-1);

	scene.alphaW = M_PI / 4;
	scene.alphaH = scene.alphaW / width * height;
	scene.tg_alphaW = tanf(scene.alphaW);
	scene.tg_alphaH = tanf(scene.alphaH);

	// == end 设置相机

	
	// == begin 布置场景

	// 光源
	Triangle lamp1(glm::fvec3(0.3, 1-1e-5, 0.3), glm::fvec3(-0.3, 1-1e-5, 0.3), glm::fvec3(-0.3, 1-1e-5, -0.3), COL_WHITE);
	lamp1.material.isLighter = 1;
	scene.addShape(&lamp1);

	Triangle lamp2(glm::fvec3(0.3, 1-1e-5, 0.3), glm::fvec3(-0.3, 1-1e-5, -0.3), glm::fvec3(0.3, 1-1e-5, -0.3), COL_WHITE);
	lamp2.material.isLighter = 1;
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

	// == end 布置场景

	

	{
		bool success = loadOBJ("../../data/aranara.obj", &scene, 0, {-0.5, -0.5, -0.5}, {0.5, 0.5, 0.5});
		
		// bool success = loadOBJ("../../data/bunny.obj", &scene, 0, {-0.5, -0.5, -0.5}, {0.5, 0.5, 0.5});

		assert(success);
	}

	scene.root = scene.buildBVH({});

    if (!glfwInit()) return -1;

    GLFWwindow* window;
 
    window = glfwCreateWindow(width, height, "New window", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
 
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
    if (glewInit() != GLEW_OK) exit(EXIT_FAILURE);

	
	// == begin bind shader programs
	Shader program1("../../data/shaders/vertexshader2.vs", "../../data/shaders/fragmentshader.fs");
	Shader program2("../../data/shaders/vertexshader2.vs", "../../data/shaders/fragmentshader2.fs");
	// == end bind shader programs

	// == begin bind VAO/VBO/EBO
	glBindVertexArray(0);

	float vertices[] = { 
         1.0f,  1.0f,   // top right
         1.0f, -1.0f,   // bottom right
        -1.0f, -1.0f,   // bottom left
        -1.0f,  1.0f,   // top left 
	};
    unsigned int indices[] = {  
        0, 1, 3,
        1, 2, 3 
    };
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// == end bind VAO/VBO/EBO

	// == begin bind texture/bufferTexture

	GLuint fbo, lastFrame;

	{
		
		GLuint textureNames[5];
		glGenTextures(5, textureNames);

		unsigned char* data;
		int width0, height0, nrChannels;
		data = stbi_load("../../data/aranara_image.png", &width0, &height0, &nrChannels, 0);

		if (data != NULL) {
			assert(nrChannels == 4);

			glActiveTexture(GL_TEXTURE0); 
			glBindTexture(GL_TEXTURE_2D, textureNames[0]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width0, height0, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		else {
			std::cout << "Failed to load texture" << std::endl;
		}

		assert(sizeof(BVHnode) % 12 == 0);
		assert(sizeof(Triangle) % 12 == 0);

		stbi_image_free(data);
		GLuint tbo[2];
		glGenBuffers(2, tbo);

		glActiveTexture(GL_TEXTURE1);
		glBindBuffer(GL_TEXTURE_BUFFER, tbo[0]);
		glBufferData(GL_TEXTURE_BUFFER, scene.BVH_id * sizeof(BVHnode), &scene.t[1], GL_STATIC_DRAW);
		glBindTexture(GL_TEXTURE_BUFFER, textureNames[1]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, tbo[0]);

		glActiveTexture(GL_TEXTURE2);
		glBindBuffer(GL_TEXTURE_BUFFER, tbo[1]);
		glBufferData(GL_TEXTURE_BUFFER, scene.triangle_id * sizeof(Triangle), &scene.triangle[0], GL_STATIC_DRAW);
		glBindTexture(GL_TEXTURE_BUFFER, textureNames[2]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, tbo[1]);

		lastFrame = textureNames[3];
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glBindTexture(GL_TEXTURE_2D, lastFrame);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lastFrame, 0);
		program2.use();
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, lastFrame);
		program2.setInt("texPass4", lastFrame);

		program1.use();
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, lastFrame);
		program2.setInt("lastFrame", lastFrame);

		GLuint attachments[] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, attachments);

	}

	program1.use();
	program1.setInt("u_width", width);
	program1.setInt("u_height", height);
	program1.setInt("u_size_BVHnode", sizeof(BVHnode) / 12);
	program1.setInt("u_size_Triangle", sizeof(Triangle) / 12);
	program2.use();
	program2.setInt("u_width", width);
	program2.setInt("u_height", height);

	// == end bind texture/bufferTexture

	puts("Start rendering loop...");

	int frameCounter = 0;
	
	int lastColorWeight = 0;
	


    while (!glfwWindowShouldClose(window)) {
		#ifdef RENDERDOC
		if(rdoc_api) rdoc_api->StartFrameCapture(NULL, NULL);
		#endif
		
		// fbo/tex[1]: lastFrame; [0]: outputFrame

		program1.use();

		++frameCounter;

        static float passed_time = 0, passed_time_last, delta_time;
        passed_time_last = passed_time;
        passed_time = (float) glfwGetTime();
        delta_time = passed_time - passed_time_last;


		glfwSetWindowSize(window, width, height);
		
		if(scene.handleCameraChange(window, delta_time, width, height)) lastColorWeight = 0;
		else ++lastColorWeight;

		program1.setInt("u_lastColorWeight", lastColorWeight);
		program1.setUInt("frameCounter", frameCounter);
		program1.setVec3("u_campos", scene.cam.getPos());
		program1.setVec3("u_camdir", scene.cam.getDir());
		glm::fvec3 _axisx = scene.cam.getAxisX() / scene.tg_alphaW;
		glm::fvec3 _axisy = scene.cam.getAxisY() / scene.tg_alphaH;
		program1.setVec3("u_camaxisX", _axisx);
		program1.setVec3("u_camaxisY", _axisy);
		
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor (0.0, 0.0, 0.0, 0.0); 
		glClear(GL_COLOR_BUFFER_BIT);
		
		program2.use();

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();

		// printf("FPS: %.10lf\n", 1 / delta_time);
		glGetError();
		

		#ifdef RENDERDOC
		if(rdoc_api) rdoc_api->EndFrameCapture(NULL, NULL);
		#endif
    }
 
    glfwTerminate();
    return 0;
}