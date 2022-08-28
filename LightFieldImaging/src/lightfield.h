#pragma once

#include <string>

#include <opencv2/core/core.hpp>

struct stImage {
	cv::Vec2f uv; // Position in the UV plane
	cv::Mat image; // the st-image
	stImage() {}
};

class Aperture {
public:
	virtual bool transparent(cv::Vec2f uv) { return 0; }
};

class LightField {
private:
	std::vector<stImage> m_images;

public:
	int rows = -1;
	int cols = -1;

	LightField() { }
	LightField(int rows_, int cols_) : m_images(rows_*cols_, stImage()), rows(rows_), cols(cols_) { }
	LightField(std::string filepath, int rows_, int cols_);


	stImage* operator[] (int row) { return &(m_images.at(row * cols)); }
	const stImage* operator[] (int row) const { return &(m_images.at(row * cols)); }

	cv::Mat reconstruct(Aperture& aperture, float focalDistance = 1.f);
};

class NullAperture : public Aperture {
public:
	virtual bool transparent(cv::Vec2f uv) override { return true; }
};

class CircularAperture : public Aperture {
private:
	cv::Vec2f m_centre;
	float m_radius;
public:
	CircularAperture(cv::Vec2f centre_, float radius_) : m_centre(centre_), m_radius(radius_) {}
	virtual bool transparent(cv::Vec2f uv) override {
		return cv::norm(uv - m_centre) < m_radius;
	}
};