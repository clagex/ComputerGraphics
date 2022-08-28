
#pragma once

// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// project
#include "opengl.hpp"
#include "cgra/cgra_mesh.hpp"
#include "skeleton_model.hpp"


// Basic model that holds the shader, mesh and transform for drawing.
// Can be copied and modified for adding in extra information for drawing
// including textures for texture mapping etc.
struct basic_model {
	GLuint shader = 0;
	cgra::gl_mesh mesh;
	glm::vec3 color{0.7};
	glm::mat4 modelTransform{1.0};
    glm::vec3 lightPos{3.f, 5.f, 3.f};
	GLuint texture;

    float roughness = 0.8;
    float diffRoughness = 0.8;
    // 0 = default, 1 = cook-torrance, 2 = oren-nayar
    int render_mode = 1;
    float refAtNormIncidence = 0.5;
    float albedo = 2.0;
    bool showTex = false;
    bool showNorm = false;

	void draw(const glm::mat4 &view, const glm::mat4 proj);
    void light_space_transform();
};


// Main application class
//
class Application {
private:
	// window
	glm::vec2 m_windowsize;
	GLFWwindow *m_window;

	// oribital camera
	float m_pitch = .86;
	float m_yaw = -.86;
	float m_distance = 20;
	glm::vec2 m_cam_pos;

	// last input
	bool m_leftMouseDown = false;
	bool m_rightMouseDown = false;
	bool m_middleMouseDown = false;
	glm::vec2 m_mousePosition;

	// drawing flags
	bool m_show_axis = false;
	bool m_show_grid = false;
	bool m_showWireframe = false;

	// geometry
	basic_model m_model;
    // sphere lat long
	int m_elevation = 24;
	int m_azimuthal = 24;
    // sphere from cube
	int m_subdivision = 1;
    // torus
    int m_torus_u = 24;
    int m_torus_v = 24;
    float m_torus_c = 3;
    float m_torus_a = 1.5;
	// 0 = latlong, 1 = cube to sphere, 2 = torus
	int m_currentModel = 0;
    bool m_update = true;

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

	// Part 1
	cgra::mesh_builder sphere_latlong();
	cgra::mesh_builder sphere_from_cube();
    cgra::mesh_builder torus_latlong();
	int get_sphere_index(int i, int j);
    int get_cube_index(int offset, int j, int k) const;
    int get_torus_index(int j, int k) const;

    // part 2
    static void setShadowPass();


    //question
    void question();
};