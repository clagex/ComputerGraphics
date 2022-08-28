#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/utils/logger.hpp>
#include "patchMatch.h"
#include "quilting.h"
#include "main.h"

using namespace std;
using namespace cv;


int main(int argc, char* argv[]) {
    // set the logging level to avoid info warnings
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

    // load the image
    reload_img();

    // waiting for input to run the program
    cout << "Please type one of the following commands:" << endl;
    cout << "core" << endl;
    cout << "comp" << endl << "chag" << endl;
    cout << "reload" << endl << "exit" << endl;
    cout << "waiting for input..." << endl;
    std::string input;
    cin >> input;

    // waiting for commands to run the assignment
    while (input.compare("exit") != 0) {

        if (input.compare("core") == 0) {
            load_default("patchmatch");

            patchmatch(source, target, 7, 4, Mat(), true);
            cv::waitKey(0);
            cv::destroyAllWindows();
        }
       else if (input.compare("comp") == 0) {
            load_default("quilt");
            quilting(quilt, 100, 20);

            cv::waitKey(0);
            cv::destroyAllWindows();
        }
        else if (input.compare("chag") == 0) {
            load_default("reshuffle");
            reshuffle(shuffle_src, mask, 7, 4, 3, 5);

            cv::waitKey(0);
            cv::destroyAllWindows();
        }
        else if (input.compare("reload") == 0) {
            cin.ignore();
            reload_img();
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
void reload_img() {
    std::string part = "";
    cout << "please input \"core\", \"comp\" or \"chag\" for reloading or continue with default images with enter" << endl;
    getline(cin, part);
    if (part.compare("core") == 0) {
        user_path = "";
        cout << "Please input the source image path" << endl;
        getline(cin, user_path);
        validate_input(user_path, source);

        cout << "Please input the target image path" << endl;
        user_path = "";
        getline(cin, user_path);
        validate_input(user_path, target);
    }
    else if (part.compare("comp") == 0) {
        user_path = "";
        cout << "Please input the texture image path" << endl;
        getline(cin, user_path);
        validate_input(user_path, quilt);
    }
    else if (part.compare("chag") == 0) {
        user_path = "";       
        cout << "Please input the source image path" << endl;
        getline(cin, user_path);
        validate_input(user_path, shuffle_src);

        user_path = "";
        cout << "Please input the target image path" << endl;
        getline(cin, user_path);
        validate_input(user_path, mask);
    }


}


void validate_input(cv::String input, cv::Mat img) {
    if (input != "") {
        useDeafult = false;
        img = cv::imread(input);
        while (img.empty()) {
            cout << "Could not read image: " << input << endl;
            cout << "Enter another path..." << endl;
            input.clear();
            cin >> input;
            img = cv::imread(input);
        }
    }
    else {
        useDeafult = true;
    }
}


void load_default(std::string str) {
    if (str.compare("patchmatch") == 0) {
        if (useDeafult || source.empty() || target.empty()) {
            source = imread(source_path);
            target = imread(target_path);
        }
    }
    else if (str.compare("quilt") == 0) {
        if (useDeafult || quilt.empty()) {
            quilt = imread(quilt_path);
        }
    }
    else if (str.compare("reshuffle") == 0) {
        if (useDeafult || shuffle_src.empty() || mask.empty()) {
            shuffle_src = imread(shuffle_path);
            mask = imread(mask_path);
        }
    }
    useDeafult = true;
}

