#include "ImageProc.h"
#include <iostream>

using namespace std;
using namespace cv;

cv::Mat separate_RGB_HSV(cv::Mat& img) {
	// set a bigger canvas in grayscale
	int r = img.rows;
	int c = img.cols;
	cv::Mat new_img = cv::Mat(r*2, c*3, CV_8UC1);

	vector<cv::Mat> rgb_channels;
	cv::split(img, rgb_channels);

	rgb_channels[0].copyTo(new_img(cv::Rect(0, 0, c, r)));
	rgb_channels[1].copyTo(new_img(cv::Rect(c, 0, c, r)));
	rgb_channels[2].copyTo(new_img(cv::Rect(c*2, 0, c, r)));

	cv::Mat hsv_img;
	cv::cvtColor(img, hsv_img, COLOR_BGR2HSV);
	vector<cv::Mat> hsv_channels;
	cv::split(hsv_img, hsv_channels);

	hsv_channels[0].copyTo(new_img(cv::Rect(0, r, c, r)));
	hsv_channels[1].copyTo(new_img(cv::Rect(c, r, c, r)));
	hsv_channels[2].copyTo(new_img(cv::Rect(c * 2, r, c, r)));

	return new_img;
}

cv::Mat HSV_multiply(cv::Mat& img) {
	// set a bigger canvas in RGB
	int r = img.rows;
	int c = img.cols;
	cv::Mat new_img = cv::Mat(r * 3, c * 5, CV_8UC3);

	// splitting image into HSV channels
	cv::Mat hsv_img;
	cv::cvtColor(img, hsv_img, COLOR_BGR2HSV);
	vector<cv::Mat> hsv_channels;
	cv::split(hsv_img, hsv_channels);

	// multiply h, s, v channels
	for (double i = 0; i < 1; i += 0.2) {
		cv::Mat h = hsv_channels[0] * i;
		cv::Mat s = hsv_channels[1] * i;
		cv::Mat v = hsv_channels[2] * i;

		cv::Mat h_hsv, s_hsv, v_hsv;
		cv::merge(vector<cv::Mat>{h, hsv_channels[1], hsv_channels[2]}, h_hsv);
		cv::merge(vector<cv::Mat>{hsv_channels[0], s, hsv_channels[2]}, s_hsv);
		cv::merge(vector<cv::Mat>{hsv_channels[0], hsv_channels[1], v}, v_hsv);

		cv::cvtColor(h_hsv, h_hsv, COLOR_HSV2BGR);
		cv::cvtColor(s_hsv, s_hsv, COLOR_HSV2BGR);
		cv::cvtColor(v_hsv, v_hsv, COLOR_HSV2BGR);

		h_hsv.copyTo(new_img(cv::Rect(i*c*5, 0, c, r)));
		s_hsv.copyTo(new_img(cv::Rect(i * c * 5, c, c, r)));
		v_hsv.copyTo(new_img(cv::Rect(i * c * 5, c*2, c, r)));
	}
	
	return new_img;
}

cv::Mat Euclidean(cv::Mat& img) {
	// set a new canvas in grayscale
	int r = img.rows;
	int c = img.cols;
	cv::Mat new_img = cv::Mat::zeros(r, c, CV_8UC1);
	Vec3b pixel = img.at<Vec3b>(80, 80);

	for (int i = 0; i < r; i++) {
		for (int j = 0; j < c; j++) {
			if (cv::norm(pixel, img.at<Vec3b>(i, j)) < 100) {
				new_img.at<uchar>(i, j) = 255;
			}
		}
	}
	return new_img;
}

cv::Mat applyFilter(cv::Mat& img, std::string method) {

	cv::Mat padded, kernel, new_img;

	copyMakeBorder(img, padded, 1, 1, 1, 1, BORDER_REFLECT);

	float laplacian[] = {0, 1, 0, 1, -4, 1, 0, 1, 0};
	float sobelx[] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };
	float sobely[] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };

	if (method.compare("laplacian") == 0) {
		kernel = cv::Mat(3, 3, CV_32FC1, laplacian);
	}
	else if (method.compare("sobelx") == 0) {
		kernel = cv::Mat(3, 3, CV_32FC1, sobelx);
	}
	else {
		kernel = cv::Mat(3, 3, CV_32FC1, sobely);
	}
	
	kernel = 1 / 9.0f * kernel;

	new_img = calculateFilter(padded, kernel);

	normalize(new_img, new_img, 1.0, 0.0, NORM_MINMAX);
	new_img.convertTo(new_img, CV_8UC1, 255);

	return new_img;
}

cv::Mat calculateFilter(cv::Mat padded, cv::Mat kernel) {
	int r = padded.rows;
	int c = padded.cols;
	cv::Mat result = Mat(r - 2, c - 2, CV_32FC1);

	for (int i = 1; i < result.rows + 1; i++) {
		for (int j = 1; j < result.cols + 1; j++) {
			float sum = 0;

			for (int k = -1; k <= 1; k++) {
				for (int l = -1; l <= 1; l++) {
					int val = padded.at<uchar>(i + k, j + l);

					sum += val * kernel.at<float>(k + 1, l + 1);
				}
			}

			result.at<float>(i - 1, j - 1) = sum;
		}
	}

	return result;
}

cv::Mat Histogram(cv::Mat& img) {
	// create a bigger canvas
	int r = img.rows;
	int c = img.cols;
	cv::Mat new_img = cv::Mat(r, c*2, CV_8UC1);

	// copy the original image to the left
	img.copyTo(new_img(cv::Rect(0, 0, c, r)));

	int histSize = 256;
	float range[] = {0, 256};
	const float* histRange = { range };

	cv::Mat hist;
	cv::calcHist(&img, 1, 0, Mat(), hist, 1, &histSize, &histRange, true, false);
	normalize(hist, hist, 1.0, 0.0, NORM_L1);

	for (int i = 0; i < r; i++) {
		for (int j = 0; j < c; j++) {
			uchar current = img.at<uchar>(i, j);
			float cdf = 0;
			for (int i = 0; i < current; i++) {
				cdf += hist.at<float>(i);
			}
			new_img.at<uchar>(i, j+c) = cdf * 255;
		}
	}


	return new_img;
}
