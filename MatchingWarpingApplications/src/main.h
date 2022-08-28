#pragma once
#include<opencv2/opencv.hpp>

int min_col = 770;
int max_col = 870;
int min_row = 205;
int max_row = 305;
const int frame_count = 102;

std::string path = "res\\frames\\";
const float epsilon = 10.f;

void doCore(std::vector<cv::Mat> frames);
void doComp(std::vector<cv::Mat> frames);

cv::Mat ransac(std::vector<cv::DMatch> matches, std::vector<cv::Point2f> points_1, std::vector<cv::Point2f>points_2);
cv::Mat findH(cv::Mat img_1, cv::Mat img_2);
cv::Mat warpImg(cv::Mat img, cv::Mat H);