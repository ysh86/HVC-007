// build:
// $ arduino-cli lib install Keyboard
// $ arduino-cli compile -b arduino:avr:micro --build-cache-path ./build --build-path ./build ../HVC-007
//
// program:
// $ avrdude.exe -p m32u4 -c avr109 -P COM3 -U flash:w:./build/HVC-007.ino.hex:i

#include "keyboard.h"

void setup() {
    Serial.begin(9600); // Should be always ON (for programmer)

    // init all pins to input/pull-up
    PORTB |= 0b11111111; // pull-up: B0-7
    DDRB &= ~0b11111111; // input: B0-7
    PORTC |= 0b11000000; // pull-up: C6,7
    DDRC &= ~0b11000000; // input: C6,7
    PORTD |= 0b11011111; // pull-up: D0-4,6,7
    DDRD &= ~0b11011111; // input: D0-4,6,7
    PORTE |= 0b01000000; // pull-up: E6
    DDRE &= ~0b01000000; // input: E6
    PORTF |= 0b11110011; // pull-up: F0,1,4-7
    DDRF &= ~0b11110011; // input: F0,1,4-7

    // init in pins
    pinMode(IN_D4, INPUT);
    pinMode(IN_D3, INPUT);
    pinMode(IN_D2, INPUT);
    pinMode(IN_D1, INPUT);
    pinMode(IN_TAPE, INPUT_PULLUP); // TBD

    // init out pins
    pinMode(OUT2_SELECT_DEVICE, OUTPUT);
    pinMode(OUT1_SELECT_COL,    OUTPUT);
    pinMode(OUT0_ROW_RESET,     OUTPUT);

    resetKeyboard();

#if !DEBUG
    Keyboard.begin();
#endif
}

void loop() {
    scanKeys();
    delay(LOOP_MSEC);
}
