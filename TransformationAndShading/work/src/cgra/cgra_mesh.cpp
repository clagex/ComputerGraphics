
// std
#include <stdexcept>
#include <iostream>

// project
#include "cgra_mesh.hpp"
#include <glm/glm.hpp>



using namespace glm;

namespace cgra {

	void gl_mesh::draw() {
		if (vao == 0) return;
		// bind our VAO which sets up all our buffers and data for us
		glBindVertexArray(vao);
		// tell opengl to draw our VAO using the draw mode and how many verticies to render
		glDrawElements(mode, index_count, GL_UNSIGNED_INT, 0);
	}

	void gl_mesh::draw(int num) {
		if (vao == 0) return;
		// bind our VAO which sets up all our buffers and data for us
		glBindVertexArray(vao);
		glDrawElementsInstanced(mode, index_count, GL_UNSIGNED_INT, 0, num);
	}

	void gl_mesh::destroy() {
		// delete the data buffers
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ibo);
	}


	gl_mesh mesh_builder::build() const {

		gl_mesh m;
		glGenVertexArrays(1, &m.vao); // VAO stores information about how the buffers are set up
		glGenBuffers(1, &m.vbo); // VBO stores the vertex data
		glGenBuffers(1, &m.ibo); // IBO stores the indices that make up primitives


		// VAO
		//
		glBindVertexArray(m.vao);

		
		// VBO (single buffer, interleaved)
		//
		glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
		// upload ALL the vertex data in one buffer
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(mesh_vertex), &vertices[0], GL_STATIC_DRAW);
		//glBufferData(GL_ARRAY_BUFFER, m.)

		// this buffer will use location=0 when we use our VAO
		glEnableVertexAttribArray(0);
		// tell opengl how to treat data in location=0 - the data is treated in lots of 3 (3 floats = vec3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(mesh_vertex), (void *)(offsetof(mesh_vertex, pos)));

		// do the same thing for Normals but bind it to location=1
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(mesh_vertex), (void *)(offsetof(mesh_vertex, norm)));

		// do the same thing for UVs but bind it to location=2 - the data is treated in lots of 2 (2 floats = vec2)
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(mesh_vertex), (void *)(offsetof(mesh_vertex, uv)));

		// IBO
		//
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ibo);
		// upload the indices for drawing primitives
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);


		// set the index count and draw modes
		m.index_count = indices.size();
		m.mode = mode;

		// clean up by binding VAO 0 (good practice)
		glBindVertexArray(0);

		return m;
	}

	gl_mesh mesh_builder::build(int num, mat4 transforms[], vec3 colors[]) const {

		gl_mesh m;
		glGenVertexArrays(1, &m.vao); // VAO stores information about how the buffers are set up
		glGenBuffers(1, &m.vbo); // VBO stores the vertex data
		glGenBuffers(1, &m.ibo); // IBO stores the indices that make up primitives

		unsigned int transVBO;
		glGenBuffers(1, &transVBO); // stores transformation data
		glBindBuffer(GL_ARRAY_BUFFER, transVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * num, &transforms[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		unsigned int colorsVBO;
		glGenBuffers(1, &colorsVBO); // stores color data
		glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * num, &colors[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		// VAO
		glBindVertexArray(m.vao);

		// VBO (single buffer, interleaved)
		//
		glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
		// upload ALL the vertex data in one buffer
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(mesh_vertex), &vertices[0], GL_STATIC_DRAW);

		// this buffer will use location=0 when we use our VAO
		glEnableVertexAttribArray(0);
		// tell opengl how to treat data in location=0 - the data is treated in lots of 3 (3 floats = vec3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(mesh_vertex), (void*)(offsetof(mesh_vertex, pos)));

		// do the same thing for Normals but bind it to location=1
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(mesh_vertex), (void*)(offsetof(mesh_vertex, norm)));

		// do the same thing for UVs but bind it to location=2 - the data is treated in lots of 2 (2 floats = vec2)
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(mesh_vertex), (void*)(offsetof(mesh_vertex, uv)));

		// for colors
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
		glVertexAttribDivisor(3, 1); // for instancing

		// for transformations
		for (int i = 0; i < 4; i++) {
			glEnableVertexAttribArray(4 + i);
			glBindBuffer(GL_ARRAY_BUFFER, transVBO);
			glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(i * sizeof(vec4)));
			glVertexAttribDivisor(4 + i, 1); // for instancing
		}

		// IBO
		//
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ibo);
		// upload the indices for drawing primitives
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);


		// set the index count and draw modes
		m.index_count = indices.size();
		m.mode = mode;

		// clean up by binding VAO 0 (good practice)
		glBindVertexArray(0);

		m.minv = m.maxv = vertices[0].pos;
		for (auto &v : vertices) {
			if (v.pos.x < m.minv.x) m.minv.x = v.pos.x;
			if (v.pos.y < m.minv.y) m.minv.y = v.pos.y;
			if (v.pos.z < m.minv.z) m.minv.z = v.pos.z;
			if (v.pos.x > m.maxv.x) m.maxv.x = v.pos.x;
			if (v.pos.y > m.maxv.y) m.maxv.y = v.pos.y;
			if (v.pos.z > m.maxv.z) m.maxv.z = v.pos.z;
		}

		return m;
	}
}