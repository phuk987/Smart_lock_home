#ifndef ADD_DELETE_h
#define ADD_DELETE_h

#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
#include <U8g2lib.h> 


extern MFRC522 mfrc522;

extern U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2;

void addTagToEEPROM(MFRC522 &mfrc522);
void removeTag(MFRC522 &mfrc522);
bool checkCard();
bool checkCardInEEPROM();

#endif
