#include "tests.h"

void cameraTest() {
    cv::Mat img;
    cv::VideoCapture cap;
    cap.open(0);

    if (!cap.isOpened()) {
        std::cerr << "ERROR: Camera opening failed!" << std::endl;
    }

    assertm(cap.isOpened(), "Camera opening failed");
    cap.read(img);

    if (img.empty()) {
        std::cerr << "ERROR: Captured image is empty!" << std::endl;
    }

    assertm(!img.empty(), "Image is empty");
    return;
}

void eyeStatusTest() {
    cv::Mat closed = cv::imread("../../../test/images/eyeClosed.jpg");
    cv::Mat open = cv::imread("../../../test/images/eyeOpen.jpg");

    if (closed.empty()) {
        std::cerr << "ERROR: Unable to load eyeClosed.jpg. Check path!" << std::endl;
    }
    if (open.empty()) {
        std::cerr << "ERROR: Unable to load eyeOpen.jpg. Check path!" << std::endl;
    }

    assertm(!closed.empty(), "Image of a closed eye is empty");
    assertm(!open.empty(), "Image of an open eye is empty");

    EyeStatus blinkDetector;

    bool statusClosed = blinkDetector.detect(closed);
    bool statusOpen = blinkDetector.detect(open);

    assertm(!statusClosed, "Closed eye image falsely detected as open eye");
    assertm(statusOpen, "Open eye image falsely detected as closed eye");
    return;
}

void frameProcessorTest() {
    cv::Mat noface = cv::imread("../../../test/images/noface.jpg");
    cv::Mat face_closedeyes = cv::imread("../../../test/images/face_closedeyes.jpg");
    cv::Mat face_openeyes = cv::imread("../../../test/images/face_openeyes.jpg");

    if (noface.empty()) {
        std::cerr << "ERROR: Unable to load noface.jpg. Check path!" << std::endl;
    }
    if (face_closedeyes.empty()) {
        std::cerr << "ERROR: Unable to load face_closedeyes.jpg. Check path!" << std::endl;
    }
    if (face_openeyes.empty()) {
        std::cerr << "ERROR: Unable to load face_openeyes.jpg. Check path!" << std::endl;
    }

    FrameProcessor processor;  // Fixed variable name

    int statusNoFace = processor.processFrame(noface);
    int statusClosedEyes = processor.processFrame(face_closedeyes);
    int statusOpenEyes = processor.processFrame(face_openeyes);

    assertm(FACE_NOT_FOUND == statusNoFace, "Incorrect detection for picture noface.jpg");
    assertm(EYES_CLOSED == statusClosedEyes, "Incorrect detection for picture face_closedeyes.jpg");
    assertm(EYES_OPEN == statusOpenEyes, "Incorrect detection for picture face_openeyes.jpg");

    return;
}
