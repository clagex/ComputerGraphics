
#pragma once

// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// project
#include "opengl.hpp"
#include "cgra/cgra_mesh.hpp"

// Basic model that holds the shader, mesh and transform for drawing.
// Can be copied and/or modified for adding in extra information for drawing
// including colors for diffuse/specular, and textures for texture mapping etc.
struct basic_model {
	GLuint shader = 0;
	cgra::gl_mesh mesh;
	cgra::gl_mesh bounding_box;
	glm::vec3 color{1, 0, 0};
	glm::mat4 modelTransform{1.0};
	int num = 0;
	float shininess = 16;
	glm::vec3 specColor{ 1.0, 1.0, 1.0 };
	// completion
	glm::mat4 offsets[101];
	glm::vec3 colors[101];
	bool show_texture = true;


	void draw(const glm::mat4 &view, const glm::mat4 proj) {
		using namespace glm;

		// cacluate the modelview transform
		mat4 modelview = view * modelTransform;

		// load shader and variables
		glUseProgram(shader);
		glUniformMatrix4fv(glGetUniformLocation(shader, "uProjectionMatrix"), 1, false, value_ptr(proj));
		glUniformMatrix4fv(glGetUniformLocation(shader, "uModelViewMatrix"), 1, false, value_ptr(modelview));
		glUniform3fv(glGetUniformLocation(shader, "uColor"), 1, value_ptr(color));
		glUniform3fv(glGetUniformLocation(shader, "uSpecColor"), 1, value_ptr(specColor));
		glUniform3fv(glGetUniformLocation(shader, "uColor"), 1, value_ptr(colors[0]));
		glUniform3fv(glGetUniformLocation(shader, "uLightDirection"), 1, value_ptr(view * vec4(0.0, 0.0, 0.0, 1.0)));
		glUniform1f(glGetUniformLocation(shader, "uShiniess"), shininess);
		glUniform1i(glGetUniformLocation(shader, "uShowTex"), show_texture);

		// draw the mesh
		mesh.draw(num);
		bounding_box.draw(num);
	}
};