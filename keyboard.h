#include <Keyboard.h>

#pragma once

#define DEBUG 0
#define LOOP_MSEC 5
#define DEBOUNCE_MASK 1 // 5[msec] * 2 = 10[msec] => 100[Hz]

constexpr uint8_t IN_D4 = 18;
constexpr uint8_t IN_D3 = 19;
constexpr uint8_t IN_D2 = 20;
constexpr uint8_t IN_D1 = 21;
constexpr uint8_t IN_TAPE = 4;
#define READ_IN_D4321 (PINF & 0xf0)

constexpr uint8_t OUT2_SELECT_DEVICE = 10; // 1:Keyboard, 0:Tape READ
constexpr uint8_t OUT1_SELECT_COL    = 9;  // 1:High,     0:Low
constexpr uint8_t OUT0_ROW_RESET     = 8;  // 1:reset,    0:set

void resetKeyboard();
void scanKeys();
