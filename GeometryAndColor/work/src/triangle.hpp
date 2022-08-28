#pragma once

// std
#include <vector>

// glm
#include <glm/glm.hpp>

// project
#include "opengl.hpp"


class TestTriangle {
private:

	// CPU-side data
	std::vector<glm::vec3> m_positions;
	std::vector<glm::vec3> m_normals;
	std::vector<unsigned int> m_indices;

	// GPU-side data
	GLuint m_vao = 0;
	GLuint m_vbo_pos = 0;
	GLuint m_vbo_norm = 0;
	GLuint m_ibo = 0;

public:

	TestTriangle() {
		//upload vertex data

		// vertex 1
		m_positions.push_back(glm::vec3(0, 0, 0));
		m_normals.push_back(glm::vec3(0, 0, 1));

		// vertex 2
		m_positions.push_back(glm::vec3(10, 0, 0));
		m_normals.push_back(glm::vec3(0, 0, 1));

		// vertex 3
		m_positions.push_back(glm::vec3(0, 10, 0));
		m_normals.push_back(glm::vec3(0, 0, 1));

		// create a triangle face by adding three verticies
		m_indices.push_back(0);
		m_indices.push_back(1);
		m_indices.push_back(2);
	}


	void setup() {
		if (m_vao == 0) {
			// generate buffers
			glGenVertexArrays(1, &m_vao); // VAO stores information about how the buffers are set up
			glGenBuffers(1, &m_vbo_pos); // VBO stores the vertex data
			glGenBuffers(1, &m_vbo_norm);
			glGenBuffers(1, &m_ibo); // IBO stores the indices that make up primitives


			// VAO
			//
			glBindVertexArray(m_vao);


			// VBO
			//
			// upload Positions to this buffer
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo_pos);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_positions.size(), m_positions.data(), GL_STATIC_DRAW);
			// this buffer will use location=0 when we use our VAO
			glEnableVertexAttribArray(0);
			// tell opengl how to treat data in location=0 - the data is treated in lots of 3 (3 floats = vec3)
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

			// do the same thing for Normals but bind it to location=1
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo_norm);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_normals.size(), m_normals.data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);


			// IBO
			//
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
			// upload the indices for drawing primitives
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_indices.size(), m_indices.data(), GL_STATIC_DRAW);

			// clean up by binding VAO 0 (good practice)
			glBindVertexArray(0);
		}
	}


	void draw() {
		if (m_vao == 0) return;
		// bind our VAO which sets up all our buffers and data for us
		glBindVertexArray(m_vao);
		// tell opengl to draw our VAO using the draw mode and how many verticies to render
		glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
	}


	void destroy() {
		// delete the data buffers
		glDeleteVertexArrays(1, &m_vao);
		glDeleteBuffers(1, &m_vbo_pos);
		glDeleteBuffers(1, &m_vbo_norm);
		glDeleteBuffers(1, &m_ibo);
		m_vao = 0;
	}
};