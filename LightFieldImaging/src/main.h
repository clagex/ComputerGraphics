#pragma once
#include<opencv2/opencv.hpp>
#include "lightfield.h"

int min_col = 770;
int max_col = 870;
int min_row = 205;
int max_row = 305;

LightField lightfield;
cv::String folder_path = "res//rectified";
cv::String user_path = "";

void doCore();