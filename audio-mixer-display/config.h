#pragma once

uint8_t masterMac[] = {0xA4, 0xF0, 0x0F, 0x90, 0x2E, 0x34};

// True = 1; False = 0;
#define ENABLE_BUTTONS 1

#define BUTTON_ROW_COUNT 2
#define BUTTON_COL_COUNT 3

const int BUTTON_ROWS[BUTTON_ROW_COUNT] = {5, 6};
const int BUTTON_COLS[BUTTON_COL_COUNT] = {7, 8, 9};

const int BUTTON_IDS[BUTTON_ROW_COUNT][BUTTON_COL_COUNT] = {
  {0, 1, 2},
  {3, 4, 5}
};