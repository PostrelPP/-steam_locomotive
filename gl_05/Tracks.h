#pragma once
#include "Composite.h"
#include "Cube.h"

class Tracks :
	public Composite
{
public:
	Tracks(glm::vec3 coordinates, int tracksAmount, ShaderProgram* shader) {
		for (int i = 0; i < tracksAmount; i++) {
			Object* board = new Cube(coordinates + glm::vec3(0.0f, 0.0f, i * 2.0f), glm::vec3(2.10f, 0.15f, 0.65f), "oldWood.png", shader);
			items.push_back(board);
			Object* track1 = new Cube(coordinates + glm::vec3(0.85f, 0.18f, i * 2.0f), glm::vec3(0.25f, 0.2f, 2.0f), "black-steel.png", shader);
			items.push_back(track1);
			Object* track2 = new Cube(coordinates + glm::vec3(-0.85f, 0.18f, i * 2.0f), glm::vec3(0.25f, 0.2f, 2.0f), "black-steel.png", shader);
			items.push_back(track2);
		}
	}
};

class Road :
	public Composite
{
public:
	Road(glm::vec3 coordinates, int tracksAmount, ShaderProgram* shader) {
		for (int i = 0; i < tracksAmount; i++) {
			Object* road = new Cube(coordinates + glm::vec3(i * 20.0f, 0.0f, 0.0f), glm::vec3(20.0f, 0.1f, 5.0f), "asphalt1.png", shader);
			items.push_back(road);
		}
	}
};


