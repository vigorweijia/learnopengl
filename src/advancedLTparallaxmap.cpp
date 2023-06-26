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

// turn on/off gamma correction
static bool gammaKeyPressed = false;
static bool useGammaCorrection = false;

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

static bool shadows = true;

static unsigned int cubeVAO = 0, cubeVBO;
static void renderCube();

static unsigned int planeVAO = 0;
static void renderScene(const ShaderManager& shader);

static unsigned int quadVAO = 0, quadVBO;
static void renderQuad();

static constexpr float heightScale = 0.1;

void advancedLT_Parallax_Mapping() {
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
	// ShaderManager shader("../src/shaders/advancedLTparallaxmap/parallaxVS.glsl", "../src/shaders/advancedLTparallaxmap/parallaxFS.glsl");
	// ShaderManager shader("../src/shaders/advancedLTparallaxmap/parallaxVS.glsl", "../src/shaders/advancedLTparallaxmap/steepParallaxFS.glsl");
	ShaderManager shader("../src/shaders/advancedLTparallaxmap/parallaxVS.glsl", "../src/shaders/advancedLTparallaxmap/occlusionParallaxFS.glsl");

	/*--------------  load texture  ---------------------*/
	// unsigned int diffuseMap = loadTexture("../resources/common/textures/bricks2.jpg");
	// unsigned int normalMap = loadTexture("../resources/common/textures/bricks2_normal.jpg");
	// unsigned int heightMap = loadTexture("../resources/common/textures/bricks2_disp.jpg");
	unsigned int diffuseMap = loadTexture("../resources/common/textures/wood.png");
	unsigned int normalMap = loadTexture("../resources/common/textures/toy_box_normal.png");
	unsigned int heightMap = loadTexture("../resources/common/textures/toy_box_disp.png");

	/*----------------- use shaders ---------------------*/
	shader.use();
	shader.setInt("diffuseMap", 0);
	shader.setInt("normalMap", 1);
	shader.setInt("depthMap", 2);

	/*---------------  light pos  -----------------------*/
	glm::vec3 lightPos(0.5f, 1.0f, 0.3f);

	/*---------------  render loop  ---------------------*/
	while (!glfwWindowShouldClose(window)) {
		/*----- process input ------*/
		processInputCameraObj(window);

		/*----- render command -----*/
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// configure view/projection matrices
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)800.0 / (float)600.0, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shader.use();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		// render normal-mapped quad
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
		shader.setMat4("model", model);
		shader.setVec3("viewPos", camera.Position);
		shader.setVec3("lightPos", lightPos);
		shader.setFloat("heightScale", heightScale);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, heightMap);
		renderQuad();

		// render light source (simply re-renders a smaller plane at the light's position for debugging/visualization)
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.1f));
		shader.setMat4("model", model);
		renderQuad();


		/*----- double buffer ------*/
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	/*------------- de-allocate all resources ----------*/

	glfwTerminate();
	exit(0);
}

static void renderQuad() {
	if (quadVAO == 0)
	{
		// positions
		glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
		glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
		glm::vec3 pos3(1.0f, -1.0f, 0.0f);
		glm::vec3 pos4(1.0f, 1.0f, 0.0f);
		// texture coordinates
		glm::vec2 uv1(0.0f, 1.0f);
		glm::vec2 uv2(0.0f, 0.0f);
		glm::vec2 uv3(1.0f, 0.0f);
		glm::vec2 uv4(1.0f, 1.0f);
		// normal vector
		glm::vec3 nm(0.0f, 0.0f, 1.0f);

		// calculate tangent/bitangent vectors of both triangles
		glm::vec3 tangent1, bitangent1;
		glm::vec3 tangent2, bitangent2;
		// triangle 1
		// ----------
		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent1 = glm::normalize(tangent1);

		bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent1 = glm::normalize(bitangent1);

		// triangle 2
		// ----------
		edge1 = pos3 - pos1;
		edge2 = pos4 - pos1;
		deltaUV1 = uv3 - uv1;
		deltaUV2 = uv4 - uv1;

		f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent2 = glm::normalize(tangent2);


		bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent2 = glm::normalize(bitangent2);


		float quadVertices[] = {
			// positions            // normal         // texcoords  // tangent                          // bitangent
			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
		};
		// configure plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}