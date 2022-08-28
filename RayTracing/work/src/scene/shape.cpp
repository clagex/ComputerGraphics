
// std
#include <algorithm>
#include <utility>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// project
#include "shape.hpp"


using namespace glm;


RayIntersection AABB::intersect(const Ray &ray) {
	RayIntersection intersect;
	vec3 rel_origin = ray.origin - m_center;

	// start magic
	// x
	float rdx_inv = 1 / ray.direction.x;
	float tx1 = (-m_halfsize.x - rel_origin.x) * rdx_inv;
	float tx2 = (m_halfsize.x - rel_origin.x) * rdx_inv;

	float tmin = std::min(tx1, tx2);
	float tmax = std::max(tx1, tx2);

	// y
	float rdy_inv = 1 / ray.direction.y;
	float ty1 = (-m_halfsize.y - rel_origin.y) * rdy_inv;
	float ty2 = (m_halfsize.y - rel_origin.y) * rdy_inv;

	tmin = std::max(tmin, std::min(ty1, ty2));
	tmax = std::min(tmax, std::max(ty1, ty2));

	// z
	float rdz_inv = 1 / ray.direction.z;
	float tz1 = (-m_halfsize.z - rel_origin.z) * rdz_inv;
	float tz2 = (m_halfsize.z - rel_origin.z) * rdz_inv;

	tmin = std::max(tmin, std::min(tz1, tz2));
	tmax = std::min(tmax, std::max(tz1, tz2));

	if (tmax < tmin) return intersect;
	// end magic

	intersect.m_distance = tmin < 0 ? tmax : tmin;
	intersect.m_position = ray.origin + intersect.m_distance * ray.direction;
	intersect.m_valid = tmax >= 0;
	vec3 work_out_a_name_for_it_later = abs((intersect.m_position - m_center) / m_halfsize);
	float max_v = std::max(work_out_a_name_for_it_later[0], std::max(work_out_a_name_for_it_later[1], work_out_a_name_for_it_later[2]));
	intersect.m_normal = normalize(mix(intersect.m_position - m_center, vec3(0), lessThan(work_out_a_name_for_it_later, vec3(max_v))));
	intersect.m_uv_coord = (abs(intersect.m_normal.x) > 0) ?
		vec2(intersect.m_position.y, intersect.m_position.z) :
		vec2(intersect.m_position.x, intersect.m_position.y + intersect.m_position.z);
	intersect.m_shape = this;

	return intersect;
}


RayIntersection Sphere::intersect(const Ray &ray) {
	RayIntersection intersect;
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Implement the intersection method for Sphere that returns
	// a RayIntersection object with valid == false if the 
	// ray doesn't intersect and true otherwise. If true, then
	// remember to fill out each feild in the object correctly:
	// - m_valid : true if object is itersected
	// - m_distance : distance along the ray of the intersection
	// - m_position : position on the surface of the intersection
	// - m_normal : normal on the surface of the intersection
	// - m_shape : set to "this"
	// - m_uv_coord : texture coordinates (challenge only)
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	vec3 d = ray.direction;
	vec3 o = ray.origin;
	vec3 co = o - m_center;
	float a = dot(d, d);
	float b = dot(2.0f * co, d);
	float c = dot(co, co) - pow(m_radius, 2);

	float delta = b * b - 4.0f * a * c;
	if (delta >= 0) {
		float t0 = (-b + sqrt(delta)) / (2.0f * a);
		float t1 = (-b - sqrt(delta)) / (2.0f * a);
		if ((t0 > 0 && t1 > 0) || t0 == t1) {
			float t = min(t0, t1);
			vec3 p = o + t * d;
			intersect.m_valid = true;
			intersect.m_distance = length(p - o);
			intersect.m_position = p;
			intersect.m_normal = normalize(p - m_center);
			intersect.m_shape = this;
			intersect.m_uv_coord = vec2(0);
			vec3 dir = intersect.m_normal;
			float u = 0.5f + (atan2(dir.x, dir.z) / pi<float>() * length(p-m_center));
			float v = 0.5f - asin(dir.y) / pi<float>();
			intersect.m_uv_coord = vec2(u, v);
		}
		else {
			intersect.m_valid = false;
		}
	}
	else {
		intersect.m_valid = false;
	}
	return intersect;
}

RayIntersection Plane::intersect(const Ray& ray) {
	RayIntersection intersect;

	vec3 normal = normalize(m_normal);
	vec3 oq = m_point - ray.origin;
	vec3 d = normalize(ray.direction);
	float angle = dot(d, normal);
	if (abs(angle) < 0.00001) {
		return intersect;
	}
	float t = dot(oq, normal) / angle;
	if (t >= 0.0f) {
		vec3 p = ray.origin + t * d;
		intersect.m_valid = true;
		intersect.m_distance = length(p - ray.origin);
		intersect.m_position = p;
		angle > 0 ? intersect.m_normal = -normal : intersect.m_normal = normal;
		intersect.m_uv_coord = vec2(0);
		intersect.m_shape = this;
		intersect.m_uv_coord = (abs(intersect.m_normal.x) > 0) ?
			vec2(intersect.m_position.y, intersect.m_position.z) :
			vec2(intersect.m_position.x*2.0, intersect.m_position.y*2.0 + intersect.m_position.z*2.0);
	}
	else {
		intersect.m_valid = false;
	}

	return intersect;
}

RayIntersection Disk::intersect(const Ray& ray) {
	Plane p = Plane(m_point, m_normal);
	RayIntersection intersect = p.intersect(ray);
	if (intersect.m_valid) {
		if (length(intersect.m_position - m_point) > m_radius) {
			intersect.m_valid = false;
		}
	}
	return intersect;
}

RayIntersection Triangle::intersect(const Ray& ray) {
	vec3 d1 = m_v2 - m_v1;
	vec3 d2 = m_v3 - m_v1;
	vec3 normal = normalize(cross(d1, d2));
	Plane p = Plane(m_v1, normal);
	RayIntersection intersect = p.intersect(ray);
	if (intersect.m_valid) {
		std::vector<vec3> v = { m_v1, m_v2, m_v3 };
		vec3 p = intersect.m_position;
		intersect.m_uv_coord = vec2(intersect.m_position.x, intersect.m_position.y + intersect.m_position.z);
		for (unsigned int i = 0; i < 3; i++) {
			vec3 v1 = v.at(i);
			vec3 v2 = v.at((i + 1) % 3);
			vec3 f = cross(v2 - v1, p - v1);
			if (dot(normal, f) <= 0.0f) {
				intersect.m_valid = false;
				return intersect;
			}
		}
		
	}

	return intersect;
}

