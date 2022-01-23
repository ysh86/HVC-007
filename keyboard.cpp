#include "keyboard.h"

struct key_state {
    const unsigned char code;

    // signal from keyboard
    char prev;
    char curr;
    char debounce;

    // ctor
    key_state(const unsigned char c)
    : code(c)
    , prev(0)
    , curr(0)
    , debounce(0) {
    }
};

static key_state keys[] = {
    KEY_F8,
    KEY_RETURN,
    ']', // [
    '\\', // ]
    KEY_RIGHT_ALT, // kana
    KEY_RIGHT_SHIFT,
    '`', // ¥
    KEY_BACKSPACE, // STOP

    KEY_F7,
    '[', // @
    '\'', // :
    ';',
    KEY_LEFT_ALT, // _
    '/',
    '-',
    '=', // ^

    KEY_F6,
    'o',
    'l',
    'k',
    '.',
    ',',
    'p',
    '0',

    KEY_F5,
    'i',
    'u',
    'j',
    'm',
    'n',
    '9',
    '8',

    KEY_F4,
    'y',
    'g',
    'h',
    'b',
    'v',
    '7',
    '6',

    KEY_F3,
    't',
    'r',
    'd',
    'f',
    'c',
    '5',
    '4',

    KEY_F2,
    'w',
    's',
    'a',
    'x',
    'z',
    'e',
    '3',

    KEY_F1,
    KEY_TAB, // KEY_ESC,
    'q',
    KEY_LEFT_CTRL,
    KEY_LEFT_SHIFT,
    KEY_LEFT_GUI, // GRPH
    '1',
    '2',

    KEY_ESC, // KEY_HOME, // CLR HOME
    KEY_UP_ARROW,
    KEY_RIGHT_ARROW,
    KEY_LEFT_ARROW,
    KEY_DOWN_ARROW,
    ' ',
    KEY_DELETE,
    KEY_INSERT
};

static void updateAndSendKey(int k);


void resetKeyboard() {
    // 4016: out 0x05
    digitalWrite(OUT2_SELECT_DEVICE, HIGH);
    digitalWrite(OUT1_SELECT_COL, LOW);
    digitalWrite(OUT0_ROW_RESET, HIGH);

    // NOP x6 @ 1.79 MHz
    delayMicroseconds(10);

    // 4016: out 0x04
    digitalWrite(OUT0_ROW_RESET, LOW);

    // NOP x20 @ 1.79 MHz
    delayMicroseconds(30);
}

static void selectColHi() {
    // 4016: out 0x06
    digitalWrite(OUT1_SELECT_COL, HIGH);

    // NOP x20 @ 1.79 MHz
    delayMicroseconds(30);
}

static void nextRow() {
    // 4016: out 0x04
    digitalWrite(OUT1_SELECT_COL, LOW);

    // NOP x20 @ 1.79 MHz
    delayMicroseconds(30);
}

void scanKeys() {
    resetKeyboard();

    for (int x = 0; x < 9; x++) {
        unsigned char data;
        // Lo
        data = READ_IN_D4321 >> 4;
        // Hi
        selectColHi();
        data |= READ_IN_D4321;

        /* original:
        unsigned char temp;
        temp = data ^ 0xff;
        data = 0;
        data |= (temp & 0b0000_0001) << 7;
        data |= (temp & 0b0000_0010) << 5;
        data |= (temp & 0b0000_0100) << 3;
        data |= (temp & 0b0000_1000) << 1;
        data |= (temp & 0b0001_0000) >> 1;
        data |= (temp & 0b0010_0000) >> 3;
        data |= (temp & 0b0100_0000) >> 5;
        data |= (temp & 0b1000_0000) >> 7;
        */

        keys[8*x+0].curr = data & 1;
        keys[8*x+1].curr = (data >> 1) & 1;
        keys[8*x+2].curr = (data >> 2) & 1;
        keys[8*x+3].curr = (data >> 3) & 1;
        keys[8*x+4].curr = (data >> 4) & 1;
        keys[8*x+5].curr = (data >> 5) & 1;
        keys[8*x+6].curr = (data >> 6) & 1;
        keys[8*x+7].curr = (data >> 7) & 1;

        nextRow();
    }

    // update modifiers
    updateAndSendKey(4);  // KEY_RIGHT_ALT (kana)
    updateAndSendKey(5);  // KEY_RIGHT_SHIFT
    updateAndSendKey(59); // KEY_LEFT_CTRL
    updateAndSendKey(60); // KEY_LEFT_SHIFT
    updateAndSendKey(61); // KEY_LEFT_GUI (GRPH)

    // update keys
    for (int i = 0; i < 72; i++) {
        updateAndSendKey(i);
    }
}

static void updateAndSendKey(int k) {
    key_state &key = keys[k];

    if (key.debounce != 0) {
        key.debounce = (key.debounce + 1) & DEBOUNCE_MASK;
        return;
    }

    int edge = (key.prev << 1) | key.curr;
    if (edge == 0b01) { // pos edge
#if DEBUG
        Serial.print("Pressed: ");
        Serial.println(key.code);
#else
        Keyboard.press(key.code);
#endif
        key.debounce++;
        key.prev = key.curr;
    } else if (edge == 0b10) { // neg edge
#if DEBUG
        Serial.print("Released: ");
        Serial.println(String(key.code));
#else
        Keyboard.release(key.code);
#endif
        key.debounce++;
        key.prev = key.curr;
    }
}
