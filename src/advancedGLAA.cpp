#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdlib>
#include <filesystem>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "utils.h"
#include "ShaderManager.h"
#include "Camera.h"
#include "model.h"

using namespace std;

// timing
static float deltaTime = 0.0f;
static float lastFrame = 0.0f;

// camera
static Camera camera;
static float lastX = 400.0f, lastY = 300.0f;
static bool mouseFirst = true;

// lighting
// static glm::vec3 lightSourcePos(1.2f, 1.0f, 2.0f);

static void processInputCameraObj(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
	// if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		// camera.ProcessKeyboard(Camera_Movement::UP, deltaTime);
	// if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		// camera.ProcessKeyboard(Camera_Movement::DOWN, deltaTime);
}

static void MouseCallBackCameraObj(GLFWwindow* window, double x, double y) {
	float xPos = static_cast<float>(x);
	float yPos = static_cast<float>(y);
	if (mouseFirst) {
		lastX = xPos;
		lastY = yPos;
		mouseFirst = false;
	}
	float xoffset = xPos - lastX;
	float yoffset = lastY - yPos; // ע���������෴�ģ���Ϊy�����Ǵӵײ����������������
	lastX = xPos;
	lastY = yPos;
	camera.ProcessMouseMovement(xoffset, yoffset);
}

static void ScrollCallBackCameraObj(GLFWwindow* window, double xOffset, double yOffset) {
	camera.ProcessMouseScroll(static_cast<float>(yOffset));
}


void advancedGL_anti_aliasing() {
	/*-------------  glfw initialization   --------------*/
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif 
	glfwWindowHint(GLFW_SAMPLES, 10); // use Multisample Buffer with 4 samples

	/*--------------  create window  --------------------*/
	GLFWwindow* window = glfwCreateWindow(800, 600, "learnopengl", NULL, NULL);
	if (window == nullptr) {
		cout << "Failed to create window." << endl;
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window);

	/*-------- glad manager of opengl function ptr ------*/
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to initialize GLAD." << endl;
		exit(-1);
	}

	/*---------------  set viewport ---------------------*/
	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	/*----------- set mouse move callback ---------------*/
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, MouseCallBackCameraObj);

	/*----------- set mouse scroll callback -------------*/
	glfwSetScrollCallback(window, ScrollCallBackCameraObj);

	/*--------------  enable z-buffer  ------------------*/
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	/*-------------  use shader manager  ----------------*/
	ShaderManager shader("../src/shaders/advancedGLAA/aaVS.glsl", "../src/shaders/advancedGLAA/aaFS.glsl");

	/*--------- set vertex data and attributes -----------*/
	float cubeVertices[] = {
		// Positions       
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f
	};
	// Setup cube VAO
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);


	/*---------------  render loop  ---------------------*/
	while (!glfwWindowShouldClose(window)) {
		/*----- process input ------*/
		processInputCameraObj(window);

		/*----- render command -----*/
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*------ draw pointes ------*/
		shader.use();
		glm::mat4 projection = glm::perspective(camera.Zoom, (float)800.0f / (float)600.0f, 0.1f, 1000.0f);
		shader.setMat4("projection", projection);
		shader.setMat4("view", camera.GetViewMatrix());
		shader.setMat4("model", glm::mat4(1.0f));

		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		/*----- double buffer ------*/
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	/*------------- de-allocate all resources ----------*/
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);

	glfwTerminate();
	exit(0);
}

void advancedGL_offscreen() {
	/*-------------  glfw initialization   --------------*/
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif 

	/*--------------  create window  --------------------*/
	GLFWwindow* window = glfwCreateWindow(800, 600, "learnopengl", NULL, NULL);
	if (window == nullptr) {
		cout << "Failed to create window." << endl;
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window);

	/*-------- glad manager of opengl function ptr ------*/
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to initialize GLAD." << endl;
		exit(-1);
	}

	/*---------------  set viewport ---------------------*/
	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	/*----------- set mouse move callback ---------------*/
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, MouseCallBackCameraObj);

	/*----------- set mouse scroll callback -------------*/
	glfwSetScrollCallback(window, ScrollCallBackCameraObj);

	/*--------------  enable z-buffer  ------------------*/
	glEnable(GL_DEPTH_TEST);

	/*-------------  use shader manager  ----------------*/
	ShaderManager shader("../src/shaders/advancedGLAA/aaVS.glsl", "../src/shaders/advancedGLAA/aaFS.glsl");
	ShaderManager screenShader("../src/shaders/advancedGLAA/screenVS.glsl", "../src/shaders/advancedGLAA/screenFS.glsl");

	/*--------- set vertex data and attributes -----------*/
	float cubeVertices[] = {
		// positions       
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f
	};
	float quadVertices[] = {   // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};
	// setup cube VAO
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// setup screen VAO
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


	// configure MSAA framebuffer
	// --------------------------
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// create a multisampled color attachment texture
	unsigned int textureColorBufferMultiSampled;
	glGenTextures(1, &textureColorBufferMultiSampled);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, 800.0, 600.0, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);
	// create a (also multisampled) renderbuffer object for depth and stencil attachments
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, 800.0, 600.0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// configure second post-processing framebuffer
	unsigned int intermediateFBO;
	glGenFramebuffers(1, &intermediateFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);
	// create a color attachment texture
	unsigned int screenTexture;
	glGenTextures(1, &screenTexture);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800.0, 600.0, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);	// we only need a color buffer

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// shader configuration
	// --------------------
	screenShader.use();
	screenShader.setInt("screenTexture", 0);


	/*---------------  render loop  ---------------------*/
	while (!glfwWindowShouldClose(window)) {
		/*----- process input ------*/
		processInputCameraObj(window);

		/*----- render command -----*/
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1. draw scene as normal in multisampled buffers
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		// set transformation matrices		
		shader.use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)800.0 / (float)600.0, 0.1f, 1000.0f);
		shader.setMat4("projection", projection);
		shader.setMat4("view", camera.GetViewMatrix());
		shader.setMat4("model", glm::mat4(1.0f));

		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// 2. now blit multisampled buffer(s) to normal colorbuffer of intermediate FBO. Image is stored in screenTexture
		glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
		glBlitFramebuffer(0, 0, 800.0, 600.0, 0, 0, 800.0, 600.0, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		// 3. now render quad with scene's visuals as its texture image
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		// draw Screen quad
		screenShader.use();
		glBindVertexArray(quadVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, screenTexture); // use the now resolved color attachment as the quad's texture
		glDrawArrays(GL_TRIANGLES, 0, 6);

		/*----- double buffer ------*/
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	/*------------- de-allocate all resources ----------*/
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);

	glfwTerminate();
	exit(0);
}