
#include "objfile.h"
#include <vector>
#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include "opengl.hpp"
#include <fstream>
#include <sstream>

using namespace std;

void ObjFile::loadOBJ(string filename) {
	ifstream infn;
	infn.open(filename);
	if (!infn.is_open()) {
		cout << "file not opened!" << endl;
		return;
	}
	
	string line;
	string tmp;
	while (getline(infn, line)) {
		if (line.starts_with("v ")) {
			// position
			glm::vec3 pos;
			istringstream(line) >> tmp >> pos.x >> pos.y >> pos.z;
			m_positions.push_back(pos);
		}
		else if (line.starts_with("vn ")) {
			// normal
			glm::vec3 norm;
			istringstream(line) >> tmp >> norm.x >> norm.y >> norm.z;
			m_normals.push_back(norm);
		}
		else if (line.starts_with("f ")) {
			// face
			istringstream v(line.substr(2));
			string vertex;
			string token;
			unsigned int index;
			vector<unsigned int> indices;
			while (getline(v, vertex, ' ')) {
				istringstream t(vertex);
				while (getline(t, token, '/')) {
					istringstream(token) >> index;
					indices.push_back(index - 1);
				}
			}
			Vertex v1 = { m_positions.at(indices[0]), m_normals.at(indices[2]) };
			Vertex v2 = { m_positions.at(indices[3]), m_normals.at(indices[5]) };
			Vertex v3 = { m_positions.at(indices[6]), m_normals.at(indices[8]) };
			m_faces.push_back(v1);
			m_faces.push_back(v2);
			m_faces.push_back(v3);
		}
		else { continue; }
	}
	infn.close();
}

void ObjFile::build() {
	if (m_vao == 0) {
		// generate buffers
		glGenVertexArrays(1, &m_vao); // VAO stores information about how the buffers are set up
		glGenBuffers(1, &m_vbo); // VBO stores the vertex data

		glBindVertexArray(m_vao); // Bind a VAO
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // Bind a VBO

		// VBO
		// upload Positions to this buffer
		glBufferData(GL_ARRAY_BUFFER, m_faces.size() * sizeof(Vertex), (void*)m_faces.data(), GL_STATIC_DRAW);
		// this buffer will use location=0 when we use our VAO
		glEnableVertexAttribArray(0);
		// tell opengl how to treat data in location=0 - the data is treated in lots of 3 (3 floats = vec3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));

		// do the same thing for Normals but bind it to location=1
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));


		// clean up by binding VAO 0 (good practice)
		glBindVertexArray(0);

	}
}

void ObjFile::draw() {
	if (m_vao == 0) return;
	// bind our VAO which sets up all our buffers and data for us
	glBindVertexArray(m_vao);
	// tell opengl to draw our VAO using the draw mode and how many verticies to render
	glDrawArrays(GL_TRIANGLES, 0, m_faces.size());
}

void ObjFile::destroy() {
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
	m_vao = 0;
	m_positions.clear();
	m_normals.clear();
	m_faces.clear();
}

void ObjFile::printMeshData() {
	cout << "printing..."<< endl;
	for (const auto& v : m_faces) {
		cout << "Vertex Position " << v.position.x << " " << v.position.y << " " << v.position.z 
			<< " Normal " << v.normal.x << " " << v.normal.y << " " << v.normal.z << endl;
	}
}
