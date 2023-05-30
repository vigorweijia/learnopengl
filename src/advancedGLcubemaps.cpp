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
	float cameraMovingSpeed = 2.0f * deltaTime;
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
	float yoffset = lastY - yPos; // 注意这里是相反的，因为y坐标是从底部往顶部依次增大的
	lastX = xPos;
	lastY = yPos;
	camera.ProcessMouseMovement(xoffset, yoffset);
}

static void ScrollCallBackCameraObj(GLFWwindow* window, double xOffset, double yOffset) {
	camera.ProcessMouseScroll(static_cast<float>(yOffset));
}


void advancedGL_cubemaps() {
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
	ShaderManager rcShader("../src/shaders/advancedGLcubemaps/reflectCubeVS.glsl", "../src/shaders/advancedGLcubemaps/reflectCubeFS.glsl");
	// ShaderManager modelShader("../src/shaders/advancedGLcubemaps/modelloadingVS.glsl", "../src/shaders/advancedGLcubemaps/modelloadingFS.glsl");
	// ShaderManager modelShader("../src/shaders/advancedGLcubemaps/modelReflectionVS.glsl", "../src/shaders/advancedGLcubemaps/modelReflectionFS.glsl");
	// ShaderManager modelShader("../src/shaders/advancedGLcubemaps/modelReflectionVS.glsl", "../src/shaders/advancedGLcubemaps/modelRefractionFS.glsl");
	ShaderManager modelShader("../src/shaders/advancedGLcubemaps/modelReflectionVS.glsl", "../src/shaders/advancedGLcubemaps/modelReflectTexturemapFS.glsl");
	ShaderManager skyboxShader("../src/shaders/advancedGLcubemaps/skyboxVS.glsl", "../src/shaders/advancedGLcubemaps/skyboxFS.glsl");

	/*---------------  load models  ---------------------*/
	Model ourModel("../resources/nanosuit_reflection/nanosuit.obj");

	/*--------- set vertex data and attributes -----------*/
	float reflectCubes[] = {
		// positions          // normals
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	// skyboxVAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// reflect cube
	unsigned int rcVAO, rcVBO;
	glGenVertexArrays(1, &rcVAO);
	glGenBuffers(1, &rcVBO);
	glBindVertexArray(rcVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rcVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(reflectCubes), &reflectCubes, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	/*-----------  load texture map  --------------------*/
	unsigned int cubeTexture = loadTexture("../resources/common/textures/container.jpg");
	unsigned int floorTexture = loadTexture("../resources/common/textures/metal.png");
	unsigned int cubemapTexture = loadCubemap({
		"../resources/common/textures/skybox/right.jpg",
		"../resources/common/textures/skybox/left.jpg",
		"../resources/common/textures/skybox/top.jpg",
		"../resources/common/textures/skybox/bottom.jpg",
		"../resources/common/textures/skybox/front.jpg",
		"../resources/common/textures/skybox/back.jpg"});

	// 貌似use shader的顺序会影响最终结果，不知道为什么

	/*---------------  render loop  ---------------------*/
	while (!glfwWindowShouldClose(window)) {
		/*----- process input ------*/
		processInputCameraObj(window);

		/*----- render command -----*/
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*------ draw cubes --------*/
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)800 / (float)600, 0.1f, 100.0f);

		// models
		modelShader.use();
		modelShader.setMat4("view", view);
		modelShader.setMat4("projection", projection);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		modelShader.setMat4("model", model);
		/* for reflection & refraction material
			modelShader.setVec3("cameraPos", camera.Position);
			modelShader.setInt("skybox", 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		*/
		///* for reflection texture map
		modelShader.setVec3("cameraPos", camera.Position);
		modelShader.setInt("skybox", 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		//*/
		ourModel.Draw(modelShader);
		glBindVertexArray(0);

		// reflect cube
		rcShader.use();
		rcShader.setInt("skybox", 0);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(1.0f, 1.0f, 1.0f));
		rcShader.setMat4("model", model);
		rcShader.setMat4("view", view);
		rcShader.setMat4("projection", projection);
		rcShader.setVec3("cameraPos", camera.Position);
		glBindVertexArray(rcVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		// draw skybox as last
		skyboxShader.use();
		skyboxShader.setInt("skybox", 0);
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to defaul

		/*----- double buffer ------*/
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	/*------------- de-allocate all resources ----------*/
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteVertexArrays(1, &rcVAO);
	glDeleteBuffers(1, &skyboxVBO);
	glDeleteBuffers(1, &rcVBO);

	glfwTerminate();
	exit(0);
}