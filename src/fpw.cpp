#include "../lib/GLAD/glad.h"
#include <GLFW/glfw3.h>
#include "../lib/shader.h"
#include <iostream>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <regex.h>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"
#include "camera.h"
#include "SupportObjects.h"
using namespace std;
using namespace glm;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char * path);
int stepProject = 0;

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;

// camera
int nCols = 15, nRows = 15;
float cellSize = 100.0f;
glm::vec3 cameraPos = glm::vec3(0.0f, 70.0f, 0.0f);
glm::vec3 lightPos = glm::vec3((nRows / 2) * cellSize, 500.0f, (nCols / 2) * cellSize);
Camera cam(cameraPos);
bool firstMouse = true;
bool moved = false;
bool Pressed = true;

float lastX = SCR_WIDTH / 2.0;
float lastY = SCR_HEIGHT / 2.0;
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
float currentFrame;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "FPW", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	///////////////LOAD SHADER////////////////////////
	Shader lightShader("src/lightShader.vs", "src/lightShader.fs");
	////////////////////////////SHDAERS//////////////////////////////
	SupportObjects support;
	////////////////////////////floor///////////////////////////////////////////////

	vector<float> vertexFloor;
	vector<float> vertexTextures;
	vector<float> vertexNormal;
	vector<unsigned int> indexFloor;
	support.generateFloor(vertexFloor, vertexTextures, vertexNormal, cellSize, nCols, nRows);
	support.generateIndexFloor(indexFloor, nCols, nRows);
	unsigned int VAO, VBO, EBO, NORMAL, TEXTURES;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &NORMAL);
	glGenBuffers(1, &TEXTURES);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexFloor.size() * sizeof(float), &vertexFloor[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexFloor.size() * sizeof(int), &indexFloor[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) (0 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, NORMAL);
	glBufferData(GL_ARRAY_BUFFER, vertexNormal.size() * sizeof(float), &vertexNormal[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) (0 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, TEXTURES);
	glBufferData(GL_ARRAY_BUFFER, vertexTextures.size() * sizeof(float), &vertexTextures[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*) (0 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int textID = loadTexture("Data/floor.png");

	////////////////////////////floor///////////////////////////////////////////////

//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_ALWAYS);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	while (!glfwWindowShouldClose(window))
	{
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		lightShader.use();
		glm::mat4 modelL = glm::mat4();
		modelL = glm::translate(modelL, glm::vec3(1.0f, 0.0f, 1.0f));
		//glm::mat4 projectionProspective = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
		glm::mat4 projectionL = glm::perspective(glm::radians(cam.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
				10000.0f);
		glm::mat4 viewL = cam.GetViewMatrix();

		lightShader.setVec3("lightPos", lightPos);
		lightShader.setVec3("viewPos", cam.Position);
		lightShader.setMat4("projection", projectionL);
		lightShader.setMat4("view", viewL);
		lightShader.setMat4("model", modelL);
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textID);
		glDrawElements( GL_TRIANGLES, indexFloor.size(), GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
void processInput(GLFWwindow *window)
{


	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
	{
		cout << "camInfo2" << endl;
		cout << "yaw:" << cam.Yaw << "  " << "Pitch:" << cam.Pitch << endl;
		cout << cam.Position.x << "/" << cam.Position.y << "/" << cam.Position.z << endl;
		cout << cam.Front.x << "/" << cam.Front.y << "/" << cam.Front.z << endl;
		cout << cam.Up.x << "/" << cam.Up.y << "/" << cam.Up.z << endl;
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		cam.resetVisual();
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cam.ProcessKeyboard(FORWARD, deltaTime, nCols * cellSize, nRows * cellSize);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cam.ProcessKeyboard(BACKWARD, deltaTime, nCols * cellSize, nRows * cellSize);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cam.ProcessKeyboard(LEFT, deltaTime, nCols * cellSize, nRows * cellSize);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		cam.ProcessKeyboard(RIGHT, deltaTime, nCols * cellSize, nRows * cellSize);
		moved = true;
	}

}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
//	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
//	{

		if (firstMouse || moved)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
			moved = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;

		lastX = xpos;
		lastY = ypos;
		cam.ProcessMouseMovement(xoffset, yoffset);
//	}
}

unsigned int loadTexture(const char * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		stbi_set_flip_vertically_on_load(true);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
