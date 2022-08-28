
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
#include "bounding_box.hpp"
#include "cgra/cgra_geometry.hpp"
#include "cgra/cgra_gui.hpp"
#include "cgra/cgra_image.hpp"
#include "cgra/cgra_shader.hpp"
#include "cgra/cgra_wavefront.hpp"


using namespace std;
using namespace cgra;
using namespace glm;


#define instanceNum 101

Application::Application(GLFWwindow* window) : m_window(window) {

	// build the shader for the model
	shader_builder color_sb;
	color_sb.set_shader(GL_VERTEX_SHADER, CGRA_SRCDIR + std::string("//res//shaders//default_vert.glsl"));
	color_sb.set_shader(GL_FRAGMENT_SHADER, CGRA_SRCDIR + std::string("//res//shaders//default_frag.glsl"));
	// load and upload the texture
	cgra::rgba_image texture_data(CGRA_SRCDIR + std::string("//res//textures//texture.jpg"));
	m_texture = texture_data.uploadTexture();
	GLuint color_shader = color_sb.build();

	//completion, set up translations
	mat4 transforms[instanceNum] = { mat4(1.0f) };
	vec3 colors[instanceNum] = { vec3(1.0f, 0.0f, 0.0f) };
	for (unsigned int i = 1; i < instanceNum; i++) {
		vec3 transl = vec3(rand() % 100 - 50, rand() % 100 - 50, rand() % 100 - 50);
		float rot_angle = radians(rand() % 360 /1.);
		float scal = rand() / (RAND_MAX + 1.);
		vec3 col = vec3(rand() / (RAND_MAX + 1.), rand() / (RAND_MAX + 1.), rand() / (RAND_MAX + 1.));
		mat4 trans = mat4(1.0f);
		trans = translate(trans, transl);
		trans = rotate(trans, (float)(radians(rand() % 360 / 1.)), vec3(0.0, 1.0, 0.0));
		trans = rotate(trans, (float)(radians(rand() % 360 / 1.)), vec3(1.0, 0.0, 0.0));
		trans = rotate(trans, (float)(radians(rand() % 360 / 1.)), vec3(0.0, 0.0, 1.0));
		trans = scale(trans, vec3(scal, scal, scal));
		transforms[i] = trans;
		colors[i] = col;
	}

	// build the mesh for the model
	mesh_builder teapot_mb = load_wavefront_data(CGRA_SRCDIR + std::string("//res//assets//teapot_unwrapped.obj"));
	gl_mesh teapot_mesh = teapot_mb.build(instanceNum, transforms, colors);
	gl_mesh bounding_box_mesh = createBoundingBoxMesh(teapot_mesh.minv, teapot_mesh.maxv, instanceNum, transforms, colors);

	// put together an object
	m_model.shader = color_shader;
	m_model.mesh = teapot_mesh;
	m_model.num = instanceNum;
	m_model.modelTransform = glm::mat4(1);
	m_model.bounding_box = bounding_box_mesh;
	for (unsigned int i = 0; i < instanceNum; i++) {
		m_model.colors[i] = colors[i];
	}

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
	vec3 up = vec3(0.0f, 1.0f, 0.0f);
	mat4 trans = mat4(1.0f);
	vec4 pos = vec4(0.0, 0.0, 0.0, 1.0);
	trans = rotate(trans, m_yaw, vec3(0.0, 1.0, 0.0));
	trans = rotate(trans, m_pitch, vec3(1.0, 0.0, 0.0));
	trans = translate(trans, vec3(0.0, 0.0, m_distance));
	m_cameraPos = trans * pos;
	mat4 view = lookAt(m_cameraPos, m_cameraTarget, up);

	// draw options
	if (m_show_grid) cgra::drawGrid(view, proj);
	if (m_show_axis) cgra::drawAxis(view, proj);
	glPolygonMode(GL_FRONT_AND_BACK, (m_showWireframe) ? GL_LINE : GL_FILL);


	// draw the model
	m_model.draw(view, proj);


}


void Application::renderGUI() {

	// setup window
	ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 250), ImGuiSetCond_Once);
	ImGui::Begin("Camera", 0);

	// display current camera parameters
	ImGui::Text("Application %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::SliderFloat("Distance", &m_distance, 0, 100, "%.1f");
	ImGui::SliderAngle("Camera yaw", &m_yaw, -90, 90);
	ImGui::SliderAngle("Camera pitch", &m_pitch, -89, 89);
	ImGui::SliderFloat3("Model Color", value_ptr(m_model.colors[0]), 0, 1, "%.2f");
	ImGui::SliderFloat3("Specular Color", value_ptr(m_model.specColor), 0, 1, "%.2f");
	ImGui::SliderFloat("Shininess", &m_model.shininess, 1, 256, "%.2f");

	// extra drawing parameters
	ImGui::Checkbox("Show axis", &m_show_axis);
	ImGui::SameLine();
	ImGui::Checkbox("Show grid", &m_show_grid);
	ImGui::SameLine();
	ImGui::Checkbox("Show texture", &m_model.show_texture);
	ImGui::Checkbox("Wireframe", &m_showWireframe);
	ImGui::SameLine();
	if (ImGui::Button("Screenshot")) rgba_image::screenshot(true);

	// finish creating window
	ImGui::End();
}


void Application::cursorPosCallback(double xpos, double ypos) {
	m_cursorX = xpos;
	m_cursorY = ypos;
	if (!m_firstClick) {
		float xoffset = m_lastX - m_cursorX;
		float yoffset = m_lastY - m_cursorY;
		m_lastX = m_cursorX;
		m_lastY = m_cursorY;

		float sensitivity = 0.005f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		m_yaw += xoffset;
		m_pitch += yoffset;

		if (m_pitch > radians(89.0f)) m_pitch = radians(89.0f);
		if (m_pitch < radians(-89.0f)) m_pitch = radians(-89.0f);
	}
}


void Application::mouseButtonCallback(int button, int action, int mods) {
	if (button == 0) {
		if (action == 1 && m_firstClick) {
			m_lastX = m_cursorX;
			m_lastY = m_cursorY;
			m_firstClick = false;
		}
		if (action == 0) {
			m_firstClick = true;
		}
	}
}


void Application::scrollCallback(double xoffset, double yoffset) {
	m_distance -= (float)yoffset;
	if (m_distance < 0.0f) m_distance = 0;
	if (m_distance > 100.0f) m_distance = 100;
}


void Application::keyCallback(int key, int scancode, int action, int mods) {
	(void)key, (void)scancode, (void)action, (void)mods; // currently un-used
}


void Application::charCallback(unsigned int c) {
	(void)c; // currently un-used
}

void Application::updateVBO(int num, vec3 colors[]) {
	unsigned int colorsVBO;
	glGenBuffers(1, &colorsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * num, &colors[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}