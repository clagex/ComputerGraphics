
#pragma once

//std
#include <vector>

// glm
#include <glm/glm.hpp>

// project
#include "cgra/cgra_mesh.hpp"
#include "cgra/cgra_shader.hpp"


// boid class (forward declaration)
class Boid;

class Scene {
private:
	// opengl draw data
	GLuint m_color_shader = 0;
	GLuint m_aabb_shader = 0;
	GLuint m_skymap_shader = 0;
	cgra::gl_mesh m_simple_boid_mesh;
	cgra::gl_mesh m_boid_mesh;
	cgra::gl_mesh m_predator_mesh;
	cgra::gl_mesh m_sphere_mesh;

	// draw status
	bool m_show_aabb = true;
	bool m_show_axis = false;
	bool m_show_skymap = false;

	// scene data
	glm::vec3 m_bound_hsize = glm::vec3(20);
	std::vector<Boid> m_boids;
	std::vector<Boid> m_objs;


	//-------------------------------------------------------------
	// [Assignment 3] :
	// Create variables for keeping track of the boid parameters
	// such as min and max speed etc. These paramters can either be
	// public, or private with getter functions.
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	float m_min_speed = 3.0;
	float m_max_speed = 6.0;
	float m_max_accel = 2.0;
	int m_mode = 0;
	// Boid parameters
	float m_rad = 15.0;
	float m_cohes = 1.0;
	float m_align = 1.0;
	float m_avoid = 0.5;
	float m_obj_avoid = 50.0;
	float m_obj_rad = 1.5;
	float m_flee = 35.0;
	//Predator Parameters
	float m_pred_min_speed = 3.0;
	float m_pred_max_speed = 8.0;
	float m_pred_max_accel = 10.0;
	float m_pred_rad = 50.0;
	float m_chasing = 5.0;
	float m_pred_avoid = 0.4;
	float m_pursuit = 0.1;

public:

	Scene();

	// functions that load the scene
	void loadCore();
	void loadCompletion();
	void loadChallenge();

	// called every frame, with timestep in seconds
	void update(float timestep);

	// called every frame, with the given projection and view matrix
	void draw(const glm::mat4 &proj, const glm::mat4 &view);

	// called every frame (to fill out a ImGui::TreeNode)
	void renderGUI();

	// returns a const reference to the boids vector
	const std::vector<Boid> & boids() const { return m_boids; }
	const std::vector<Boid>& objs() const { return m_objs; }

	// returns the half-size of the bounding box (centered around the origin)
	glm::vec3 bound() const { return m_bound_hsize; }

	// boid getters
	float radius() const { return m_rad; }
	float avoidance() const { return m_avoid; }
	float cohesion() const { return m_cohes; }
	float alignment() const { return m_align; }
	float min_speed() const { return m_min_speed; }
	float max_speed() const { return m_max_speed; }
	float max_accel() const { return m_max_accel; }
	float flee() const { return m_flee; }
	float obj_radius() const { return m_obj_rad; }
	float obj_avoid() const { return m_obj_avoid; }
	float mode() const { return m_mode; }

	// predator getters
	float pred_radius() const { return m_pred_rad; }
	float pred_avoidance() const { return m_pred_avoid; }
	float pred_min_speed() const { return m_pred_min_speed; }
	float pred_max_speed() const { return m_pred_max_speed; }
	float pred_max_accel() const { return m_pred_max_accel; }
	float chasing() const { return m_chasing; }
	float pursuit() const { return m_pursuit; }
};