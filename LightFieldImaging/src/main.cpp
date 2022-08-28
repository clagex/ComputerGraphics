#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/utils/logger.hpp>
#include "main.h"
#include "lightfield.h"

using namespace std;
using namespace cv;

int main(int argc, char* argv[]) {
    // set the logging level to avoid info warnings
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);


    lightfield = LightField(argv[1], 17, 17);
    cout << "The pixel value at (7, 10, 384, 768) is " << lightfield[7][10].image.at<Vec3b>(384, 768) << endl;

    // waiting for input to run the program
    cout << "Please type one of the following commands:" << endl;
    cout << "core"  << endl;
    cout << "comp" << endl;
    cout << "exit" << endl;
    cout << "waiting for input..." << endl;
    std::string input;
    cin >> input;

    // waiting for commands to run the assignment
    while (input.compare("exit") != 0) {

        if (input.compare("core") == 0) {
            doCore(); 
            waitKey(0);
            destroyAllWindows();
        }
        else if (input.compare("comp") == 0) {
            NullAperture a;
            Mat f1 = lightfield.reconstruct(a, 1);
            Mat f2 = lightfield.reconstruct(a, 1.2);
            Mat f3 = lightfield.reconstruct(a, 1.4);
            Mat f4 = lightfield.reconstruct(a, 1.6);
            Mat f5 = lightfield.reconstruct(a, 1.9);

            imshow("test", f1);
            imshow("test2", f2);
            imshow("test3", f3);
            imshow("test4", f4);
            imshow("test5", f5);
            //imwrite("recon_1.jpg", f1);
            //imwrite("recon_1.2.jpg", f2);
            //imwrite("recon_1.4.jpg", f3);
            //imwrite("recon_1.6.jpg", f4);
            //imwrite("recon_1.9.jpg", f5);
            cv::waitKey(0);
            cv::destroyAllWindows();
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

void doCore() {
    int stWidth = max_col - min_col;
    int stHeight = max_row - min_row;

    Mat raw_data(stHeight * lightfield.rows, stWidth * lightfield.cols, CV_8UC3, Scalar(0, 0, 0));
    Mat raw_40 = raw_data.clone();
    Mat raw_75 = raw_data.clone();

    Vec2f ac(533.057190, -776.880371);

    CircularAperture a40(ac, 40), a75(ac, 75);
    NullAperture aNull;

    for (int t = 0; t < stHeight; t++)
        for (int s = 0; s < stWidth; s++) 
        {
           Vec2i p(t * lightfield.rows, s * lightfield.cols);
            for (int row = 0; row < lightfield.rows; row++) 
                for (int col = 0; col < lightfield.cols; col++)
                {
                    Vec2f uv = lightfield[row][col].uv;
                    Vec2i loc(min_row + t, min_col + s);

                    if (aNull.transparent(uv)) {
                        raw_data.at<Vec3b>(p[0] + row, p[1] + col) = lightfield[row][col].image.at<Vec3b>(loc);
                    }
                    if (a40.transparent(uv)) {
                        raw_40.at<Vec3b>(p[0] + row, p[1] + col) = lightfield[row][col].image.at<Vec3b>(loc);
                    }
                    if (a75.transparent(uv)) {
                        raw_75.at<Vec3b>(p[0] + row, p[1] + col) = lightfield[row][col].image.at<Vec3b>(loc);
                    }
                }           
        }
    
    imshow("Null Aperture", raw_data);
    imshow("40 Aperture", raw_40);
    imshow("75 Aperture", raw_75);
    /*imwrite("raw_data.jpg", raw_data);
    imwrite("raw_data_40.jpg", raw_40);
    imwrite("raw_data_75.jpg", raw_75);*/
}