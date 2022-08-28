
#pragma once

// glm
#include <glm/glm.hpp>

// project
#include "opengl.hpp"
#include "triangle.hpp"
#include "objfile.h"


// Main application class
//
class Application {
private:
	// window
	glm::vec2 m_windowsize;
	GLFWwindow *m_window;

	// basic shader
	GLuint m_shader;
	glm::vec3 m_color = glm::vec3(0.066, 0.341, 0.215);
	glm::vec3 m_light = glm::vec3(0, 0, -1);

	// obj model
	ObjFile m_obj;

public:
	// setup
	Application(GLFWwindow *);

	// disable copy constructors (for safety)
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	// rendering callbacks (every frame)
	void render();
	void renderGUI();

	// input callbacks
	void cursorPosCallback(double xpos, double ypos);
	void mouseButtonCallback(int button, int action, int mods);
	void scrollCallback(double xoffset, double yoffset);
	void keyCallback(int key, int scancode, int action, int mods);
	void charCallback(unsigned int c);
};