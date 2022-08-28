#pragma once
#include<opencv2/opencv.hpp>

using namespace cv;

void quilting(Mat source, int patch_size, int overlap);
Mat hquilt(Mat left, Mat right, int overlap, int SeamDraw);
Vec2i find_min_neigh_up(Mat map, int i, int j);