#pragma once

#define DEADBAND 5
#define NUM_FADERS 2
#define STBY 23

const int WIPER_PINS[NUM_FADERS] = {34, 35};
const int IN1_PINS[NUM_FADERS] = {16, 17};
const int IN2_PINS[NUM_FADERS] = {4, 5};
const int PWM_PINS[NUM_FADERS] = {18, 19};