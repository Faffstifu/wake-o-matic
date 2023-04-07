#include "action.h"

void Action::threadLoop() {
    while(deviceActive){
        if(flagChanged == true){
            flagChanged = false;
            if(nullptr == actionCallback) return;
            actionCallback->nextAction(warningFlag, sleepingFlag);
        }
    }
}

void Action::activate(){
    deviceActive = true;
    actionThread = std::thread(&Action::threadLoop, this);
}

void Action::deactivate(){
    deviceActive = false;
    actionThread.join();
}

void Action::setSleepingFlag(bool newVal){
    sleepingFlag = newVal;
    flagChanged = true;
}

void Action::setWarningFlag(bool newVal){
    warningFlag = newVal;
    flagChanged = true;
}

void outputAlarm(){
    std::system("amixer set PCM unmute");
    // std::system("amixer set PCM 100%");
    std::system("aplay ../wav/alarm.wav");
}

void outputWarning(){
    std::system("amixer set PCM unmute");
    // std::system("amixer set PCM 100%");
    std::system("aplay ../wav/warning.wav");
}