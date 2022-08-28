
// std
#include <iostream>
#include <string>
#include <chrono>

// glm
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// project
#include "application.hpp"
#include "cgra/cgra_gui.hpp"
#include "cgra/cgra_shader.hpp"
#include "objfile.h"


using namespace std;
using namespace cgra;
using namespace glm;


Application::Application(GLFWwindow *window) : m_window(window) {
	
	// build the shader
	shader_builder color_sb;
	color_sb.set_shader(GL_VERTEX_SHADER, CGRA_SRCDIR + std::string("//res//shaders//default_vert.glsl"));
	color_sb.set_shader(GL_FRAGMENT_SHADER, CGRA_SRCDIR + std::string("//res//shaders//default_frag.glsl"));
	m_shader = color_sb.build(); 

	// load the teapot.obj file and build the model
	m_obj.loadOBJ(CGRA_SRCDIR + std::string("//res//assets//teapot.obj"));
	m_obj.build();
}


void Application::render() {
	
	// retrieve the window hieght
	int width, height;
	glfwGetFramebufferSize(m_window, &width, &height); 

	m_windowsize = vec2(width, height); // update window size
	glViewport(0, 0, width, height); // set the viewport to draw to the entire window

	// clear the back-buffer
	glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	// enable flags for normal/forward rendering
	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LESS);

	// calculate the projection and view matrix
	mat4 proj = perspective(1.f, float(width) / height, 0.1f, 1000.f);
	vec3 cameraPos = vec3(2, 2, 3);
	vec3 objPos = vec3(1, 2, 5);
	vec3 camDir = normalize(cameraPos - objPos);
	vec3 up = vec3(0, 1, 0);
	vec3 camRight = cross(up, camDir);
	vec3 camUp = cross(camDir, camRight);

	mat4 view = lookAt(cameraPos, objPos, up);

	// set shader and upload variables
	glUseProgram(m_shader);
	glUniformMatrix4fv(glGetUniformLocation(m_shader, "uProjectionMatrix"), 1, false, value_ptr(proj));
	glUniformMatrix4fv(glGetUniformLocation(m_shader, "uModelViewMatrix"), 1, false, value_ptr(view));
	glUniform3f(glGetUniformLocation(m_shader, "uShaderColor"), m_color.r, m_color.g, m_color.b);
	glUniform3f(glGetUniformLocation(m_shader, "uLightDirection"), m_light.x, m_light.y, m_light.z);

	// draw the model
	m_obj.draw();
}


void Application::renderGUI() {

	// setup window
	ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2(500, 150), ImGuiSetCond_Once);
	ImGui::Begin("Mesh loader", 0);

	// Loading buttons
	static char filename[512] = "";
	ImGui::InputText("", filename, 512);
	ImGui::SameLine();
	if (ImGui::Button("Load")) {
		// load mesh from 'filename'
		m_obj.destroy();
		m_obj.loadOBJ(filename);
		m_obj.build();
	}

	ImGui::SameLine();
	if (ImGui::Button("Print")) {
		// print mesh data
		m_obj.printMeshData();
	}

	ImGui::SameLine();
	if (ImGui::Button("Unload")) {
		// unload mesh
		m_obj.destroy();
	}

	ImGui::NewLine();
	if (ImGui::ColorEdit3("Shader Color", value_ptr(m_color))) {
		// update the color rgb values into following member variable
	}

	ImGui::NewLine();
	if (ImGui::DragFloat3("Lighting Direction", value_ptr(m_light))) {
		// update the lighting direction into following member variable
	}

	// finish creating window
	ImGui::End();
}


void Application::cursorPosCallback(double xpos, double ypos) {
	(void)xpos, ypos; // currently un-used
}


void Application::mouseButtonCallback(int button, int action, int mods) {
	(void)button, action, mods; // currently un-used
}


void Application::scrollCallback(double xoffset, double yoffset) {
	(void)xoffset, yoffset; // currently un-used
}


void Application::keyCallback(int key, int scancode, int action, int mods) {
	(void)key, (void)scancode, (void)action, (void)mods; // currently un-used
}


void Application::charCallback(unsigned int c) {
	(void)c; // currently un-used
}