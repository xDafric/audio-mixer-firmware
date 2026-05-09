#pragma once
#include "config.h"

class Fader {
  private:
    int index;

    int IN1;
    int IN2;
    int PWM;
    int WIPER;

    int pos;
    int value;

    bool moving;
    int target;
    int delayCount;


    int readWiper();

    void updateFader();

    void moveUp(int speed);
    void moveDown(int speed);
    void stopMotor();
    void brakeMotor();

  public:
    Fader(int index, int IN1, int IN2, int PWM, int WIPER);

    void moveTo(int pos);
    
    bool update(int& outValue);
};