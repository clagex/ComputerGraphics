#define GLM_ENABLE_EXPERIMENTAL
//std
#include <iostream>


// glm
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// project
#include "cgra/cgra_geometry.hpp"
#include "skeleton_model.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/euler_angles.hpp"


using namespace std;
using namespace glm;
using namespace cgra;



void skeleton_model::draw(const mat4 &view, const mat4 &proj) {
	// set up the shader for every draw call
	glUseProgram(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader, "uProjectionMatrix"), 1, false, value_ptr(proj));

	// if the skeleton is not empty, then draw
	if (!skel.bones.empty()) {
		drawBone(view, 0);
	}
}


void skeleton_model::drawBone(const mat4 &parentTransform, int boneid) {
    skeleton_bone bone = skel.bones.at(boneid);
    skeleton_bone_pose bonePose = pose.boneTransforms[boneid];
    vec3 dir = bone.direction;
    vec3 basis = bone.basis;
    float length = bone.length;

    mat4 R = orientation(dir, vec3(0, 0, 1));


    mat4 Oy = rotate(mat4(1), radians(90.f), vec3(-1, 0, 0));
    mat4 Ox = rotate(mat4(1), radians(90.f), vec3(0,1,0));
    //mat4 basisR = eulerAngleXYZ(basis.x, basis.y, basis.z);
    mat4 basisR = rotate(mat4(1), basis.z, vec3(0, 0, 1)) *
            rotate(mat4(1), basis.y, vec3(0, 1, 0)) * rotate(mat4(1), basis.x, vec3(1, 0, 0));

    mat4 localC = rotate(mat4(1), bonePose.rotation.z, normalize(vec3(basisR * vec4(0, 0, 1, 1)))) *
                  rotate(mat4(1), bonePose.rotation.y, normalize(vec3(basisR * vec4(0, 1, 0, 1)))) *
                  rotate(mat4(1), bonePose.rotation.x, normalize(vec3(basisR * vec4(1, 0, 0, 1))));


    vec3 newDir = vec3(((localC * vec4(dir, 1.f))));
    mat4 T = translate(mat4(1), length * newDir);
    T = translate(T, bonePose.translation);

    mat jointR = localC * R;
    mat4 newT = parentTransform * T * localC;

/*    if (bone.name == "lhumerus") {
        cout << length * dir.x << " " << length * dir.y << " " << length * dir.z << " " << endl;
        cout << length * newDir.x << " " << length * newDir.y << " " << length * newDir.z << " " << endl;
    }*/

    // draw joint as sphere, skip the root
    if (boneid != 0) {
        mat4 jointS(1);
        jointS = scale(jointS, vec3(0.01));
        glUniformMatrix4fv(glGetUniformLocation(shader, "uModelViewMatrix"), 1, false,
                           value_ptr(parentTransform * jointS));
        glUniform3fv(glGetUniformLocation(shader, "uColor"), 1, value_ptr(vec3(0.2, 0.7, 0.8)));
        drawSphere();

        // draw bone as cone
        mat4 boneS(1);
        boneS = scale(boneS, vec3(0.005, 0.005, length));
        glUniformMatrix4fv(glGetUniformLocation(shader, "uModelViewMatrix"), 1, false,
                           value_ptr(parentTransform * jointR * boneS));
        glUniform3fv(glGetUniformLocation(shader, "uColor"), 1, value_ptr(vec3(1, 1, 1)));
        drawCylinder();

        // draw axis
        drawArrow(parentTransform * basisR * Ox, vec3(1, 0, 0));
        drawArrow(parentTransform * basisR * Oy, vec3(0, 1, 0));
        drawArrow(parentTransform * basisR, vec3(0, 0, 1));
    }

    for (int i : bone.children) {
        drawBone(newT, i);
    }
}

void skeleton_model::drawArrow(const glm::mat4 transform, glm::vec3 col) {
    mat4 cynS = scale(mat4(1), vec3(0.002, 0.002, 0.04));
    mat4 coneS = scale(mat4(1), vec3(0.005, 0.005, 0.01));
    mat4 coneT = translate(mat4(1), vec3(0, 0, 0.04));
    glUniformMatrix4fv(glGetUniformLocation(shader, "uModelViewMatrix"), 1, false, value_ptr(transform * cynS));
    glUniform3fv(glGetUniformLocation(shader, "uColor"), 1, value_ptr(col));
    drawCylinder();
    glUniformMatrix4fv(glGetUniformLocation(shader, "uModelViewMatrix"), 1, false, value_ptr(transform * coneT * coneS));
    drawCone();
}