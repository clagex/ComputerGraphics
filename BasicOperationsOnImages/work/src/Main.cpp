#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/utils/logger.hpp>
#include "ImageProc.h"

using namespace std;
using namespace cv;

void reload_img(cv::Mat& img);
void load_default(cv::Mat& img, std::string name);

cv::String flower_path = "res//Flower.jpg";
cv::String building_path = "res//Building.jpg";
cv::String user_path = "";
bool useDeafult = true;

int main(int argc, char* argv[]) {
    // set the logging level to avoid info warnings
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

    // load the image
    cv::Mat img;
    cv::Mat grayimg;
    reload_img(img);

    // waiting for input to run the program
    cout << "Please type one of the following commands:" << endl;
    cout << "core1" << endl << "core2" << endl << "core3" << endl;
    cout << "comp" << endl << "chag" << endl;
    cout << "reload" << endl << "exit" << endl;
    cout << "waiting for input..." << endl;
    std::string input;
    cin >> input;

    // waiting for commands to run the assignment
    while (input.compare("exit") != 0) {

        if (input.compare("core1") == 0) {
            load_default(img, "rgb");
            cv::Mat E = separate_RGB_HSV(img);
            cv::String windowName = "Split RGB/HSVs";
            cv::imshow(windowName, E);
            cv::waitKey(0);
            cv::destroyAllWindows();
        }
        else if (input.compare("core2") == 0) {
            load_default(img, "rgb");
            cv::Mat E = HSV_multiply(img);
            cv::String windowName = "HSV Multiply";
            cv::imshow(windowName, E);
            cv::waitKey(0);
            cv::destroyAllWindows();
        }
        else if (input.compare("core3") == 0) {
            load_default(img, "rgb");
            cv::Mat E = Euclidean(img);
            cv::String windowName = "Mask";
            cv::imshow(windowName, E);
            cv::waitKey(0);
            cv::destroyAllWindows();

        }
        else if (input.compare("comp") == 0) {
            load_default(grayimg, "gray");
            cv::Mat laplacian = applyFilter(grayimg, "laplacian");
            cv::imshow("Laplacian", laplacian);

            cv::Mat sobelx = applyFilter(grayimg, "sobelx");
            cv::imshow("Sobel X", sobelx);

            cv::Mat sobely = applyFilter(grayimg, "sobely");
            cv::imshow("Sobel Y", sobely);

            cv::waitKey(0);
            cv::destroyAllWindows();
        }
        else if (input.compare("chag") == 0){
            load_default(grayimg, "cgray");
            cv::Mat h = Histogram(grayimg);
            cv::imshow("Histogram", h);
            cv::waitKey(0);
            cv::destroyAllWindows();

        }   
        else if (input.compare("reload") == 0) {
            cin.ignore();
            reload_img(img);
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

/* Load a new image by default or by a new path*/
void reload_img(cv::Mat& img) {
    user_path = "";
    cout << "Please input the file path or continue with default images with enter" << endl;
    
    getline(cin, user_path);

    if (user_path != "") {
        useDeafult = false;
        img = cv::imread(user_path);
        while (img.empty()) {
            cout << "Could not read image: " << user_path << endl;
            cout << "Enter another path..." << endl;
            user_path.clear();
            cin >> user_path;
            img = cv::imread(user_path);
        }
    }
    else {
        useDeafult = true;
    }
}

void load_default(cv::Mat& img, std::string name) {
    if (useDeafult) {
        if (name.compare("rgb") == 0) {
            img = cv::imread(flower_path);
        }
        else if (name.compare("gray") == 0) {
            img = cv::imread(flower_path, IMREAD_GRAYSCALE);
        }
        else {
            img = cv::imread(building_path, IMREAD_GRAYSCALE);
        }
    }
    else {
        if (name.contains("gray")) {
            if (img.channels() == 3) {
                cv::cvtColor(img, img, COLOR_BGR2GRAY);
                
            }
        }
        else {
            if (img.channels() < 3) {
                cout << "reload a RGB image please" << endl;
                reload_img(img);
            }
        }
    }
}
