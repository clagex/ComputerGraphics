#pragma once
#include<opencv2/opencv.hpp>

using namespace cv;

cv::Mat patchmatch(Mat source, Mat target, int patch_size, int iterations, Mat nnf, bool show);
cv::Mat reshuffle(Mat source, Mat mask, int patch_size, int patchmatch_iter, int pyramid_depth, int correct_iter);

void improve_nnf(Vec2i sp, Vec2i tp, Mat source, Mat target, Mat nnf, Mat cost, int patch_size);
void random_search(Mat source, Mat source_buffer, Mat target_buffer, int patch_size, Mat nnf, Mat cost);
void show_imgs(Mat source, Mat nnf, int iteration, bool show);

cv::Mat nnf2img(Mat nnf, Size s);
cv::Mat upsampleNNF(Mat nnf, Size size);
cv::Mat reconstruct(Mat source, Mat nnf);
cv::Mat reconstruct(Mat source, Mat nnf, int patch_size);

bool contains(Mat m, Vec2i v);



