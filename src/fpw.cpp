#include "../lib/GLAD/glad.h"
#include "../lib/model.h"
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
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"
#include "camera.h"
#include "coneGenerator.h"
#include "particle.h"
using namespace std;
using namespace glm;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window, SupportObjects& support);
unsigned int loadTexture(const char * path);
float searchMinTex(Model guard)
{
	float min = guard.meshes[0].vertices[0].Position.x;
	for (int i = 0; i < guard.meshes.size(); i++)
		for (int j = 0; j < guard.meshes[i].vertices.size(); j++)
		{
			if (guard.meshes[i].vertices[j].Position.x < min)
				min = guard.meshes[i].vertices[j].Position.x;
			if (guard.meshes[i].vertices[j].Position.z < min)
				min = guard.meshes[i].vertices[j].Position.z;

		}
	return min;
}
float scaleCone = 30.0f;
float scaleGuard = 5.0f;
float scaleCube = 5.0f;
// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;

// camera
int nCols = 14, nRows = 14;
float cellSize = 100.0f;
glm::vec3 cameraPos = glm::vec3(8.0f, 60.0f, 8.0f);
glm::vec3 lightPos = glm::vec3((nRows / 2) * cellSize, 30.0f, (nCols / 2) * cellSize);
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
	lightShader.use();
	lightShader.setInt("TEXTURE1", 0);

	//////MODELLO ALBANO
	Model guard("Data/nanosuit/nanosuit.obj");
	lightShader.use();
	lightShader.setInt("TEXTURE2", 1);
	float min = searchMinTex(guard);
	support.setGuardPosition(glm::vec3(lightPos.x, 0.75f, lightPos.z), (min * scaleGuard));

	////////MODELLO RUBIK
//	Model rubik("Data/cubeColored/Cube.obj");
////	Model rubik("Data/Game_block/cub.obj");
	Model aztec("Data/obj/objAztec.obj");
	lightShader.use();
	lightShader.setInt("TEXTURE5", 3);
	float min2 = searchMinTex(aztec);
	support.setCubePosition(glm::vec3(650, 10, 1100), (min2 * scaleCube));

	//////////CONE/////
	unsigned int VAOcone, VBOcone, NORMALcone, TEXTUREcone;
	Cone cone;
	int increment = 10;

	unsigned int nVertexCone = cone.dimensionCone(increment);
	float vertexCone[nVertexCone];
	float normalCone[nVertexCone];
	float textureCOne[cone.numberTextureCone(increment)];
	cone.createCone(vertexCone, normalCone, textureCOne, 5.00f, increment);
	glGenVertexArrays(1, &VAOcone);
	glGenBuffers(1, &VBOcone);
	glGenBuffers(1, &NORMALcone);
	glGenBuffers(1, &TEXTUREcone);
	glBindVertexArray(VAOcone);
	glBindBuffer(GL_ARRAY_BUFFER, VBOcone);
	glBufferData(GL_ARRAY_BUFFER, nVertexCone * sizeof(float), vertexCone, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, NORMALcone);
	glBufferData(GL_ARRAY_BUFFER, nVertexCone * sizeof(float), normalCone, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, TEXTUREcone);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCOne) * sizeof(float), textureCOne, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(2);

	unsigned int coneID = loadTexture("Data/cone.jpg");
	lightShader.use();
	lightShader.setInt("TEXTURE3", 2);
	support.setConePosition(glm::vec3(lightPos.x - 200.0f, -0.10f, lightPos.z - 100.0f), cone.radius * scaleCone);

	///////////////SPHERE
	unsigned int VBOsphere, VAOsphere, TEXTUREsphere;
	float radius = 130.25f;
	int precisionSphere = 200;
	Particle particle(radius, precisionSphere, precisionSphere);
	vector<float> vertexSphere;
	vector<float> textureSphere;
	particle.setVertex(vertexSphere, textureSphere);
//	support.addSpherePosition();
	glGenVertexArrays(1, &VAOsphere);
	glGenBuffers(1, &VBOsphere);
	glGenBuffers(1, &TEXTUREsphere);
	glBindVertexArray(VAOsphere);
	glBindBuffer(GL_ARRAY_BUFFER, VBOsphere);
	glBufferData(GL_ARRAY_BUFFER, vertexSphere.size() * sizeof(float), &vertexSphere[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, TEXTUREsphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * textureSphere.size(), &textureSphere[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(2);
	unsigned int sphereID = loadTexture("Data/sphere.jpg");
	lightShader.use();
	lightShader.setInt("TEXTURE4", 3);
	support.setSPherePosition(glm::vec3(lightPos.x + 200.0f, -0.10f, lightPos.z - 75.0f), particle.getRadius());

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	while (!glfwWindowShouldClose(window))
	{
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window, support);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//////////////DRAW FLOOR/////////////////
		lightShader.use();
		glm::mat4 modelL = glm::mat4();
		modelL = glm::translate(modelL, glm::vec3(1.0f, 0.0f, 1.0f));
		glm::mat4 projectionL = glm::perspective(glm::radians(cam.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 1.1f,
				10000.0f);
		glm::mat4 viewL = cam.GetViewMatrix();
		lightShader.setInt("textureSelected", 0);
		lightShader.setVec3("lightPos", lightPos);
		lightShader.setVec3("viewPos", cam.Position);
		lightShader.setMat4("projection", projectionL);
		lightShader.setMat4("view", viewL);
		lightShader.setMat4("model", modelL);
		glad_glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textID);
		glBindVertexArray(VAO);
		glDrawElements( GL_TRIANGLES, indexFloor.size(), GL_UNSIGNED_INT, 0);

		////////////////////////DRAW CONE///////////////
		lightShader.use();
		glm::mat4 modelCone;
		lightShader.setMat4("projection", projectionL);
		lightShader.setMat4("view", viewL);
		lightShader.setInt("textureSelected", 2);
		modelCone = glm::translate(modelCone, support.getConePosition());
		modelCone = glm::scale(modelCone, glm::vec3(scaleCone, scaleCone, scaleCone));
		lightShader.setMat4("model", modelCone);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, coneID);
		glBindVertexArray(VAOcone);
		glDrawArrays(GL_TRIANGLES, 0, nVertexCone);
		glBindTexture(GL_TEXTURE_2D, 0);

		//////////////////////DRAW GUARD
		lightShader.use();
		glm::mat4 modelGuard = glm::mat4();
		lightShader.setMat4("projection", projectionL);
		lightShader.setMat4("view", viewL);
		lightShader.setInt("textureSelected", 1);
		modelGuard = glm::translate(modelGuard, support.getGuardPosition());
		modelGuard = glm::scale(modelGuard, glm::vec3(5.0f, 5.0f, 5.0f));
		lightShader.setMat4("model", modelGuard);
		glActiveTexture(GL_TEXTURE1);
		guard.Draw(lightShader);
		glBindTexture(GL_TEXTURE_2D, 0);

		////////////////////////DRAW Sphere///////////////
		lightShader.use();
		glm::mat4 modelSPhere;
		lightShader.setMat4("projection", projectionL);
		lightShader.setMat4("view", viewL);
		lightShader.setInt("textureSelected", 3);
		modelSPhere = glm::translate(modelSPhere, support.getSpherePosition());
//		modelSPhere = glm::scale(modelCone, glm::vec3(20.0f, 20.0f, 20.0f));
		lightShader.setMat4("model", modelSPhere);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, sphereID);
		glBindVertexArray(VAOsphere);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexSphere.size());
		glBindTexture(GL_TEXTURE_2D, 0);

		//////////////////////DRAW AZTEC
		lightShader.use();
		glm::mat4 modelRubik = glm::mat4();
		lightShader.setMat4("projection", projectionL);
		lightShader.setMat4("view", viewL);
		lightShader.setInt("textureSelected", 4);
		modelRubik = glm::translate(modelRubik, support.getCubePosition());
		modelRubik = glm::scale(modelRubik, glm::vec3(5.0f, 5.0f, 5.0f));
		lightShader.setMat4("model", modelRubik);
		glActiveTexture(GL_TEXTURE4);
		aztec.Draw(lightShader);
		glBindTexture(GL_TEXTURE_2D, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteBuffers(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &NORMAL);
	glDeleteBuffers(1, &TEXTURES);
	glDeleteBuffers(1, &VAOcone);
	glDeleteBuffers(1, &VBOcone);
	glDeleteBuffers(1, &NORMALcone);
	glDeleteBuffers(1, &TEXTUREcone);
	glDeleteBuffers(1, &VAOsphere);
	glDeleteBuffers(1, &VBOsphere);
	glDeleteBuffers(1, &TEXTUREsphere);
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
void processInput(GLFWwindow *window, SupportObjects& support)
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
		cam.ProcessKeyboard(FORWARD, deltaTime, nCols * cellSize, nRows * cellSize, support);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cam.ProcessKeyboard(BACKWARD, deltaTime, nCols * cellSize, nRows * cellSize, support);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cam.ProcessKeyboard(LEFT, deltaTime, nCols * cellSize, nRows * cellSize, support);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		cam.ProcessKeyboard(ROTATELEFT, deltaTime, nCols * cellSize, nRows * cellSize, support);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		cam.ProcessKeyboard(ROTATERIGHT, deltaTime, nCols * cellSize, nRows * cellSize, support);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		cam.ProcessKeyboard(ROTATEUP, deltaTime, nCols * cellSize, nRows * cellSize, support);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		cam.ProcessKeyboard(ROTATEDOWN, deltaTime, nCols * cellSize, nRows * cellSize, support);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		cam.ProcessKeyboard(RIGHT, deltaTime, nCols * cellSize, nRows * cellSize, support);
		moved = true;
	}

}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
	{

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
	}
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
