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
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::DOWN, deltaTime);
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


void lighting_multilightsource() {
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
	ShaderManager materialShader("../src/shaders/lightingmultishader/cubeVS.glsl", "../src/shaders/lightingmultishader/cubeMultiLightFS.glsl");
	ShaderManager lightSourceShader("../src/shaders/lightingmultishader/lightsourceVS.glsl", "../src/shaders/lightingmultishader/lightsourceFS.glsl");
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	/*--------- set vertex data and attributes -----------*/
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	unsigned int cubeVAO, VBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/*-----------  load texture map  --------------------*/
	unsigned int diffuseMap = loadTexture("../src/container2.png");
	unsigned int specularMap = loadTexture("../src/lighting_maps_specular_color.png");
	materialShader.use();
	materialShader.setInt("material.diffuse", 0);
	materialShader.setInt("material.specular", 1);

	/*---------------  render loop  ---------------------*/
	while (!glfwWindowShouldClose(window)) {
		/*----- process input ------*/
		processInputCameraObj(window);

		/*----- render command -----*/
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*- view & projection matrix -*/
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		/*------- light color ------*/
		glm::vec3 lightColor(1.0f);
		glm::vec3 diffuseColor = lightColor * 0.4f;
		glm::vec3 ambientColor = lightColor * 0.05f;
		float constance = 1.0f;
		float linear = 0.09f;
		float quadratic = 0.0032f;

		/*----- light source -------*/
		lightSourceShader.use();
		lightSourceShader.setVec3("lightColor", lightColor);
		lightSourceShader.setMat4("projection", projection);
		lightSourceShader.setMat4("view", view);
		for (int i = 0; i < 4; i++) {
			glm::mat4 model(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
			lightSourceShader.setMat4("model", model);
			glBindVertexArray(lightVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		/*------ cube object -------*/
		glm::vec3 objectColor(1.0f, 0.5f, 0.31f);
		materialShader.use();
		// Directional Light
		materialShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		materialShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		materialShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		materialShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// Point Light
		materialShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		materialShader.setFloat("pointLights[0].constance", constance);
		materialShader.setFloat("pointLights[0].linear", linear);
		materialShader.setFloat("pointLights[0].quadratic", quadratic);
		materialShader.setVec3("pointLights[0].ambient", ambientColor);
		materialShader.setVec3("pointLights[0].diffuse", diffuseColor);
		materialShader.setVec3("pointLights[0].specular", lightColor);

		materialShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		materialShader.setFloat("pointLights[1].constance", constance);
		materialShader.setFloat("pointLights[1].linear", linear);
		materialShader.setFloat("pointLights[1].quadratic", quadratic);
		materialShader.setVec3("pointLights[1].ambient", ambientColor);
		materialShader.setVec3("pointLights[1].diffuse", diffuseColor);
		materialShader.setVec3("pointLights[1].specular", lightColor);

		materialShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		materialShader.setFloat("pointLights[2].constance", constance);
		materialShader.setFloat("pointLights[2].linear", linear);
		materialShader.setFloat("pointLights[2].quadratic", quadratic);
		materialShader.setVec3("pointLights[2].ambient", ambientColor);
		materialShader.setVec3("pointLights[2].diffuse", diffuseColor);
		materialShader.setVec3("pointLights[2].specular", lightColor);

		materialShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		materialShader.setFloat("pointLights[3].constance", constance);
		materialShader.setFloat("pointLights[3].linear", linear);
		materialShader.setFloat("pointLights[3].quadratic", quadratic);
		materialShader.setVec3("pointLights[3].ambient", ambientColor);
		materialShader.setVec3("pointLights[3].diffuse", diffuseColor);
		materialShader.setVec3("pointLights[3].specular", lightColor);
		// Spotlight
		materialShader.setVec3("spotLight.position", camera.Position);
		materialShader.setVec3("spotLight.direction", camera.Front);
		materialShader.setFloat("spotLight.cutoff", glm::cos(glm::radians(12.5f)));
		materialShader.setFloat("spotLight.outerCutoff", glm::cos(glm::radians(17.5f)));
		materialShader.setFloat("spotLight.constance", 1.0f);
		materialShader.setFloat("spotLight.linear", 0.09f);
		materialShader.setFloat("spotLight.quadratic", 0.0032f);
		materialShader.setVec3("spotLight.ambient", ambientColor);
		materialShader.setVec3("spotLight.diffuse", diffuseColor);
		materialShader.setVec3("spotLight.specular", lightColor);

		materialShader.setVec3("viewPos", camera.Position);
		materialShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
		materialShader.setFloat("material.shininess", 64.0f);
		// Bind texture map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		// Bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		materialShader.setMat4("projection", projection);
		materialShader.setMat4("view", view);
		for (int i = 0; i < 10; i++) {
			glm::mat4 model(1.0f);
			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model, glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
			materialShader.setMat4("model", model);
			glBindVertexArray(cubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		/*----- double buffer ------*/
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	/*------------- de-allocate all resources ----------*/
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	exit(0);
}