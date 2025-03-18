#include "cppTests.h"
#include <cassert>
#include "../../src/modules/sleepDetect.h"
#ifdef _WIN32
    #include <windows.h>
    #define sleep(x) Sleep(1000 * (x))  // Windows uses Sleep(ms)
#else
    #include <unistd.h>  // Unix/Linux systems
#endif


bool test_action_activated_by_state_sleeping(){
    ActionStateMachine myAction;
    myAction.changeState(0);
    assertm((myAction.getState() == 0),"ActionStateMachine.changeState(0) did not work");
    return true;
}

bool test_action_deactivated_by_state_awake(){
    ActionStateMachine myAction;
    myAction.changeState(1);
    assertm((myAction.getState() != 0),"ActionStateMachine.changeState(1) did not work");
    return true;
}