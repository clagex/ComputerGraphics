#pragma once
#include<opencv2/opencv.hpp>

void reload_img();
void load_default(std::string str);
void validate_input(cv::String input, cv::Mat img);

cv::Mat source;
cv::Mat target;
cv::Mat quilt;
cv::Mat shuffle_src;
cv::Mat mask;

cv::String source_path = "res//Source.jpg";
cv::String target_path = "res//Target.jpg";
cv::String quilt_path = "res//TextureSample.jpg";
cv::String shuffle_path = "res//ReshuffleSource.jpg";
cv::String mask_path = "res//ReshuffleMask.jpg";
cv::String user_path = "";

bool useDeafult = true;