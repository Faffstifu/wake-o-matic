#pragma once

#include <thread>
#include "sleepDetect.h"  // ✅ Ensure AWAKE, SLEEPING, NOFACE are properly defined
#include <iostream>
#include <mutex>
#include <condition_variable>

// 🚀 Global mutex and condition variable for action state machine
extern std::mutex action_mutex;
extern std::condition_variable action_cv;

/** 
 * @brief Class that handles the appropriate action depending on sleep status.
 * 
 * - Plays **warning** when the driver is not detected.
 * - Plays **alarm** when the driver is asleep.
 * 
 * ### USAGE:
 * - Call `changeState(int state)` to update the state.
 * - The class will automatically handle the appropriate actions.
 */
class ActionStateMachine
{
public:

    /** 
     * @brief Changes the current state of the state machine and performs the appropriate action.
     * @param state The new state to change to.
     */
    void changeState(int state);

    /** 
     * @brief Get the current state.
     * @return int The current state.
     */
    int getState() {
        return currentState;
    }

    /** Constructor */
    ActionStateMachine() = default;

    /** Destructor (Ensures thread stops when object is destroyed) */
    ~ActionStateMachine() { stop(); }  

private:
    void start();
    void stop();
    void doAction(int sleepStatus);
    void outputAlarm();
    void outputWarning();
    void threadLoop();

    bool isOn = false;
    int currentState = AWAKE;  // ✅ Ensure AWAKE is defined in `sleepDetect.h`
    std::thread actionThread;
};

