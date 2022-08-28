# CGRA352 A3
#### Clara Ge 300515999

## Introduction
In `src` folder, there are four files: `main.h`, `main.cpp`, `lightfield.h`, `lightfield.cpp`.<br>
The following functions defined in `main`:
```c++
// load the images first, then waits for input to do core and completion
int main(int argc, char* argv[])
// does the Generate ST-array task
void doCore();
```
The following struct/class and functions defined in `lightfield`:
```c++
// structure storing the image and uv
struct stImage {};
// lightfield class, 
// load the images and do the completion/challenge in reconstruct
class LightField {
	LightField();
	LightField(int rows_, int cols_);
	LightField(std::string filepath, int rows_, int cols_);
	stImage* operator[] (int row) { return &(m_images.at(row * cols)); }
	const stImage* operator[] (int row) const;
	Mat reconstruct(Aperture& aperture, float focalDistance = 1.f);
};

// Aperture classes
class Aperture {};
class NullAperture : public Aperture {}
class CircularAperture : public Aperture {};
```

## How to run the program
Due to submission's file size limit, the images are not included. The program takes 1 argument as the folder path, for example the argument in my computer is `"res//rectified"`<br>
After running `main.cpp`, it will auto load the folder with images, after finishes loading, the program will ask for one of the following input:
- core (generates ST-array, 3 images)
- comp (focal stack, 5 images)
- exit
The loading and comp takes some time to compute.

## Results
##### Core
Raw data, null aperture<br>
![raw_data](../output/raw_data.jpg){width=500}<br>
Raw data, 40 aperture<br>
![raw_data](../output/raw_data_40.jpg){width=500}<br>
Raw data, 75 aperture<br>
![raw_data](../output/raw_data_75.jpg){width=500}<br>
##### Completion/Challenge
reconstruction, focal distance = 1.0<br>
![raw_data](../output/recon_1.jpg){width=500}<br>
reconstruction, focal distance = 1.2<br>
![raw_data](../output/recon_1.2.jpg){width=500}<br>
reconstruction, focal distance = 1.4<br>
![raw_data](../output/recon_1.4.jpg){width=500}<br>
reconstruction, focal distance = 1.6<br>
![raw_data](../output/recon_1.6.jpg){width=500}<br>
reconstruction, focal distance = 1.9<br>
![raw_data](../output/recon_1.9.jpg){width=500}<br>