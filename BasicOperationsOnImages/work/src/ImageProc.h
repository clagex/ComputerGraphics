#pragma once
#include <opencv2/opencv.hpp>

using namespace cv;

cv::Mat separate_RGB_HSV(cv::Mat& img);
cv::Mat HSV_multiply(cv::Mat& img);
cv::Mat Euclidean(cv::Mat& img);
cv::Mat applyFilter(cv::Mat& img, std::string method);
cv::Mat calculateFilter(cv::Mat padded, cv::Mat kernel);
cv::Mat Histogram(cv::Mat& img);
