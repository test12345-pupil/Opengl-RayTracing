#include "gl_env.h"
#include <omp.h>

#include <cmath>
#include <cstring>
#include <iostream>
#include <fstream>

#include "scene.h"
#include "randomtools.h"
#include "triangle.h"
#include "sphere.h"

#include "texture_image.h"
#include "objloader.h"

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

	
const TextureImage::Texture *texture;


void _glShaderSource (GLuint shader, GLsizei count, const char* path, const GLint *length){
	std::string S="", s;
	std::ifstream prog(path);
	while(std::getline(prog, s)){ 
		S += s + '\n';
	}
	char *S_content = new char[S.size()+10];
	memcpy(S_content, S.c_str(), S.size());
	S_content[S.size()] = 0;
	glShaderSource(shader, count, &S_content, length);
	delete S_content;
}


int main(void) {

	// == begin 设置相机
	scene.cam.setPosition(0,0,0.8);
	scene.cam.setDirection(0,0.1,-1);

	scene.alphaW = M_PI / 4;
	scene.alphaH = scene.alphaW / WIDTH * HEIGHT;
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

	
	texture = &TextureImage::Texture::loadTexture("aranara_image", "aranara_image.png");

	{
		bool success = loadOBJ("../../data/aranara.obj", &scene, 0, {-0.5, -0.5, -0.5}, {0.5, 0.5, 0.5});
		
		// bool success = loadOBJ("../../data/bunny.obj", &scene, 0, {-0.5, -0.5, -0.5}, {0.5, 0.5, 0.5});

		assert(success);
	}

	scene.root = scene.buildBVH({});

    GLFWwindow* window;
 
    if (!glfwInit()) return -1;
 

    window = glfwCreateWindow(WIDTH, HEIGHT, "New window", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
 
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

	
	// == begin bind shader programs

    GLuint vertex_shader, fragment_shader, program;

    if (glewInit() != GLEW_OK)
        exit(EXIT_FAILURE);
		
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    _glShaderSource(vertex_shader, 1, "../../data/shaders/vertexshader.vs", NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    _glShaderSource(fragment_shader, 1, "../../data/shaders/fragmentshader.fs", NULL);
    glCompileShader(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
	

    int linkStatus;
    if (glGetProgramiv(program, GL_LINK_STATUS, &linkStatus), linkStatus == GL_FALSE){
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        std::cout << "Error occured in glLinkProgram():" << &infoLog[0] << std::endl;
		exit(0);
	}
	// == end bind shader programs


	// == begin bind VAO/VBO/EBO
	glBindVertexArray(0);

	float vertices[] = { 
         1.0f,  1.0f,    1.0f, 1.0f, // top right
         1.0f, -1.0f,    1.0f, 0.0f, // bottom right
        -1.0f, -1.0f,    0.0f, 0.0f, // bottom left
        -1.0f,  1.0f,    0.0f, 1.0f  // top left 
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

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// == end bind VAO/VBO/EBO

	// == begin bind tbo

	GLuint tbo;
	glGenBuffers(1, &tbo);
	glBindBuffer(GL_TEXTURE_BUFFER, tbo);

	// == end bind tbo

	const int SAMPLE_PER_FRAME = 1, D = 3;

	int k = 0;
	
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


		for(int i=k % D; i<HEIGHT; i += D){
			for(int j=k / D % D; j<WIDTH; j += D){
				for(int id=0; id<SAMPLE_PER_FRAME; ++id){
					screen[i][j]+=scene.sampleOnce(1.0*i/HEIGHT + randf(0, 1.0/HEIGHT), 1.0*j/WIDTH + randf(0, 1.0/WIDTH));
				}
			}
		}

		++k;
		
		nsamples += SAMPLE_PER_FRAME;

		glClearColor (0.0, 0.0, 0.0, 0.0); 
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(program);
		glBindVertexArray(VAO);

		glUniform1f(glGetUniformLocation(program, "u_width"), WIDTH);
		glUniform1f(glGetUniformLocation(program, "u_height"), HEIGHT);

		auto drawPixel = [&](int i, int j, float r, float g, float b){
			glm::vec2 pos(i, j);
			glm::vec3 col(r*C,g*C,b*C);
			glUniform2fv(glGetUniformLocation(program, "u_position"), 1, (const GLfloat*)&pos);
			glUniform3fv(glGetUniformLocation(program, "u_color"), 1, (const GLfloat*)&col);
        	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		};

		for(int i=0; i<HEIGHT; ++i){
			for(int j=0; j<WIDTH; ++j){
				drawPixel(i, j, screen[i][j].x / nsamples * D * D, screen[i][j].y / nsamples * D * D, screen[i][j].z / nsamples * D * D);
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