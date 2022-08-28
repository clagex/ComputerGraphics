
#pragma once

// glm
#include <glm/glm.hpp>

// project
#include "opengl.hpp"
#include "basic_model.hpp"
#include "cgra/cgra_image.hpp"


// Main application class
//
class Application {
private:
	// window
	glm::vec2 m_windowsize;
	GLFWwindow *m_window;
	bool m_firstClick = true;
	float m_cursorX;
	float m_cursorY;
	float m_lastX = 0;
	float m_lastY = 0;

	// oribital camera
	float m_pitch = glm::radians(-45.0f);
	float m_yaw = 0;
	float m_distance = 35;
	glm::vec3 m_cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);

	// drawing flags
	bool m_show_axis = false;
	bool m_show_grid = false;
	bool m_showWireframe = false;

	// texture
	GLuint m_texture; // member variable to hold texture handle

	// basic model
	// contains a shader, a model transform
	// a mesh, and other model information (color etc.)
	basic_model m_model;

public:
	// setup
	Application(GLFWwindow *);

	// disable copy constructors (for safety)
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	// rendering callbacks (every frame)
	void render();
	void renderGUI();
	void updateVBO(int, glm::vec3[]);

	// input callbacks
	void cursorPosCallback(double xpos, double ypos);
	void mouseButtonCallback(int button, int action, int mods);
	void scrollCallback(double xoffset, double yoffset);
	void keyCallback(int key, int scancode, int action, int mods);
	void charCallback(unsigned int c);
};