#include<iostream>
#include "patchMatch.h"

using namespace std;
using namespace cv;


cv::Mat patchmatch(Mat source, Mat target, int patch_size, int iterations, Mat nnf, bool show) {
	int tar_rows = target.rows;
	int tar_cols = target.cols;
	Mat cost(tar_rows, tar_cols, CV_32FC1, Scalar(std::numeric_limits<float>::infinity()));


	// Initialization
	if (nnf.empty()) {
		nnf = Mat(tar_rows, tar_cols, CV_32SC2); // 32 bits integer
		cv::randu(nnf, Scalar(patch_size, patch_size), Scalar(source.rows - 1, source.cols - 1)); // absolute coordinates
		
		// Convert the positios to the offset vectors
		for (int i = 0; i < tar_rows; i++) {
			for (int j = 0; j < tar_cols; j++) {
				Vec2i s = nnf.at<Vec2i>(i, j);
				Vec2i t = Vec2i(i, j);
				nnf.at<Vec2i>(i, j) = s - t;
			}
		}
	}
	if (iterations == 0)
		return nnf;

	
	show_imgs(source, nnf, 0, show);

	// Set buffer images to avoid bounding errors
	Mat target_buffer, source_buffer;
	copyMakeBorder(target, target_buffer, patch_size, patch_size, patch_size, patch_size, BORDER_REPLICATE);
	copyMakeBorder(source, source_buffer, patch_size, patch_size, patch_size, patch_size, BORDER_REPLICATE);

	// Calculate the initial costs
	for (int i = 0; i < nnf.rows; i++) {
		for (int j = 0; j < nnf.cols; j++) {
			improve_nnf(nnf.at<Vec2i>(i, j), Vec2i(i, j), source_buffer, target_buffer, nnf, cost, patch_size);
		}
	}

	// Iteration

	for (int iter = 0; iter < iterations; iter++) {
		// Propagation
		// forward propagation
		if (iter % 2 == 0) {
			for (int i = 0; i < nnf.rows; i++) {
				for (int j = 0; j < nnf.cols; j++) {
					Vec2i p, q1, q2;
					p = Vec2i(i, j);
					q1 = Vec2i(i - 1, j);
					q2 = Vec2i(i, j - 1);

					// For top neighbour
					if (contains(nnf, q1)) {
						Vec2i cand_sp = p + nnf.at<Vec2i>(q1);
						if (contains(source, cand_sp))
							improve_nnf(nnf.at<Vec2i>(q1), p, source_buffer, target_buffer, nnf, cost, patch_size);
					}

					// For left neighbour
					if (contains(nnf, q2)) {
						Vec2i cand_sp = p + nnf.at<Vec2i>(q2);
						if (contains(source, cand_sp))
							improve_nnf(nnf.at<Vec2i>(q2), p, source_buffer, target_buffer, nnf, cost, patch_size);
					}
				}
			}
		}
		// backward propagation
		else if (iter % 2 == 1) {
			for (int i = 0; i < nnf.rows; i++) {
				for (int j = 0; j < nnf.cols; j++) {
					Vec2i p, q1, q2;
					p = Vec2i(i, j);
					q1 = Vec2i(i + 1, j);
					q2 = Vec2i(i, j + 1);

					// For bottom neighbour
					if (contains(nnf, q1)) {
						Vec2i cand_sp = p + nnf.at<Vec2i>(q1);
						if (contains(source, cand_sp))
							improve_nnf(nnf.at<Vec2i>(q1), p, source_buffer, target_buffer, nnf, cost, patch_size);
					}
					// For right neighbour
					if (contains(nnf, q2)) {
						Vec2i cand_sp = p + nnf.at<Vec2i>(q2);
						if (contains(source, cand_sp))
							improve_nnf(nnf.at<Vec2i>(q2), p, source_buffer, target_buffer, nnf, cost, patch_size);
					}
				}
			}
		}

		// Random search
		random_search(source, source_buffer, target_buffer, patch_size, nnf, cost);

		// Output image
		show_imgs(source, nnf, iter+1, show);
	}

	return nnf;
}



/* random search - improve the nnf*/
void random_search(Mat source, Mat source_buffer, Mat target_buffer, int patch_size, Mat nnf, Mat cost) {
	
	for (int i = 0; i < nnf.rows; i++) {
		for (int j = 0; j < nnf.cols; j++) {
			Vec2i p(i, j);
			float t = max(source.rows, source.cols);
			while (t > 1) {
				cv::RNG rng(time(NULL));
				Vec2i sp = p + nnf.at<Vec2i>(p);
				Vec2i cand;
				int minX = max(0, sp[1] - int(t / 2.f));
				int maxX = min(source.cols, sp[1] + int(t / 2.f));
				int minY = max(0, sp[0] - int(t / 2.f));
				int maxY = min(source.rows, sp[0] + int(t / 2.f));

				cand = Vec2i(rng.uniform(minY, maxY), rng.uniform(minX, maxX));

				if (contains(source, cand)) {
					cand = cand - p;
					improve_nnf(cand, p, source_buffer, target_buffer, nnf, cost, patch_size);
				}

				t /= 2.0f;
			}
		}
	}
}


cv::Mat reshuffle(Mat source_img, Mat mask_img, int patch_size, int patchmatch_iter, int pyramid_depth, int correction_iter) {
	// make sure source img and mask img are the same size
	resize(mask_img, mask_img, source_img.size());
	assert(source_img.size() == mask_img.size());
	cvtColor(mask_img, mask_img, COLOR_BGR2GRAY);

	// create the guess for the target
	Mat target_img = source_img.clone();
	for (int i = 0; i < target_img.rows; i++) {
		for (int j = 0; j < target_img.cols; j++) {
			if (mask_img.at<uchar>(i, j) > 128) {
				target_img.at<Vec3b>(i, j) = source_img.at<Vec3b>(i, j - 270);
				target_img.at<Vec3b>(i, j - 270) = source_img.at<Vec3b>(i, j);
			}
		}
	}
	
	// construct pyramids
	vector<Mat> source_pyr, target_pyr;
	buildPyramid(source_img, source_pyr, pyramid_depth);
	buildPyramid(target_img, target_pyr, pyramid_depth);

	// iteration over pyramid
	Mat nnf;
	cout << "Begin synthesis" << endl;
	for (int k = pyramid_depth; k-- > 0; ) {
		cout << " - Pyramid Iterations " << (pyramid_depth - k) << "/" << pyramid_depth << endl;

		// correction
		for (int i = 0; i < correction_iter; i++) {
			cout << " - Correction Iterations " << (i + 1) << "/" << correction_iter << endl;
			nnf = patchmatch(source_pyr[k], target_pyr[k], patch_size, patchmatch_iter, nnf, false);	
			target_pyr[k] = reconstruct(source_pyr[k], nnf, patch_size);
		}

		imshow("TargetImg", target_pyr[k]);
		imshow("NNF", nnf2img(nnf, source_pyr[k].size()));
		waitKey(0);
		destroyAllWindows();

		// upsample
		if (k > 0) {
			nnf = upsampleNNF(nnf, source_pyr[k - 1].size());
			target_pyr[k-1] = reconstruct(source_pyr[k-1], nnf, patch_size);
		}
	}
	cout << "Finished" << endl;

	// reconstruct result
	Mat result = reconstruct(source_img, nnf, patch_size);
	Mat nnf_img = nnf2img(nnf, source_img.size());

	imshow("TargetImg", result);
	imshow("NNF", nnf_img);
	waitKey(0);
	destroyAllWindows();
	
	return nnf;
}


/* helper method to compare and update the costs and nnf */
void improve_nnf(Vec2i offset, Vec2i tp, Mat source, Mat target, Mat nnf, Mat cost, int patch_size) {
	int radius = patch_size / 2;
	cv::Mat patch_s, patch_t;
	Vec2i sp = tp + offset;
	float c; //cost

	// get patches
	patch_s = source(Rect(sp[1] - radius + patch_size, sp[0] - radius + patch_size, patch_size, patch_size));
	patch_t = target(Rect(tp[1] - radius + patch_size, tp[0] - radius + patch_size, patch_size, patch_size));

	// cost (ssd)
	c = norm(patch_s, patch_t, NORM_L2);
	if (c < cost.at<float>(tp)) {
		cost.at<float>(tp) = c;
		nnf.at<Vec2i>(tp) = offset;
	}
}


cv::Mat nnf2img(cv::Mat nnf, cv::Size s) {
	cv::Mat nnf_img(nnf.rows, nnf.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	for (int i = 0; i < nnf.rows; ++i) {
		for (int j = 0; j < nnf.cols; ++j) {
			Vec2i p = nnf.at<Vec2i>(i, j);
			if (p[0] < 0 || p[1] < 0 || p[0] >= s.height || p[1] >= s.width) {
				/* coordinate is outside, insert error of choice */
			}
			int r = int(p[1] * 255.0 / s.width); // cols -> red
			int g = int(p[0] * 255.0 / s.height); // rows -> green
			int b = 255 - max(r, g); // blue
			nnf_img.at<Vec3b>(i, j) = Vec3b(b, g, r);
		}
	}
	return nnf_img;
}

cv::Mat reconstruct(Mat source, Mat nnf) {
	Mat new_img = Mat(nnf.rows, nnf.cols, CV_8UC3);
	for (int i = 0; i < nnf.rows; i++) {
		for (int j = 0; j < nnf.cols; j++) {
			Vec2i offset = nnf.at<Vec2i>(i, j);
			Vec2i sp = Vec2i(i, j) + offset;
			new_img.at<Vec3b>(i, j) = source.at<Vec3b>(sp);
		}
	}
	return new_img;
}

cv::Mat reconstruct(Mat source, Mat nnf, int patch_size) {
	vector<vector<Vec3f>> pixels;
	Mat new_img = Mat(nnf.rows, nnf.cols, CV_32FC3, 0.0f);
	Mat src;
	source.convertTo(src, CV_32FC3);
	int radius = patch_size / 2;
	vector<vector<float>> overlaps(nnf.rows, vector<float>(nnf.cols, 0.f));
	for (int i = 0; i < nnf.rows; i++) {
		for (int j = 0; j < nnf.cols; j++) {
			Vec2i offset = nnf.at<Vec2i>(i, j);		
			Vec3f pixel;
			for (int m = i - radius; m <= i + radius; m++) {
				for (int n = j - radius; n <= j + radius; n++) {

					if (contains(nnf, Vec2i(m, n))) {
						Vec2i p = Vec2i(m, n) + offset;						
						if (contains(source, p)) {		
							overlaps.at(m).at(n)++;
							new_img.at<Vec3f>(m, n) += src.at<Vec3f>(p) / 100.f;
						}
					}
				}
			}
		}
	}

	for (int i = 0; i < new_img.rows; i++) {
		for (int j = 0; j < new_img.cols; j++) {
			new_img.at<Vec3f>(i, j) = new_img.at<Vec3f>(i, j) / overlaps.at(i).at(j) * 100;
		}
	}

	Mat result;
	new_img.convertTo(result, CV_8UC3);
	return result;
}

cv::Mat upsampleNNF(Mat nnf, Size size) {
	Mat newNNF = Mat(size.height, size.width, CV_32SC2);
	for (int i = 0; i < newNNF.rows; i++) {
		for (int j = 0; j < newNNF.cols; j++) {
			int r = i / 2;
			int c = j / 2;
			if (r < nnf.rows - 1 && c < nnf.cols - 1) {
				if (i % 2 == 0 && j % 2 == 0) {
					newNNF.at<Vec2i>(i, j) = nnf.at<Vec2i>(r, c) * 2;
				}
				else if (i % 2 == 0 && j % 2 == 1) {
					newNNF.at<Vec2i>(i, j) = nnf.at<Vec2i>(r, c) + nnf.at<Vec2i>(r, c + 1);
				}
				else if (i % 2 == 1 && j % 2 == 0) {
					newNNF.at<Vec2i>(i, j) = nnf.at<Vec2i>(r, c) + nnf.at<Vec2i>(r + 1, c);
				}
				else {
					newNNF.at<Vec2i>(i, j) = nnf.at<Vec2i>(r, c) + nnf.at<Vec2i>(r + 1, c + 1);
				}
			}
			else {		
				newNNF.at<Vec2i>(i, j) = nnf.at<Vec2i>(r, c) * 2;
			}


		}
	}
	return newNNF;
}

bool contains(Mat m, Vec2i v) {
	return v[0] >= 0 && v[1] >= 0 && v[0] < m.rows && v[1] < m.cols;
}

void show_imgs(Mat source, Mat nnf, int iteration, bool show) {
	if (show) {
		if (iteration == 0) cout << "initialized" << endl;
		else cout << "Propagation + Random search at iteration " << iteration << endl;
		Mat result = reconstruct(source, nnf);
		Mat NNF_vis = nnf2img(nnf, nnf.size());
		cv::imshow("Current Result", result);
		cv::imshow("Current NNF", NNF_vis);
		cv::waitKey();
		cv::destroyAllWindows();
	}
}
