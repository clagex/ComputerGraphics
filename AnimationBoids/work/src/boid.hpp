
#pragma once

// glm
#include <glm/glm.hpp>

// project
#include "scene.hpp"


class Boid {
private:
	glm::vec3 m_position;
	glm::vec3 m_velocity;
	glm::vec3 m_acceleration;
	int m_flock = 0;
	bool m_predator = false;
	static const Boid* m_prey;
	bool m_isobj = false;
	float m_size = 1.0;

public:
	Boid(glm::vec3 pos, glm::vec3 dir) : m_position(pos), m_velocity(dir) { }
	Boid(glm::vec3 pos, glm::vec3 dir, int flock) : m_position(pos), m_velocity(dir), m_flock(flock) { }
	Boid(glm::vec3 pos, glm::vec3 dir, int flock, bool pred) : m_position(pos), m_velocity(dir), m_flock(flock), m_predator(pred) { }
	Boid(glm::vec3 pos, float size, bool obj) : m_position(pos), m_size(size), m_isobj(obj) { }

	glm::vec3 position() const { return m_position; }
	glm::vec3 velocity() const { return m_velocity; }
	glm::vec3 acceleration() const { return m_acceleration; }
	float scale() const { return m_size; }
	bool is_predator() const { return m_predator; }
	bool is_obj() const { return m_isobj; }

	void clear_prey() { m_prey = nullptr; }

	glm::vec3 color() const;

	void calculateForces(Scene *scene);
	void update(float timestep, Scene *scene);
};