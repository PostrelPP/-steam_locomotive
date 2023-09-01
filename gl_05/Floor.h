#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include <iostream>
#include "ShaderProgram.h"

class Floor :
	public Primitive
{
public: 
	float const size = 500;
	int const textureMultiplicity = 20;
	Floor(glm::vec3 coordinates, ShaderProgram* shader) : Primitive(coordinates, glm::vec3(0.0, 0.0, 0.0), "grass.png", shader) {
		init("grass.png");
	}

	void prepareVertices() override {
		GLfloat vert[] = {-.5f*size,-.0f,-.5f * size, .5f * size,-.0f,-.5f * size, .5f * size,-.0f, .5f * size, -.5f * size,-.0f, .5f * size};

		GLfloat normals[] = {0,1,0, 0,1, 0, 0,1, 0, 0,1, 0, };

		GLfloat texCoords[] = {0, textureMultiplicity, textureMultiplicity, textureMultiplicity, textureMultiplicity, 0, 0, 0 };

		size_t elements = _countof(vert) + _countof(normals) + _countof(texCoords);

		int texIndex = 0;
		for (int i = 0; i < elements; i = i + 3) {
			vertices.push_back(vert[i]);
			vertices.push_back(vert[i + 1]);
			vertices.push_back(vert[i + 2]);
			vertices.push_back(normals[i]);
			vertices.push_back(normals[i + 1]);
			vertices.push_back(normals[i + 2]);
			vertices.push_back(texCoords[texIndex]);
			vertices.push_back(texCoords[texIndex + 1]);
			texIndex += 2;
		}
	}

	void prepareIndices() override {
		GLuint ind[] = { 0, 1, 2, 2, 3, 0 };

		for (int i = 0; i < 6; i++)
			indices.push_back(ind[i]);
	}
};
