
// glm
#include <glm/gtc/random.hpp>

// project
#include "boid.hpp"
#include "scene.hpp"
#include "cgra/cgra_mesh.hpp"


using namespace glm;
using namespace std;

const Boid* Boid::m_prey = nullptr;

vec3 Boid::color() const {
	if (m_isobj) {
		return vec3(0.66, 0.66, 0.66);
	}
	else if (this == m_prey) {
		return vec3(1, 0, 1);
	}
	else if (m_flock == 0) {
		return vec3(0, 1, 0);
	}
	else if (m_flock == 1) {
		return vec3(0, 0, 1);
	}
	else if (m_flock == 2) {
		return vec3(1, 0, 0);
	}
}


void Boid::calculateForces(Scene* scene) {
	//-------------------------------------------------------------
	// [Assignment 3] :
	// Calculate the forces affecting the boid and update the
	// acceleration (assuming mass = 1).
	// Do NOT update velocity or position in this function.
	// Core : 
	//  - Cohesion
	//  - Alignment
	//  - Avoidance
	//  - Soft Bound (optional)
	// Completion : 
	//  - Cohesion and Alignment with only boids in the same flock
	//  - Predator Avoidance (boids only)
	//  - Predator Chase (predator only)
	// Challenge : 
	//  - Obstacle avoidance
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	// ...
	if (m_isobj) return;
	vec3 avoidance = vec3(0);
	vec3 avoid_obj = vec3(0);
	vec3 cohesion = vec3(0);
	vec3 alignment = vec3(0);
	vec3 flee = vec3(0);
	vec3 chase = vec3(0);
	vec3 average_position = vec3(0);
	vec3 average_velocity = vec3(0);
	unsigned int neighbours = 0;
	bool fleeing = false;

	for (auto& other_boid : scene->boids()) {
		if (this == &other_boid) continue;
		if (other_boid.m_isobj) continue;
		// find distance
		vec3 displacement = m_position - other_boid.m_position;
		float distance = length(displacement);

		if (m_predator) {
			if (distance > scene->pred_radius()) continue;
			if (m_prey == &other_boid && distance < 1.5) {
				m_prey = nullptr;
			}
			if (m_prey == nullptr && distance > 3) {
				m_prey = &other_boid;
			}
			if (m_prey == &other_boid) {
				vec3 pos = other_boid.m_position + (other_boid.m_acceleration * distance * scene->pursuit());
				chase = (pos - m_position);
			}
			if (&other_boid != m_prey) {
				avoidance += displacement / (distance * distance);
			}
		}
		else {
			if (distance > scene->radius()) continue;
			else {
				if (other_boid.m_predator) {
					flee = displacement / (distance * distance);
				}
				// avoidance
				avoidance += displacement / (distance * distance);
				if (m_flock == other_boid.m_flock) {

					// cohesion
					average_position += other_boid.m_position;

					// alignment
					average_velocity += other_boid.m_velocity;
					neighbours++;
				}
			}
		}
	}

	if (length(m_velocity) != 0) {
		float t = 10.0;
		vec3 obj_pos = vec3(0);
		bool found_obj = false;
		for (auto& obj : scene->objs()) {
			float a = dot(m_velocity, m_velocity);
			float b = 2.0f * dot((m_position - obj.m_position), m_velocity);
			float c = dot((m_position - obj.m_position), (m_position - obj.m_position)) - pow((obj.m_size + scene->obj_radius()), 2);
			float delta = b * b - 4.0f * a * c;
			if (delta >= 0) {
				float t0 = (-b + sqrt(delta)) / (2.0f * a);
				float t1 = (-b - sqrt(delta)) / (2.0f * a);
				if (t0 > 0 && t0 < t) {
					t = t0;
					obj_pos = obj.m_position;
					found_obj = true;
				}
				if (t1 > 0 && t1 < t) {
					t = t1;
					obj_pos = obj.m_position;
					found_obj = true;
				}
			}
			//cout << obj.m_position.x << " " << obj.m_position.y << " " << obj.m_position.z << endl;
		}
		if (found_obj) {
			vec3 oc = obj_pos - m_position;
			float oe = dot(oc, m_velocity) / length(m_velocity);
			vec3 e = m_position + oe * normalize(m_velocity);
			if (length(e - obj_pos) != 0) {
				avoid_obj = normalize(e - obj_pos);
			}
			else {
				avoid_obj = sphericalRand(1.0);
			}
		}
	}

	if (neighbours != 0) {
		average_position /= neighbours;
		cohesion = average_position - m_position;
	}
	if (neighbours != 0) {
		average_velocity /= neighbours;
		alignment = average_velocity - m_velocity;
	}
	
	// apply forces
	if (m_predator) {
		m_acceleration = scene->pred_avoidance() * avoidance + scene->chasing() * chase  + avoid_obj * scene->obj_avoid();
		if (length(m_acceleration) > scene->pred_max_accel()) m_acceleration = scene->pred_max_accel() * normalize(m_acceleration);
	}
	else {
		m_acceleration = scene->avoidance() * avoidance + scene->cohesion() * cohesion + scene->alignment() * alignment + scene->flee() * flee + avoid_obj * scene->obj_avoid();
		if (length(m_acceleration) > scene->max_accel()) m_acceleration = scene->max_accel() * normalize(m_acceleration);
	}
}


void Boid::update(float timestep, Scene *scene) {
	//-------------------------------------------------------------
	// [Assignment 3] :
	// Integrate the velocity of the boid using the timestep.
	// Update the position of the boid using the new velocity.
	// Take into account the bounds of the scene which may
	// require you to change the velocity (if bouncing) or
	// change the position (if wrapping).
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	// ...
	if (m_isobj) return;
	if (m_predator) {
		float speed = length(m_velocity) * 1.2;
		speed = clamp(speed, scene->pred_min_speed(), scene->pred_max_speed());
		m_velocity = speed * normalize(m_velocity);
	}
	else {
		float speed = length(m_velocity);
		speed = clamp(speed, scene->min_speed(), scene->max_speed());
		m_velocity = speed * normalize(m_velocity);
	}
	m_velocity += m_acceleration * timestep;
	m_position += m_velocity * timestep;
	if (scene->mode() == 0) {
		m_position = -scene->bound() + mod(m_position + scene->bound(), scene->bound() + scene->bound());
	}
	else if (scene -> mode() == 1) {
		if (!(m_position.x <= scene->bound().x && m_position.x >= -scene->bound().x 
			&& m_position.y <= scene->bound().y && m_position.y >= -scene->bound().y 
			&& m_position.z <= scene->bound().z && m_position.z >= -scene->bound().z)) {
			if (length(m_velocity) != 0) { m_velocity = -m_velocity; }
			else { m_velocity = sphericalRand(1.0); }
		}
	}

}