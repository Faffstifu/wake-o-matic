// ✅ Prevent Windows.h conflicts BEFORE including it
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

#include <opencv2/opencv.hpp>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include "modules/eyeStatus.h"
#include "modules/camera.h"
#include "modules/frameProcessor.h"
#include "modules/sleepDetect.h"
#include "modules/actionStateMachine.h"
#include <mutex>
#include <condition_variable>
#include "modules/logging.h"

using namespace std;
using namespace cv;

// ✅ Queue, mutex, and CV for raw frames
std::queue<Mat> frame_queue;
std::mutex frame_mutex;
std::condition_variable frame_cv;
bool processed = true;

// ✅ Queue, mutex, and CV for eye status
std::queue<int> status_queue;
std::mutex status_mutex;
std::condition_variable status_cv;
bool loading = false;

// ✅ Mutex and CV for action state machine
std::mutex action_mutex;
std::condition_variable action_cv;

// ✅ Sleep status (-1 no face, 0 asleep, 1 awake)
int sleepStatus = NOFACE;

// ✅ Limits
const int MAX_QUEUE_SIZE = 10;

// 🚀 Callback structure for camera
struct MyCallback : Camera::SceneCallback {
    void nextScene(const cv::Mat& frame) override {
        std::unique_lock<std::mutex> lock(frame_mutex);
        frame_cv.wait(lock, [] { return processed; });  
        
        if (frame_queue.size() > MAX_QUEUE_SIZE) {
            std::cerr << "⚠️ WARNING: Frame queue is full, dropping oldest frame." << std::endl;
            frame_queue.pop();  // Drop the oldest frame
        }

        frame_queue.push(frame);
        processed = false;
        lock.unlock();
        frame_cv.notify_one();
    }
};

int main() {
    Logger MainLogger;
    Logger::logMessage(Logger::custom_severity_level::info, "✅ Logging Started");

    // ✅ Create objects
    Camera camera;
    MyCallback cb;
    FrameProcessor frameProcessor;
    SleepDetect sleepDetector;
    ActionStateMachine action;

    // ✅ Register callback and start camera
    camera.registerSceneCallback(&cb);
    camera.start(0, cv::CAP_ANY);
    std::cout << "✅ Camera started" << std::endl;

    // ✅ Start frame processing thread
    frameProcessor.start();
    std::cout << "✅ Frame processing started" << std::endl;
    std::cout << "✅ Action state machine started" << std::endl;

    for (int i = 0; i < 10; i++) {
        std::unique_lock<std::mutex> lock_buffer(status_mutex);

        if (!status_cv.wait_for(lock_buffer, std::chrono::seconds(5), [] { return !status_queue.empty(); })) {
            std::cerr << "⚠️ WARNING: Timed out waiting for status_queue updates." << std::endl;
            continue;
        }

        loading = true;

        while (!status_queue.empty()) {
            sleepDetector.load(status_queue.front());
            status_queue.pop();
        }

        loading = false;
        lock_buffer.unlock();
        status_cv.notify_one();

        // ✅ Print every 30 frames
        static int frameCount = 0;
        if (++frameCount % 30 == 0) {
            std::cout << "🔵 Sleep status: " << sleepStatus << std::endl;
        }

        // ✅ Perform corresponding action
        action.changeState(sleepStatus);

        // ✅ Sleep for 1 second
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // ✅ Stop camera and frame processor
    std::cout << "🛑 Stopping camera and frame processor..." << std::endl;
    action.changeState(AWAKE);
    camera.stop();
    frameProcessor.stop();
    std::this_thread::sleep_for(std::chrono::seconds(1));  // ✅ Ensure cleanup

    return 0;
}
