# CGRA352 A4
#### Clara Ge 300515999

## Introduction
In `src` folder, there are two files: `main.h`, `main.cpp`.<br>
The following functions defined in `main`:
```c++
// core and completion/challenge functions
void doCore(std::vector<cv::Mat> frames);
void doComp(std::vector<cv::Mat> frames);
// helper functions
// do ransac with two sets of images' points
cv::Mat ransac(std::vector<cv::DMatch> matches, 
std::vector<cv::Point2f> points_1, std::vector<cv::Point2f>points_2);
// find the best homography with two images
cv::Mat findH(cv::Mat img_1, cv::Mat img_2);
// warp the image with the best homography, 
// adding a green border to fit the image size if needed.
cv::Mat warpImg(cv::Mat img, cv::Mat H);
```

## How to run the program
You could just run the program. You can also change the image folder path in `main.h, line 10: std::string path`, for example in my computer is `"res\\frames\\"`<br>
After running `main.cpp`, it will auto load the folder with images, after finishes loading, the program will ask for one of the following input:
- core (feature matching and homography, 3 images)
- comp (video stabilizer, line 277 to show all, line 278 to write all images in local computer)<br>You can change the start frame and end frame at line 237 and 238.
- exit<br>
Completion/Challenge takes some time to compute.

## Results
##### Core
Core Part 1<br>
![raw_data](../output/core_1.jpg){width=500}<br>
Core Part 2<br>
![raw_data](../output/core_2.jpg){width=500}<br>
Core Part 3, each image has 0.5 alpha value and overlays<br>
![raw_data](../output/core_3.jpg){width=500}<br>
##### Completion/Challenge
All of the stabilized frames<br>
![raw_data](../output/StableFrames.png)<br>
