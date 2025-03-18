#include "camera.h"
#include <opencv2/highgui.hpp>  // ✅ REQUIRED for OpenCV window management

/*!
 * Loops while camera is on to add frames to the pipeline
 */
void Camera::threadLoop() {
    while (isOn) {
        postFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(30));  // ✅ Reduce CPU usage
    }
}

/*!
 * Gets the next available frame and passes it on to the registered callback.
 */
void Camera::postFrame() {
    if (nullptr == sceneCallback) return;

    cv::Mat cap;
    videoCapture.read(cap);

    if (cap.empty()) {
        std::cerr << "ERROR: Empty frame grabbed. Retrying in 500ms..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));  
        return;
    }

    std::cout << "Frame captured successfully." << std::endl;

    // ✅ DO NOT SHOW CAMERA FEED HERE (Handled in FrameProcessor)
    // cv::imshow("Camera Feed", cap);  // ❌ REMOVE THIS LINE

    sceneCallback->nextScene(cap);
}

/*!
 * Starts the worker thread recording
 */
void Camera::start(int deviceID, int apiID) {
    if (isOn) return;  // Prevent multiple starts
    isOn = true;

    std::cout << "Attempting to open camera " << deviceID << " with API " << apiID << std::endl;

    videoCapture.open(deviceID, apiID);
    if (!videoCapture.isOpened()) {
        std::cerr << "ERROR: Could not open camera with API " << apiID << "!" << std::endl;

        std::cerr << "Trying CAP_ANY..." << std::endl;
        videoCapture.open(deviceID, cv::CAP_ANY);
        if (!videoCapture.isOpened()) {
            std::cerr << "ERROR: Camera could not be opened with default settings!" << std::endl;
            isOn = false;  // Prevent loop execution if the camera fails
            return;
        }
    }

    // Set camera properties (optional)
    videoCapture.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    videoCapture.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    videoCapture.set(cv::CAP_PROP_FPS, 30);

    std::cout << "Camera successfully opened with deviceID " << deviceID << std::endl;
    cameraThread = std::thread(&Camera::threadLoop, this);
}

/*!
 * Frees thread resources and stops recording, must be called prior to program exit.
 */
void Camera::stop() {
    isOn = false;
    if (cameraThread.joinable()) {
        cameraThread.join();
    }

    // ✅ Ensure the camera is released properly
    videoCapture.release();
    cv::destroyAllWindows();  // ✅ Close all OpenCV windows when stopping
}
