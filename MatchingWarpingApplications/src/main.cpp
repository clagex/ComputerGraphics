#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/utils/logger.hpp>
#include "main.h"


using namespace std;
using namespace cv;

int main(int argc, char* argv[]) {
    // set the logging level to avoid info warnings
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

    // load in frames
    vector<Mat> frames;
    for (size_t i = 0; i < frame_count; i++) {
        ostringstream oss;
        oss << path << "Frame" << setw(3) << setfill('0') << i << ".jpg";

        Mat image = imread(oss.str());
        frames.push_back(image);
    }



    // waiting for input to run the program
    cout << "Please type one of the following commands:" << endl;
    cout << "core" << endl;
    cout << "comp" << endl;
    cout << "exit" << endl;
    cout << "waiting for input..." << endl;
    std::string input;
    cin >> input;

    // waiting for commands to run the assignment
    while (input.compare("exit") != 0) {


        if (input.compare("core") == 0) {
            doCore(frames);
            waitKey(0);
            destroyAllWindows();
        }
        else if (input.compare("comp") == 0) {
            doComp(frames);
        }
        else {
            cout << "Please enter a valid argument" << endl;
        }
        cout << "waiting for input..." << endl;
        input.clear();
        cin >> input;
    }

    return 0;
}

void doCore(vector<Mat> frames) {
    Mat img_1 = frames[39];
    Mat img_2 = frames[41];

    Ptr<cv::SIFT> sift = sift->create();
    std::vector<KeyPoint> keypoints_1, keypoints_2;
    sift->detect(img_1, keypoints_1);
    sift->detect(img_2, keypoints_2);

    // 128-dimensional vector
    Mat descriptors_1, descriptors_2;
    sift->compute(img_1, keypoints_1, descriptors_1);
    sift->compute(img_2, keypoints_2, descriptors_2);

    BFMatcher matcher(NORM_L2, true);
    vector<DMatch> matches;
    matcher.match(descriptors_1, descriptors_2, matches);

    vector<Point2f> points_1, points_2;
    for (int i = 0; i < matches.size(); i++) {
        points_1.push_back(keypoints_1[matches[i].queryIdx].pt);
        points_2.push_back(keypoints_2[matches[i].trainIdx].pt);
    }


    cv::Mat core_1(img_1.rows*2, img_1.cols, CV_8UC3);
    img_1.copyTo(core_1(Rect(0, 0, img_1.cols, img_1.rows)));
    img_2.copyTo(core_1(Rect(0, img_1.rows, img_1.cols, img_1.rows)));

    for (int i = 0; i < points_1.size(); i++) {
        Point start = Point(int(points_1[i].x + 0.5), int(points_1[i].y + 0.5));
        Point end = Point(int(points_2[i].x + 0.5), int(points_2[i].y + 0.5) + img_1.rows);
        line(core_1, start, end, Scalar(0, 255, 0));
    }

    cv::imshow("Core 1", core_1);
    // cv::imwrite("Core_1.jpg", core_1);

    // second part
    Mat best_homography = ransac(matches, points_1, points_2);


    cv::Mat core_2(img_1.rows * 2, img_1.cols, CV_8UC3);
    img_1.copyTo(core_2(Rect(0, 0, img_1.cols, img_1.rows)));
    img_2.copyTo(core_2(Rect(0, img_1.rows, img_1.cols, img_1.rows)));
    for (int i = 0; i < points_1.size(); i++) {
        Point2f p = points_2[i];

        best_homography.convertTo(best_homography, CV_32FC1);
        Mat orig_q = Mat(3, 1, CV_32FC1);
        orig_q.at<float>(0, 0) = points_1[i].x;
        orig_q.at<float>(1, 0) = points_1[i].y;
        orig_q.at<float>(2, 0) = 1.f;

        Mat trans_q = best_homography * orig_q;
        Point2f q = Point2f(trans_q.at<float>(0, 0), trans_q.at<float>(1, 0));

        float dst = norm(p - q);

        Point start = Point(int(points_1[i].x + 0.5), int(points_1[i].y + 0.5));
        Point end = Point(int(points_2[i].x + 0.5), int(points_2[i].y + 0.5) + img_1.rows);

        if (dst < epsilon) {
            line(core_2, start, end, Scalar(0, 255, 0));
        }
        else {
            line(core_2, start, end, Scalar(0, 0, 255));
        }

    }
    cv::imshow("Core 2", core_2);
    // imwrite("Core_2.jpg", core_2);

    // third part
    Mat img_1_border, img_2_border, core_3;
    copyMakeBorder(img_1, img_1_border, 50, 50, 50, 50, BORDER_CONSTANT, Scalar(0, 255, 0));
    copyMakeBorder(img_2, img_2_border, 50, 50, 50, 50, BORDER_CONSTANT, Scalar(0, 255, 0));

    Mat warped = img_2_border.clone();
    warpPerspective(img_1_border, warped, best_homography, warped.size(), 1, BORDER_CONSTANT, Scalar(0, 255, 0));
    addWeighted(warped, 0.5, img_2_border, 0.5, 0.0, core_3);
    imshow("Core 3", core_3);
    // imwrite("Core_3.jpg", core_3);
}

Mat ransac(vector<DMatch> matches, vector<Point2f> points_1, vector<Point2f> points_2) {
    Mat best_homography; //Ransac
    vector<int> best_inliers;
    int best_count = 0;
    float best_error = numeric_limits<float>::infinity();    
    RNG rng;
    for (int iter = 0; iter < 1000; iter++) {
        vector<int> random;
        while (random.size() < 4) {
            int r = int(rng) % matches.size();
            if (find(random.begin(), random.end(), r) == random.end())
                random.push_back(r);
        }

        std::vector<Point2f> r1, r2;
        Mat H;
        for (int i = 0; i < 4; i++) {
            r1.push_back(points_1[random[i]]);
            r2.push_back(points_2[random[i]]);
        }
        H = findHomography(r1, r2);

        int count = 0;
        float error = 0;
        vector<int> inliers;
        for (int i = 0; i < points_1.size(); i++) {
            Point2f p = points_2[i];

            H.convertTo(H, CV_32FC1);
            Mat orig_q = Mat(3, 1, CV_32FC1);
            orig_q.at<float>(0, 0) = points_1[i].x;
            orig_q.at<float>(1, 0) = points_1[i].y;
            orig_q.at<float>(2, 0) = 1.f;

            Mat trans_q = H * orig_q;
            Point2f q = Point2f(trans_q.at<float>(0, 0), trans_q.at<float>(1, 0));

            float dst = norm(p - q);

            if (dst < epsilon) {
                count++;
                error += dst;
                inliers.push_back(i);
            }
        }
        if (count > best_inliers.size()) {
            best_error = error;
            best_homography = H;
            best_inliers = inliers;

        }
        else if (count == best_inliers.size() && error < best_error) {
            best_error = error;
            best_homography = H;
            best_inliers = inliers;
        }
    }
    std::vector<Point2f> r1, r2;
    for (int i = 0; i < best_inliers.size(); i++) {
        r1.push_back(points_1[best_inliers[i]]);
        r2.push_back(points_2[best_inliers[i]]);
    }
    best_homography = findHomography(r1, r2);

    return best_homography;
}


Mat findH(Mat img_1, Mat img_2) {
    Ptr<cv::SIFT> sift = sift->create();
    std::vector<KeyPoint> keypoints_1, keypoints_2;
    sift->detect(img_1, keypoints_1);
    sift->detect(img_2, keypoints_2);

    // 128-dimensional vector
    Mat descriptors_1, descriptors_2;
    sift->compute(img_1, keypoints_1, descriptors_1);
    sift->compute(img_2, keypoints_2, descriptors_2);

    BFMatcher matcher(NORM_L2, true);
    vector<DMatch> matches;
    matcher.match(descriptors_1, descriptors_2, matches);

    vector<Point2f> points_1, points_2;
    for (int i = 0; i < matches.size(); i++) {
        points_1.push_back(keypoints_1[matches[i].queryIdx].pt);
        points_2.push_back(keypoints_2[matches[i].trainIdx].pt);
    }
    return ransac(matches, points_1, points_2);
}


void doComp(std::vector<cv::Mat> frames) {
    int startFrame = 1;
    int endFrame = frames.size();
    vector<Mat> homographys, tildes;
    for (int i = startFrame; i < endFrame; i++) {
        Mat h = findH(frames[i], frames[1-1]);
        h.convertTo(h, CV_32FC1);
        homographys.push_back(h);
        if (i == startFrame) {
            tildes.push_back(h);
        }
        else {
            Mat lastH = tildes.at(tildes.size() - 1);
            tildes.push_back(lastH * h);
        }
    }

    vector<float> weights = { 0.1, 0.3, 0.5, 0.3, 0.1 };
    float sum = 1.3;
    Mat front = tildes.front();
    Mat end = tildes.back();
    tildes.insert(tildes.begin(), front);
    tildes.insert(tildes.begin(), front);
    tildes.push_back(end);
    tildes.push_back(end);

    vector<Mat> tilde_primes;
    for (int i = 0; i < tildes.size() - 4; i++) {
        Mat H(3, 3, CV_32FC1, Scalar(0.f));
        for (int j = 0; j < weights.size(); j++) {
            H += weights[j] * tildes[j+i];
        }
        H /= sum;
        tilde_primes.push_back(H);
    }

    for (int i = 0; i < tilde_primes.size(); i++) {
        Mat U = tilde_primes[i].inv() * tildes[i + 2];
        Mat warped = warpImg(frames[startFrame+i], U);
        stringstream ss;
        ss << "res/stable/Stable" << setfill('0') << setw(3) << startFrame + i << ".jpg";
        //imshow(ss.str(), warped);
        //imwrite(ss.str(), warped);
    }

    
}

Mat warpImg(Mat img, Mat H) {
    Mat warped = img.clone();
    warpPerspective(img, warped, H, warped.size(), 1, BORDER_CONSTANT, Scalar(0, 255, 0));
    return warped;
}