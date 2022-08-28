#include <iostream>
#include "quilting.h"
#include "patchMatch.h"

using namespace std;
using namespace cv;

void quilting(Mat source, int patch_size, int overlap) {
	int startX = 0, startY = 0;
	Mat synthesis = source(Rect(startX, startY, patch_size, patch_size));
	for (int p = 0; p < 5; p++) {
		float cost = numeric_limits<float>::infinity();
		Mat patch;
		Mat left_over = synthesis(Rect(synthesis.cols - overlap, 0, overlap, patch_size));
		for (int i = 0; i + patch_size <= source.rows; i++) {
			for (int j = 0; j + patch_size <= source.cols; j++) {
				Mat current = source(Rect(i, j, patch_size, patch_size));
				Mat right_over = current(Rect(0, 0, overlap, patch_size));
				float c = norm(right_over, left_over, NORM_L2);
				if (c < cost) {
					patch = current;
					cost = c;
				}
			}
		}

		synthesis = hquilt(synthesis, patch, overlap, 1);
	}

	imshow("Image Quilting", synthesis);
	waitKey();
	destroyAllWindows();
}

Mat hquilt(Mat left, Mat right, int overlap, int SeamDraw) {
	assert(left.rows == right.rows);

	Mat leftPatch = left(Rect(left.cols - overlap, 0, overlap, left.rows));
	Mat rightPatch = right(Rect(0, 0, overlap, right.rows));

	// accumulate min path cost with dynamic programming
	Mat errorMap(left.rows, overlap, CV_32FC1);
	for (int i = 0; i < errorMap.rows; i++) {
		for (int j = 0; j < errorMap.cols; j++) {
			Vec3b left_p = leftPatch.at<Vec3b>(i, j);
			Vec3b right_p = rightPatch.at<Vec3b>(i, j);
			errorMap.at<float>(i, j) = norm(left_p, right_p, NORM_L2);
			
		}
		// accumulate from minimum from the three cells of the above row
		if (i > 0) {
			for (int j = 0; j < errorMap.cols; j++) {
				Vec2i min_p = find_min_neigh_up(errorMap, i, j);
				errorMap.at<float>(i, j) += errorMap.at<float>(min_p);
			}
		}
	}

	// Mask creation
	Mat mask(errorMap.rows, errorMap.cols, CV_8UC1, Scalar(255));
	Mat seam_mask(errorMap.rows, errorMap.cols, CV_8UC1, Scalar(0));

	// Find lowest value column in last row
	Point minLoc;
	minMaxLoc(errorMap.row(errorMap.rows - 1), NULL, NULL, &minLoc, NULL);

	// black out the mask	
	seam_mask.at<uchar>(errorMap.rows - 1, minLoc.x) = 255;
	for (int j = 0; j < minLoc.x; j++) {
		mask.at<uchar>(errorMap.rows - 1, j) = 0;
	}

	// calculate path
	Vec2i newLoc = Vec2i(minLoc.y, minLoc.x);
	Vec2i tmpLoc;
	for (int i = errorMap.rows - 1; i > 0; i--) {
		tmpLoc = newLoc;
		newLoc = find_min_neigh_up(errorMap, i, tmpLoc[1]);
		seam_mask.at<uchar>(newLoc[0], newLoc[1]) = 255;
		for (int j = 0; j < newLoc[1]; j++) {
			mask.at<uchar>(newLoc[0], j) = 0;
		}
	}

	// Merge the patches
	Mat merged(left.rows, left.cols + right.cols - overlap, leftPatch.type(), Scalar(0));
	left.copyTo(merged(Rect(0, 0, left.cols, left.rows)));
	rightPatch.copyTo(merged(Rect(left.cols - overlap , 0, overlap, left.rows)), mask);
	right(Rect(overlap, 0, right.cols - overlap, right.rows)).copyTo(merged(Rect(left.cols, 0, right.cols - overlap, right.rows)));

	// draw seam
	if (SeamDraw == 0)
		return merged;
	merged(Rect(left.cols - overlap, 0, overlap, left.rows)).setTo(Scalar(0, 255, 0), seam_mask);
	merged(Rect(left.cols - overlap - 1, 0, overlap, left.rows)).setTo(Scalar(0, 255, 0), seam_mask);

	return merged;
}


Vec2i find_min_neigh_up(Mat map, int i, int j) {
	Vec2i pl, pr, loc;
	float val = map.at<float>(i - 1, j);
	loc = Vec2i(i - 1, j);
	pl = Vec2i(i - 1, j - 1);
	pr = Vec2i(i - 1, j + 1);

	if (contains(map, pl)) {
		if (map.at<float>(pl) < val) {
			val = map.at<float>(pl);
			loc = pl;
		}
	}
	if (contains(map, pr)) {
		if (map.at<float>(pr) < val) {
			val = map.at<float>(pr);
			loc = pr;
		}
	}

	return loc;
}

