#include "MY_KEYPAD.h"


char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {7,2,3,5};
byte colPins[COLS] = {6,8,4};
/*byte rowPins[ROWS] = {25, 14, 13, 23};
byte colPins[COLS] = {24, 26, 12};*/

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

char password[5] = "1234";

void changePassword() {
  // Khai báo bi?n password và password_new
  char password_new[5] = "";
  
  // Hi?n th? m?t kh?u m?c d?nh lên OLED
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_7x14B_tf);
  u8g2.setCursor(2, 20);
  u8g2.print(F("New password:"));
  u8g2.sendBuffer();

  // Nh?p m?t kh?u m?i t? keypad
  int index = 0;
  while (index < 4) {
    char key = keypad.waitForKey();
    if (key >= '0' && key <= '9') {
      password_new[index++] = key;
      u8g2.setCursor(20 + index * 8, 40);
      u8g2.print(key);
      u8g2.sendBuffer();
    } else if (key == '#') {
    return;
  }
  }

  // Ki?m tra n?u m?t kh?u m?i trùng v?i m?t kh?u hi?n t?i, hi?n th? thông báo và thoát kh?i hàm
  if (strcmp(password_new, password) == 0) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_7x14B_tf);
    u8g2.setCursor(4, 20);
    u8g2.print(F("New password"));
    u8g2.setCursor(4, 35);
    u8g2.print(F("cannot be the same"));
    u8g2.setCursor(4, 50);
    u8g2.print(F("as the old password"));
    u8g2.sendBuffer();
    delay(2000);
    return;
  }

  // Hi?n th? m?t kh?u m?i lên OLED
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_7x14B_tf);
  u8g2.setCursor(4, 15);
  u8g2.print(F("New password:"));
  u8g2.print(password_new);  
  u8g2.setCursor(4, 35);
  u8g2.print(F("Change password?"));
  u8g2.setCursor(4, 55);
  u8g2.print(F("1. Yes  2. No"));
  u8g2.sendBuffer();

  // Nh?p l?a ch?n t? keypad
  char choice = keypad.waitForKey();
  while (choice != '1' && choice != '2') {
    choice = keypad.waitForKey();
  }

  // Thay d?i m?t kh?u n?u ngu?i dùng ch?n "Yes"
  if (choice == '1') {
    strcpy(password, password_new);
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_7x14B_tf);
    u8g2.setCursor(4, 20);
    u8g2.print(F("Password changed!"));
    u8g2.sendBuffer();
    for (int i = 0; i < 4; i++) {
      EEPROM.write(i + 100, password_new[i]);
    } 
    delay(2000);
  }
}


bool unlock() {
  char passwordBuffer[5] = "";
  char key;

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_7x14B_tf);
  u8g2.setCursor(2, 20);
  u8g2.print(F("Enter password:"));
  u8g2.setCursor(20, 40);
  u8g2.sendBuffer();

  while (strlen(passwordBuffer) != 4) {
    key = keypad.waitForKey();
    if (key >= '0' && key <= '9') {
      passwordBuffer[strlen(passwordBuffer)] = key;
      u8g2.setCursor(20, 40);
      u8g2.print(passwordBuffer);
      u8g2.sendBuffer();
    }else if (key == '#') {
    return false;
  }
  }
  for (int i = 0; i < 4; i++) {
    if (passwordBuffer[i] != EEPROM.read(i + 100)) {
      return false;
    }
  }
  return true;
 // return (strcmp(passwordBuffer, password) == 0);
}

bool checkReturnToMain() {
  char key = keypad.getKey();
  if (key == '#') {
    return true;
  } else {
    return false;
  }
}

void unlockScreen() {
  u8g2.clearDisplay();
  u8g2.setFont(u8g2_font_7x14B_tf);
  u8g2.setCursor(45, 15);
  u8g2.print(F("Unlock"));
  u8g2.drawXBMP(24, 20, 80, 40, epd_bitmap_SE);
  u8g2.sendBuffer();
}

void wrongPassScreen() {
  u8g2.clearDisplay();
  u8g2.setFont(u8g2_font_7x14B_tf);
  u8g2.setCursor(30, 15);
  u8g2.print(F("Wrong Pass"));
  u8g2.drawXBMP(24, 20, 80, 40, epd_bitmap_SE);
  u8g2.sendBuffer();
}
