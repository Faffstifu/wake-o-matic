#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
    std::string face_cascade_path = "C:/Users/faris/wake-o-matic/src/data/haarcascade_frontalface_default.xml";
    
    cv::CascadeClassifier face_cascade;
    
    if (!face_cascade.load(face_cascade_path)) {
        std::cerr << "ERROR: OpenCV cannot load the cascade file! Check path." << std::endl;
        return -1;
    } else {
        std::cout << "SUCCESS: OpenCV loaded the cascade file correctly!" << std::endl;
    }
    
    return 0;
}
