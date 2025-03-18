#pragma once

// ✅ Prevent Windows API conflicts BEFORE including <windows.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used Windows headers
#endif  

#ifndef NOMINMAX
#define NOMINMAX  // Prevent Windows.h from overriding min/max functions
#endif  

#include <windows.h>  // ✅ Must be included FIRST before OpenCV

// 🚀 Fix OpenCV `ACCESS_MASK` conflict
#ifdef ACCESS_MASK
#undef ACCESS_MASK
#endif

#ifdef byte  // Some compilers redefine `byte`
#undef byte  
#endif

// ✅ Include OpenCV libraries AFTER Windows
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>

#include <iostream>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <thread>  // ✅ Needed for threading support

// ✅ Include dependent headers
#include "eyeStatus.h"
#include "camera.h"  // External camera handling

// 🚀 Define return values for sleep detection
enum SleepStatus {
    FACE_NOT_FOUND = -1,
    EYES_CLOSED = 0,
    EYES_OPEN = 1
};

// ✅ Declare shared resources (Ensure they are **defined** in `frameProcessor.cpp`)
extern std::queue<cv::Mat> frame_queue;
extern std::mutex frame_mutex;
extern std::condition_variable frame_cv;
extern bool processed;

extern std::queue<int> status_queue;
extern std::mutex status_mutex;
extern std::condition_variable status_cv;
extern bool loading;

class FrameProcessor
{
public:
    /// Constructor
    FrameProcessor();

    /// Starts the frame processing in a separate thread
    void start();

    /// Stops the frame processing thread
    void stop();
    
    /// Processes a single frame to detect faces and eyes
    int processFrame(cv::Mat& frame);

private:

    /// Main loop for frame processing thread
    void threadLoop();

    bool isOn = false;
    std::thread frameProcessorThread;

    // Cascade classifiers for face and eye detection
    cv::CascadeClassifier face_cascade;
    cv::CascadeClassifier eyes_cascade;

    // ✅ Ensure `EyeStatus` is properly defined
    EyeStatus blinkDetector;
};
