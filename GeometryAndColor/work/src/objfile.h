#ifndef __OBJFILE_H__
#define __OBJFILE_H__

#include <vector>
#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include "opengl.hpp"

using namespace std;

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
};

class ObjFile {
private:
	// CPU-side data
	vector<glm::vec3> m_positions;
	vector<glm::vec3> m_normals;
	vector<Vertex> m_faces;

	// GPU-side data
	GLuint m_vao = 0;
	GLuint m_vbo = 0;
	GLuint m_ibo = 0;
public:
	void loadOBJ(string filename);
	void build();
	void draw();
	void destroy();
	void printMeshData();
};

#endif /*__OBJFILE_H__*/