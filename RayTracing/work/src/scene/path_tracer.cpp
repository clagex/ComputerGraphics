
// glm
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>

// std
#include <random>

// project
#include "scene.hpp"
#include "shape.hpp"
#include "light.hpp"
#include "material.hpp"
#include "path_tracer.hpp"


using namespace std;
using namespace glm;


vec3 SimplePathTracer::sampleRay(const Ray &ray, int) {
	// intersect ray with the scene
	RayIntersection intersect = m_scene->intersect(ray);

	// if ray hit something
	if (intersect.m_valid) {

		// simple grey shape shading
		float f = abs(dot(-ray.direction, intersect.m_normal));
		vec3 grey(0.5, 0.5, 0.5);
		return mix(grey / 2.0f, grey, f);
	}

	// no intersection - return background color
	return { 0.3f, 0.3f, 0.4f };
}



vec3 CorePathTracer::sampleRay(const Ray &ray, int) {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Implement a PathTracer that calculates the ambient, diffuse
	// and specular, for the given ray in the scene, using the 
	// Phong lighting model. Give special consideration to objects
	// that occluded from direct lighting (shadow rays). You do
	// not need to use the depth argument for this implementation.
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	RayIntersection intersect = m_scene->intersect(ray);
	vec3 point = intersect.m_position;
	Material* material = intersect.m_material;

	vec3 ambient = vec3(0);
	vec3 diffuse = vec3(0);
	vec3 spec = vec3(0);

	if (intersect.m_valid) {
		for (auto& light : m_scene->lights()) {
			vec3 irradiance = light->irradiance(point);
			vec3 lightDir = normalize(-light->incidentDirection(point));
			ambient += light->ambience();

			if (!light->occluded(m_scene, point)) {
				diffuse += irradiance * glm::max(dot(intersect.m_normal, lightDir), 0.0f);
				vec r = reflect(ray.direction, intersect.m_normal);
				spec += irradiance * pow(glm::max(dot(r, lightDir), 0.0f), material->shininess());
			}
		}
		return diffuse * material->diffuse() + spec * material->specular() + ambient * material->diffuse();

	}
	// no intersection - return background color
	return { 0.3f, 0.3f, 0.4f };
}



vec3 CompletionPathTracer::sampleRay(const Ray &ray, int depth) {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Using the same requirements for the CorePathTracer add in 
	// a recursive element to calculate perfect specular reflection.
	// That is compute the reflection ray off your intersection and
	// sample a ray in that direction, using the result to additionally
	// light your object. To make this more realistic you may weight
	// the incoming light by the (1 - (1/shininess)).
	//-------------------------------------------------------------

	if (depth < 0) return { 0.3f, 0.3f, 0.4f };

	RayIntersection intersect = m_scene->intersect(ray);
	vec3 ambient = vec3(0);
	vec3 diffuse = vec3(0);
	vec3 spec = vec3(0);

	if (intersect.m_valid) {
		vec3 point = intersect.m_position;
		Material* material = intersect.m_material;
		vec r = reflect(ray.direction, intersect.m_normal);

		for (auto& light : m_scene->lights()) {
			vec3 irradiance = light->irradiance(point);
			vec3 lightDir = normalize(-light->incidentDirection(point));
			ambient += light->ambience();

			if (!light->occluded(m_scene, point)) {
				diffuse += irradiance * glm::max(dot(intersect.m_normal, lightDir), 0.0f);		
				spec += irradiance * pow(glm::max(dot(r, lightDir), 0.0f), material->shininess());

			}
		}

		vec3 newDir = normalize(r);
		Ray newR = Ray(point+0.0001f*newDir, newDir);
		float m = (1 - (1 / material->shininess()));
		vec3 reflect = CompletionPathTracer::sampleRay(newR, --depth);
		vec3 lightsI = diffuse * material->diffuse() + (spec + (m)*reflect) * material->specular() + ambient * material->diffuse();
		return lightsI;
	}

	// no intersection - return background color
	return { 0.3f, 0.3f, 0.4f };
}



vec3 ChallengePathTracer::sampleRay(const Ray &ray, int depth) {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Implement a PathTracer that calculates the diffuse and 
	// specular, for the given ray in the scene, using the 
	// Phong lighting model. Give special consideration to objects
	// that occluded from direct lighting (shadow rays).
	// Implement support for textured materials (using a texture
	// for the diffuse portion of the material).
	//
	// EXTRA FOR EXPERTS :
	// Additionally implement indirect diffuse and specular instead
	// of using the ambient lighting term.
	// The diffuse is sampled from the surface hemisphere and the
	// specular is sampled from a cone of the phong lobe (which
	// gives a glossy look). For best results you need to normalize
	// the lighting (see http://www.thetenthplanet.de/archives/255)
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	//cout << depth << endl;
	if (depth < 0) return { 0.3f, 0.3f, 0.4f };

	RayIntersection intersect = m_scene->intersect(ray);
	vec3 ambient = vec3(0);
	vec3 diffuse = vec3(0);
	vec3 spec = vec3(0);
	

	if (intersect.m_valid) {
		vec3 point = intersect.m_position;
		Material* material = intersect.m_material;
		vec r = reflect(ray.direction, intersect.m_normal);

		for (auto& light : m_scene->lights()) {
			vec3 irradiance = light->irradiance(point);
			vec3 lightDir = normalize(-light->incidentDirection(point));
			vec3 diff = vec3(0);
			vec3 specular = vec3(0);

			if (!light->occluded(m_scene, point)) {
				diffuse += irradiance * glm::max(dot(intersect.m_normal, lightDir), 0.0f);
				spec += irradiance * pow(glm::max(dot(r, lightDir), 0.0f), material->shininess()) * ((material->shininess()+1)/(2.0f*pi<float>()));
			}
		}

		vec3 newDir = normalize(r);
		Ray newR = Ray(point + 0.0001f * newDir, newDir);
		float m = (1 - (1 / material->shininess()));
		vec3 result = ChallengePathTracer::sampleRay(newR, --depth);
		vec3 diffCol;
		if (material->hasTexture()) {
			vec2 uv = intersect.m_uv_coord;
			if (point.z < -5 && point.z >= -30) {
				float t = glm::min((abs(point.z+5)) / 15.0f, 0.95f);
				diffCol = (1-t) * material->texture()->sample(uv.x, uv.y) + (t) * vec3(0.3f, 0.3f, 0.4f);
			}
			else if (point.z < -30) {
				diffCol = 0.05f * material->texture()->sample(uv.x, uv.y) + 0.95f * vec3(0.3f, 0.3f, 0.4f);
			}
			else {
				diffCol = material->texture()->sample(uv.x, uv.y);
			}
		}
		else {
			diffCol = material->diffuse();
		}
		spec = spec + (m)*result;

		Ray diffRay = Ray(point, intersect.m_normal);
		vec4 diffDir = vec4(diffRay.direction, 0);
		float pitch = linearRand(0.0f, pi<float>() / 2.0f);
		float yaw = linearRand(0.0f, 2.0f * pi<float>());
		mat4 trans = mat4(1);
		trans = rotate(trans, yaw, vec3(0.0, 1.0, 0.0));
		trans = rotate(trans, pitch, vec3(1.0, 0.0, 0.0));
		diffDir = trans * diffDir;
		vec3 dir = vec3(diffDir);
		diffRay.direction = normalize(dir);
		vec3 stocasticDiff = ChallengePathTracer::sampleRay(diffRay, --depth);

		Ray specRay = Ray(point, r);
		float rng = linearRand(0.0f, 1.0f);
		vec4 specDir = vec4(specRay.direction, 0);
		pitch = linearRand(0.0f, asin(pow(rng, material->shininess())));
		trans = mat4(1);
		trans = rotate(trans, yaw, vec3(0.0, 1.0, 0.0));
		trans = rotate(trans, pitch, vec3(1.0, 0.0, 0.0));
		specDir = trans * specDir;
		dir = vec3(specDir);
		specRay.direction = normalize(dir);
		vec3 stocasticSpec = ChallengePathTracer::sampleRay(specRay, --depth);

		diffuse = mix(diffuse, stocasticDiff, m_scene->m_diff);
		spec = mix(spec, stocasticSpec, m_scene->m_spec);


		vec3 lightsI = diffuse * diffCol + spec * material->specular();
		return lightsI;
	}

	// no intersection - return background color
	return { 0.3f, 0.3f, 0.4f };
}