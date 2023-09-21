#include "U8g2lib.h"
#include "ADD_DELETE.h"
#include "MY_KEYPAD.h"
#include "EEPROM.h"
#include <SPI.h>
#include <MFRC522.h>
#include <BITMAP_LIBRARY.h>

#define SS_PIN 10
#define RST_PIN 15
#define BUTTON_UP_PIN A0 // ADC pin for UP button
#define BUTTON_SELECT_PIN A1 // ADC pin for SELECT button
#define BUTTON_DOWN_PIN A2 // ADC pin for DOWN button
#define BUTTON A3
#define LOCK 9 

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance


const int NUM_ITEMS = 3; // number of items in the list and also the number of screenshots and screenshots with QR codes (other screens)
const int MAX_ITEM_LENGTH = 10; // maximum characters for the item name
char menu_items [NUM_ITEMS] [MAX_ITEM_LENGTH] = {  // array with item names
  { "Unlock" }, 
  { "Setting" }, 
  { "QR Code" },
 };
 char menu_items1 [NUM_ITEMS] [MAX_ITEM_LENGTH] = {  // array with item names
  { "Change PW" }, 
  { "Add ID" }, 
  { "Delete ID" }, 

 };
int button_up_clicked = 0; 
int button_select_clicked = 0; 
int button_down_clicked = 0; 
int button_clicked = 0;
int item_selected = 0;
int item_sel_previous; 
int item_sel_next; 
int current_screen = 0; 

void menuScreen(int &current_screen, int &item_selected, int &button_up_clicked, int &button_down_clicked) {
  if (current_screen == 0 || current_screen == 1) {
    if (digitalRead(BUTTON_UP_PIN) == LOW && button_up_clicked == 0) {
      item_selected--;
      button_up_clicked = 1;
      if (item_selected < 0) {
        item_selected = NUM_ITEMS - 1;
      }
    } else if (digitalRead(BUTTON_DOWN_PIN) == LOW && button_down_clicked == 0) {
      item_selected++;
      button_down_clicked = 1;
      if (item_selected >= NUM_ITEMS) {
        item_selected = 0;
      }
    }
    if (digitalRead(BUTTON_UP_PIN) == HIGH && button_up_clicked == 1) {
      button_up_clicked = 0;
    }
    if (digitalRead(BUTTON_DOWN_PIN) == HIGH && button_down_clicked == 1) {
      button_down_clicked = 0;
    }
  }
}


void setup() {
  u8g2.setColorIndex(1);  // set the color to white
  u8g2.begin();
  u8g2.setBitmapMode(1);
  SPI.begin();
  mfrc522.PCD_Init();
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP); // up button
  pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP); // select button
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP); // down button
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LOCK, OUTPUT);
}

void loop() {
  if (digitalRead(BUTTON) == LOW && button_clicked == 0) {
    button_clicked = 1;
    digitalWrite(LOCK, 1);
    unlockScreen();
    delay(5000);
    digitalWrite(LOCK, 0);
  }
  if (digitalRead(BUTTON) == HIGH && button_clicked == 1) { // unclick
    button_clicked = 0;
  }

  if (checkReturnToMain()) {
    current_screen = 0;
  }
  menuScreen(current_screen, item_selected, button_up_clicked, button_down_clicked);
  if (digitalRead(BUTTON_SELECT_PIN) == LOW && button_select_clicked == 0) {
    button_select_clicked = 1;
    if (current_screen == 0) {
      if (item_selected == 0) {
        bool x = unlock();
        if (x) {
          digitalWrite(LOCK, 1);
          unlockScreen();
          delay(5000);
          digitalWrite(LOCK, 0);
        } else {
          wrongPassScreen();
          delay(2000);
        }
      } else if (item_selected == 1) {
        if (unlock()) {
          current_screen = 1;
        } else {
          wrongPassScreen();
          delay(2000);
        }
      } else if (item_selected == 2) {
        changePassword();
      } else if (item_selected == 3) {
        addTagToEEPROM(mfrc522);
      } else if (item_selected == 4) {
        removeTag(mfrc522);
      } else {
        current_screen = 0;
      }
    }
  }
  if (digitalRead(BUTTON_SELECT_PIN) == HIGH && button_select_clicked == 1) { // unclick
    button_select_clicked = 0;
  }
  item_sel_previous = item_selected - 1;
  if (item_sel_previous < 0) {
    item_sel_previous = NUM_ITEMS - 1;
  }
  item_sel_next = item_selected + 1;
  if (item_sel_next >= NUM_ITEMS) {
    item_sel_next = 0;
  }
  u8g2.clearBuffer();
  if (current_screen == 0) { // MENU SCREEN
    // selected item background
    u8g2.drawXBMP(0, 22, 128, 21, bitmap_item_sel_outline);

    // draw previous item as icon + label
    u8g2.setFont(u8g_font_7x14B);
    u8g2.drawStr(25, 15, menu_items[item_sel_previous]);
    u8g2.drawXBMP(4, 2, 16, 16, bitmap_icons[item_sel_previous]);

    // draw selected item as icon + label in bold font
    u8g2.setFont(u8g_font_7x14B);
    u8g2.drawStr(25, 15 + 20 + 2, menu_items[item_selected]);
    u8g2.drawXBMP(4, 24, 16, 16, bitmap_icons[item_selected]);

    // draw next item as icon + label
    u8g2.setFont(u8g_font_7x14B);
    u8g2.drawStr(25, 15 + 20 + 20 + 2 + 2, menu_items[item_sel_next]);
    u8g2.drawXBMP(4, 46, 16, 16, bitmap_icons[item_sel_next]);

  } else if (current_screen == 1) { // SCREENSHOTS SCREEN
    // selected item background
    u8g2.drawXBMP(0, 22, 128, 21, bitmap_item_sel_outline); // draw screenshot

    // draw previous item as icon + label
    u8g2.setFont(u8g_font_7x14B);
    u8g2.drawStr(25, 15, menu_items1[item_sel_previous]);
    u8g2.drawXBMP(4, 2, 16, 16, bitmap_icons1[item_sel_previous]);

    // draw selected item as icon + label in bold font
    u8g2.setFont(u8g_font_7x14B);
    u8g2.drawStr(25, 15 + 20 + 2, menu_items1[item_selected]);
    u8g2.drawXBMP(4, 24, 16, 16, bitmap_icons1[item_selected]);

    // draw next item as icon + label
    u8g2.setFont(u8g_font_7x14B);
    u8g2.drawStr(25, 15 + 20 + 20 + 2 + 2, menu_items1[item_sel_next]);
    u8g2.drawXBMP(4, 46, 16, 16, bitmap_icons1[item_sel_next]);
  }
  u8g2.sendBuffer();
  if (checkCard()) {
    if (checkCardInEEPROM()) {
      digitalWrite(LOCK, 1);
      unlockScreen();
      delay(5000);
      digitalWrite(LOCK, 0);
    } else {
      wrongPassScreen();
      delay(2000);
    }
  }
}
