#ifndef MY_KEYPAD_H
#define MY_KEYPAD_H

#include <U8g2lib.h> // Thư viện OLED
#include <Keypad.h> // Thư viện Keypad
#include <EEPROM.h>
#include <BITMAP_LIBRARY.h>

extern U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2;

const int ROWS = 4;
const int COLS = 3;
extern char keys[ROWS][COLS];

extern Keypad keypad;

extern char password[5];

void changePassword();

void unlockScreen();

void wrongPassScreen();

bool unlock();

bool checkReturnToMain();

#endif