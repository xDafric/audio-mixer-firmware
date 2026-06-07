#pragma once
#include "config.h"
#include "Arduino.h"
#include "packet.h"

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

    uint8_t macAddress[6];

    int readWiper();

    void updateFader();

    void moveUp(int speed);
    void moveDown(int speed);
    void stopMotor();
    void brakeMotor();

  public:
    Fader(int index, int IN1, int IN2, int PWM, int WIPER, uint8_t macAddress[]);

    void setupESPNow();
    void sendEvent(MasterEvent event, String payload);

    void moveTo(int pos);
    
    bool update(int& outValue);
};