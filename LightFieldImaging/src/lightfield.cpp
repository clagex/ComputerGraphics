#include "lightfield.h"
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

LightField::LightField(std::string filepath, int rows_, int cols_) : m_images(rows_* cols_, stImage()), rows(rows_), cols(cols_) {
    std::cout << "Loading light field ..." << std::endl;

    std::vector<cv::String> lf_imgs;
    cv::glob(filepath, lf_imgs);

    // parse
    for (cv::String cv_str : lf_imgs) {

        // get the filename
        std::string filepath(cv_str);
        size_t pos = filepath.find_last_of("/\\");
        if (pos != std::string::npos) {
            // replace "_" with " "
            std::string filename = filepath.substr(pos + 1);
            pos = 0;
            while ((pos = filename.find("_", pos)) != std::string::npos) {
                filename.replace(pos, 1, " ");
                pos++;
            }
            // parse for values
            std::istringstream ss(filename);
            std::string name;
            int row, col;
            float v, u;
            ss >> name >> row >> col >> v >> u;
            if (ss.good()) {
                (*this)[row][col].uv = Vec2f(u, v);
                (*this)[row][col].image = imread(filepath, 1);
                if (!(*this)[row][col].image.data) {
                    cout << "Could not open or find the image: " << filepath << endl;
                    abort();
                }
                continue;
            }
        }
        // throw error otherwise
        std::cerr << "Filepath error with : " << filepath << std::endl;
        std::cerr << "Expected in the form : [prefix]/[name]_[row]_[col]_[v]_[u][suffix]";
        abort();
    }
    std::cout << "Finished loading light field" << std::endl;
}

Mat LightField::reconstruct(Aperture& aperture, float focalDistance) {
    Size size = (*this)[rows / 2][cols / 2].image.size();
    Vec2f centre_uv = (*this)[rows / 2][cols / 2].uv;

    Mat recon(size, CV_32FC3, Scalar(0., 0., 0.));
    Mat index(size, CV_32FC2); //

    int count = 0;
    for (stImage& img : m_images) {
        Mat temp_dst;
        
        if (aperture.transparent(img.uv)) {
            Vec2f uv = img.uv- centre_uv;
            count++;
            for (int t = 0; t < size.height; t++)
                for (int s = 0; s < size.width; s++)
                {
                    Vec2f st_prime(s, t);
                    index.at<Vec2f>(t, s) = uv + (focalDistance * st_prime - uv) / focalDistance;
                }
            
            remap(img.image, temp_dst, index, Mat(), INTER_LINEAR, BORDER_REPLICATE);
        }
        recon += temp_dst;
    }

    recon /= count;
    recon.convertTo(recon, CV_8UC3);
    return recon;

}