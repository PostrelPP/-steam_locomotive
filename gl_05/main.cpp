#define GLEW_STATIC
using namespace std;
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include <iostream>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ShaderProgram.h"
#include "Primitive.h"
#include "Object.h"
#include "CylindricalPointLight.h"
#include "Camera.h"
#include "Floor.h"
#include "Tracks.h"
#include "Barrier.h"
#include "Car.h"
#include "Train.h"

const float MAX_FPS = 60.0f;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
int PointLight::pointLightsQuantity = 0;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	cout << key << endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void setUpShaders(ShaderProgram* shader, ShaderProgram* lampShader);
GLuint LoadMipmapTexture(GLuint texId, const char* fname) {
	int width, height;
	unsigned char* image = SOIL_load_image(fname, &width, &height, 0, SOIL_LOAD_RGB);
	if (image == nullptr)
		throw exception("Failed to load texture file");
	GLuint texture;
	glGenTextures(1, &texture);
	glActiveTexture(texId);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

Camera camera(glm::vec3(-1.0f, 2.0f, 10.0f));
float lastX = 800 / 2.0f;
float lastY = 600 / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
GLuint WIDTH;
GLuint HEIGHT;

glm::vec3 train_speed = { 0.0f,0.0f,0.0f };

void setUpShaders(ShaderProgram* shader, ShaderProgram* lampShader)
{
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();

	shader->Use();
	shader->setVec3("viewPos", camera.Position);
	shader->setMat4("projection", projection);
	shader->setMat4("view", view);

	lampShader->Use();
	lampShader->setMat4("projection", projection);
	lampShader->setMat4("view", view);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		train_speed.z += 0.0005f;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		train_speed.z -= 0.0005f;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; 
	lastX = xpos;
	lastY = ypos;
	camera.ProcessMouseMovement(xoffset, yoffset);
}

int main() {
	if (glfwInit() != GL_TRUE) {
		cout << "GLFW initialization failed" << endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	try {
		WIDTH = glfwGetVideoMode(glfwGetPrimaryMonitor())->width;
		HEIGHT = glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
		GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Parowoz", nullptr, nullptr);
		if (window == nullptr)
			throw exception("GLFW window not created");
		glfwMakeContextCurrent(window);
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
			throw exception("GLEW Initialization failed");
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, WIDTH, HEIGHT);
		ShaderProgram ourShader("sfp.vert", "sfp.frag");
		ShaderProgram lampShader("LampShader.vert", "LampShader.frag");

		Tracks tory(glm::vec3(0.0f, 0.0f, -50.0f), 100, &ourShader);
		Floor floor = Floor(glm::vec3(0, -0.1, 0), &ourShader);
		Road road = Road(glm::vec3(-200.0, -0.05, 15.0), 20, &ourShader);
		Barrier barrier = Barrier({ -2, 0.3, 18 }, &ourShader);

		Train train({ 0,0,1 }, &ourShader, &lampShader);
		Car* cars[2];
		const char* bodyTextures[] = { "car_sr.png", "car_cor.png" };
		for (int i = 0; i < 2; i++)
			cars[i] = new Car({ -6 - 6 * i, 0.6, 16.25 }, &ourShader, &lampShader, bodyTextures[i]);

		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			if (deltaTime < 1.0f / MAX_FPS)
				continue;
			lastFrame = currentFrame;
			processInput(window);

			glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			setUpShaders(&ourShader, &lampShader);

			floor.draw();
			tory.draw();
			for (int i = 0; i < 2; i++)
				cars[i]->draw();

			barrier.draw();
			road.draw();
			train.draw();
			train.move(train_speed);

			glfwSwapBuffers(window);
		}
	}
	catch (exception ex) {
		cout << ex.what() << endl;
	}
	glfwTerminate();
	return 0;
};