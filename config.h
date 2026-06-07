#pragma once

#define DEADBAND 5
#define NUM_FADERS 2
#define STBY 23

const int WIPER_PINS[NUM_FADERS] = {34, 35};
const int IN1_PINS[NUM_FADERS] = {16, 17};
const int IN2_PINS[NUM_FADERS] = {4, 5};
const int PWM_PINS[NUM_FADERS] = {18, 19};

uint8_t MAC_ADDRESSES[2][6] = {{0x58, 0x8C, 0x81, 0x5A, 0x3A, 0xAC}, {0x58, 0x8C, 0x81, 0x59, 0x9F, 0x50}};