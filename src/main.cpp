#include "gl_env.h"

#include <cmath>
#include <cstring>
#include <iostream>
#include <functional>
#include <fstream>

#include "scene.h"
#include "randomtools.h"
#include "triangle.h"
#include "sphere.h"

#include "objloader.h"
#include "stb_image.h"

const int width = 512, height = 512;
const float C = 25;
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
		// bool success = loadOBJ("../../data/aranara.obj", &scene, 0, {-0.5, -0.5, -0.5}, {0.5, 0.5, 0.5});
		
		bool success = loadOBJ("../../data/bunny.obj", &scene, 0, {-0.5, -0.5, -0.5}, {0.5, 0.5, 0.5});

		assert(success);
	}

	scene.root = scene.buildBVH({});

    GLFWwindow* window;
 
    if (!glfwInit()) return -1;
 

    window = glfwCreateWindow(width, height, "New window", NULL, NULL);
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

		

	auto _glShaderSource =[&] (GLuint shader, GLsizei count, const char* path, const GLint *length, std::function<std::string(std::string)> F = NULL){
		std::string S="", s;
		std::ifstream prog(path);
		while(std::getline(prog, s)){ 
			S += s + "\n";
		}
		if(F) S = F(S);
		// std::cout<<S<<std::endl;
		char *S_content = new char[S.size()+10];
		memcpy(S_content, S.c_str(), S.size());
		S_content[S.size()] = 0;
		glShaderSource(shader, count, &S_content, length);
		delete S_content;
	};

		
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    _glShaderSource(vertex_shader, 1, "../../data/shaders/vertexshader.vs", NULL, [&](std::string s){
		// std::string screen_size_placeholder = "/*{screen_size}*/";
		// size_t pos_screen_size = s.find(screen_size_placeholder);
		// assert(pos_screen_size != std::string::npos);
		// s = s.replace(pos_screen_size, screen_size_placeholder.size(), std::to_string(width * height));
		return s;
	});
    glCompileShader(vertex_shader);
	GLint vsisCompiled = 0;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vsisCompiled);
	if(vsisCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(vertex_shader, maxLength, &maxLength, &errorLog[0]);

		std::cout << "Error compiling vertex shader: " << &errorLog[0] << std::endl;
		// Exit with failure.
		glDeleteShader(vertex_shader); // Don't leak the shader.
		exit(0);
	}


    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    _glShaderSource(fragment_shader, 1, "../../data/shaders/fragmentshader.fs", NULL);
    glCompileShader(fragment_shader);

	GLint fsisCompiled = 0;
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fsisCompiled);
	if(fsisCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(fragment_shader, maxLength, &maxLength, &errorLog[0]);

		std::cout << "Error compiling fragment shader: " << &errorLog[0] << std::endl;
		// Exit with failure.
		glDeleteShader(fragment_shader); // Don't leak the shader.
		exit(0);
	}

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

	{
		
		GLuint textureNames[4];
		glGenTextures(4, textureNames);

		unsigned char* data;
		int width, height, nrChannels;
		data = stbi_load("../../data/aranara_image.png", &width, &height, &nrChannels, 0);

		if (data != NULL) {
			assert(nrChannels == 4);

			glActiveTexture(GL_TEXTURE0); 
			glBindTexture(GL_TEXTURE_2D, textureNames[0]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glUniform1i(glGetUniformLocation(program, "imgTexture"), 0);

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
		GLuint tbo[3];
		glGenBuffers(3, tbo);

		glActiveTexture(GL_TEXTURE1);
		glBindBuffer(GL_TEXTURE_BUFFER, tbo[0]);
		glBufferData(GL_TEXTURE_BUFFER, scene.BVH_id * sizeof(BVHnode), &scene.t[1], GL_STATIC_DRAW);
		glUniform1i(glGetUniformLocation(program, "BVHnodes"), 1);
		glBindTexture(GL_TEXTURE_BUFFER, textureNames[1]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, tbo[0]);

		glActiveTexture(GL_TEXTURE2);
		glBindBuffer(GL_TEXTURE_BUFFER, tbo[1]);
		glBufferData(GL_TEXTURE_BUFFER, scene.triangle_id * sizeof(Triangle), &scene.triangle[0], GL_STATIC_DRAW);
		glUniform1i(glGetUniformLocation(program, "Triangles"), 2);
		glBindTexture(GL_TEXTURE_BUFFER, textureNames[2]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, tbo[1]);

		// glActiveTexture(GL_TEXTURE3);
		// glBindBuffer(GL_TEXTURE_BUFFER, tbo[2]);
		// glBufferData(GL_TEXTURE_BUFFER, width * height * 12, &screen, GL_DYNAMIC_DRAW);
		// glUniform1i(glGetUniformLocation(program, "Screen"), 3);
		// glBindTexture(GL_TEXTURE_BUFFER, textureNames[3]);
		// glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, tbo[2]);
	}

	// == end bind texture/bufferTexture

	puts("Start rendering loop...");

	const int SAMPLE_PER_FRAME = 1;

	int k = 0;
	
	int nsamples = 0;

    while (!glfwWindowShouldClose(window)) {
        static float passed_time = 0, passed_time_last, delta_time;
        passed_time_last = passed_time;
        passed_time = (float) glfwGetTime();
        delta_time = passed_time - passed_time_last;

		glfwSetWindowSize(window, width, height);
		int width , height;
		
		if(scene.handleCameraChange(window, delta_time, width, height)){
			glUniform1i(glGetUniformLocation(program, "u_clearScreen"), 1);
		}else{
			glUniform1i(glGetUniformLocation(program, "u_clearScreen"), 0);
		}

		++k;
		
		nsamples += SAMPLE_PER_FRAME;
		glUniform1i(glGetUniformLocation(program, "u_nsamples"), nsamples);

		glClearColor (0.0, 0.0, 0.0, 0.0); 
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(program);

		
		glUniform1i(glGetUniformLocation(program, "u_size_BVHnode"), sizeof(BVHnode) / 12);
		glUniform1i(glGetUniformLocation(program, "u_size_Triangle"), sizeof(Triangle) / 12);

		glUniform3fv(glGetUniformLocation(program, "u_campos"), 1, (GLfloat *)&scene.cam.getPos());
		glUniform3fv(glGetUniformLocation(program, "u_camdir"), 1, (GLfloat *)&scene.cam.getDir());
		glm::fvec3 _axisx = scene.cam.getAxisX() / scene.tg_alphaW;
		glUniform3fv(glGetUniformLocation(program, "u_camaxisX"), 1, (GLfloat *)&_axisx);
		glm::fvec3 _axisy = scene.cam.getAxisY() / scene.tg_alphaH;
		glUniform3fv(glGetUniformLocation(program, "u_camaxisY"), 1, (GLfloat *)&_axisy);
		glUniform1i(glGetUniformLocation(program, "u_width"), width);
		glUniform1i(glGetUniformLocation(program, "u_height"), height);
		

		for(int i=0; i<height; ++i){
			for(int j=0; j<width; ++j){
				glUniform2i(glGetUniformLocation(program, "u_position"), i, j);
				glBindVertexArray(VAO);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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