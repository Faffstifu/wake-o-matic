#include "frameProcessor.h"

// ✅ Windows API Fixes
#include <windows.h>  // Ensure Windows headers are included first

// 🚀 Fix OpenCV `ACCESS_MASK` conflict
#ifdef ACCESS_MASK
#undef ACCESS_MASK
#endif

#ifdef byte  // Some compilers redefine `byte`
#undef byte  
#endif

#include <opencv2/core.hpp>  
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <chrono>
using namespace std::chrono;

// 🚀 Track eye closure duration
static auto eyeCloseStart = steady_clock::now();
static bool wasEyeOpen = true;

// 🚀 Constructor: Loads Haar cascades properly
FrameProcessor::FrameProcessor() {
    std::cout << "Loading Haar cascades..." << std::endl;

    // Load Haar cascade paths using CMake definitions
    const std::string faceCascadePath = FACE_CASCADE_PATH;
    const std::string eyesCascadePath = EYES_CASCADE_PATH;

    if (!face_cascade.load(faceCascadePath)) {
        throw std::runtime_error("❌ ERROR: Could not load face cascade! Check path: " + faceCascadePath);
    }
    std::cout << "✅ SUCCESS: Face cascade loaded!" << std::endl;

    if (!eyes_cascade.load(eyesCascadePath)) {
        throw std::runtime_error("❌ ERROR: Could not load eye cascade! Check path: " + eyesCascadePath);
    }
    std::cout << "✅ SUCCESS: Eye cascade loaded!" << std::endl;
}

// 🚀 Start function (Thread initialization)
void FrameProcessor::start() {
    if (isOn) return;  // Prevent multiple starts
    isOn = true;
    frameProcessorThread = std::thread(&FrameProcessor::threadLoop, this);
}

// 🚀 Stop function (Thread cleanup)
void FrameProcessor::stop() {
    isOn = false;
    if (frameProcessorThread.joinable()) {
        frameProcessorThread.join();
    }
    cv::destroyAllWindows();
}

// 🚀 Processes a single frame to detect faces and eyes
int FrameProcessor::processFrame(cv::Mat& frame) {
    if (frame.empty()) {
        std::cerr << "❌ ERROR: Received an empty frame. Skipping processing." << std::endl;
        return FACE_NOT_FOUND;
    }

    if (face_cascade.empty() || eyes_cascade.empty()) {
        std::cerr << "❌ ERROR: Haar cascades are not loaded. Check paths!" << std::endl;
        return FACE_NOT_FOUND;
    }

    bool eyeStatus = false;
    std::vector<cv::Rect> faces;

    face_cascade.detectMultiScale(frame, faces, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(100, 100));

    static int noFaceCounter = 0;
    if (faces.empty()) {
        if (++noFaceCounter % 30 == 0) {  // Reduce excessive logging
            std::cerr << "⚠️ WARNING: No face detected!" << std::endl;
        }
        return FACE_NOT_FOUND;
    }
    noFaceCounter = 0;

    for (const auto& face : faces) { 
        cv::rectangle(frame, face, cv::Scalar(255, 0, 0), 2);  // Draw face rectangle

        cv::Mat faceROI = frame(face).clone();
        std::vector<cv::Rect> eyes;
        eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 4, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

        for (const auto& eye : eyes) {
            cv::rectangle(faceROI, eye, cv::Scalar(0, 255, 0), 2);  // Draw eye rectangles
            cv::Mat eyeROI = faceROI(eye).clone();

            if (blinkDetector.detect(eyeROI)) {
                eyeStatus = true;
            }
        }
    }

    // 🚀 Show the processed frame
    cv::imshow("Detection Debug", frame);
    
    // 🚀 Quit program when ESC is pressed
    char key = (char)cv::waitKey(10);
    if (key == 27) { // ESC key to quit
        std::cout << "🛑 Exit command received! Stopping wake-o-matic..." << std::endl;
        stop();  // Stop the processing thread
        exit(0); // Clean exit
    }

    // 🚀 Track eye closure duration for microsleep detection
    if (!eyeStatus) {
        if (wasEyeOpen) {
            eyeCloseStart = steady_clock::now(); // Start timing when eyes first close
            wasEyeOpen = false;
        }
    
        auto duration = duration_cast<milliseconds>(steady_clock::now() - eyeCloseStart).count();
        
        if (duration > 1500) { // 1.5 seconds threshold for microsleep
            std::cout << "⚠️ Microsleep detected! Eyes are closed for too long!" << std::endl;
            return EYES_CLOSED;
        }
    } else {
        wasEyeOpen = true; // Reset when eyes are open
    }

    return EYES_OPEN; // ✅ Ensure function always returns a value
}

// 🚀 Thread loop for continuous frame processing
void FrameProcessor::threadLoop() {
    cv::VideoCapture cap(0);  // Open the default camera

    if (!cap.isOpened()) {
        std::cerr << "❌ ERROR: Could not open camera!" << std::endl;
        return;
    }

    cv::Mat frame;
    while (isOn) {
        cap >> frame;  // Capture frame
        if (frame.empty()) continue;

        int status = processFrame(frame);  // Process the captured frame

        if (status == EYES_CLOSED) {
            std::cout << "⚠️ ALERT: Microsleep detected!" << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(30));  // Limit frame rate
    }

    cap.release();
}
