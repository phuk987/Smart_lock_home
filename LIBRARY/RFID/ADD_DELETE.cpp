#include "ADD_DELETE.h"
 
byte tagID1[4];
bool isCardDetected = false; 

bool checkCard() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {

    for (int i = 0; i < 4; i++) {
      tagID1[i] = mfrc522.uid.uidByte[i];
    }

    return true;
  } else {
    // Tr? v? false d? xác d?nh r?ng không có th? du?c quét
    return false;
  }
}

void addTagToEEPROM(MFRC522 &mfrc522) {
  int tagAddress = -1;
  bool tagExists = false;
  byte tagData[MFRC522::MF_KEY_SIZE];
  unsigned long startTime = millis();
  bool cardDetected = false;	
  u8g2.clearDisplay();
  u8g2.setFont(u8g2_font_7x14B_tf);
  u8g2.setCursor(4, 30);
  u8g2.print(F("Please scan")); 
  u8g2.setCursor(10, 50);
  u8g2.print(F("new ID"));   
  u8g2.sendBuffer();

  
	
	while (millis() - startTime <= 10000) {
	  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
	    cardDetected = true;
	    break;
	  }
	}
	
	if (!cardDetected) {
	  u8g2.clearDisplay();
	  u8g2.setFont(u8g2_font_7x14B_tf); 
	  u8g2.setCursor(4, 30);
	  u8g2.print(F("No card"));
	  u8g2.setCursor(10, 50);
	  u8g2.print(F("detected"));
	  u8g2.sendBuffer();
	  delay(2000);
	  return;
	}

	memcpy(tagData, mfrc522.uid.uidByte, mfrc522.uid.size);

  // Check if the tag already exists in EEPROM
  for (int i = 0; i < EEPROM.length(); i += mfrc522.uid.size) {
    bool tagFound = true;
    for (int j = 0; j < mfrc522.uid.size; j++) {
      if (EEPROM.read(i + j) != tagData[j]) {
        tagFound = false;
        break;
      }
    }
    if (tagFound) {
        u8g2.clearDisplay();
  	  	u8g2.setFont(u8g2_font_7x14B_tf); 
    	u8g2.setCursor(4, 30);
		u8g2.print(F("Tag already"));
    	u8g2.setCursor(10, 50);
		u8g2.print(F("exists"));
    	u8g2.sendBuffer();
    	delay(2000);
    	return;

    }
  }



// Check if the first byte of the tag address is 0
  for (int i = 0; i < EEPROM.length(); i += mfrc522.uid.size) {
    bool emptyFound = true;
    for (int j = 0; j < mfrc522.uid.size; j++) {
      if (EEPROM.read(i + j) != 0) {
        emptyFound = false;
        break;
      }
    }
    if (emptyFound) {
      tagAddress = i;
      break;
    }
  }

  // Write tag data to EEPROM
  for (int i = 0; i < mfrc522.uid.size; i++) {
    EEPROM.write(tagAddress + i, tagData[i]);
  }
	  u8g2.clearDisplay();
	  u8g2.setFont(u8g2_font_7x14B_tf); 
	  u8g2.setCursor(4, 30);
	  u8g2.print(F("Success!"));
	  u8g2.setCursor(10, 50);
	  u8g2.print(F("Tag added"));
	  u8g2.sendBuffer(); 
	  delay(2000);

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

void removeTag(MFRC522 &mfrc522) {

  u8g2.clearDisplay();
  u8g2.setFont(u8g2_font_7x14B_tf);
  u8g2.drawStr(4, 30, "Please scan ID"); 
  u8g2.sendBuffer();

  byte tagData[MFRC522::MF_KEY_SIZE];
  unsigned long startTime = millis();
  while (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    if (millis() - startTime > 10000) {
      u8g2.clearDisplay();
      u8g2.setFont(u8g2_font_7x14B_tf);
      u8g2.setCursor(4, 30);
	  u8g2.print(F("No card"));
      u8g2.setCursor(10, 50);
	  u8g2.print(F("detected"));
      u8g2.sendBuffer();
      delay(2000);
      return;
    }
  }
  // Search for tag ID in EEPROM and delete it if found
  bool foundTag = false;
  for (int i = 0; i < EEPROM.length(); i++) {
    byte value = EEPROM.read(i);
    if (value == 0) {
      // The EEPROM address is empty, continue searching
      continue;
    }
    // Compare the tag ID byte-by-byte
    bool match = true;
    for (byte j = 0; j < mfrc522.uid.size; j++) {
      byte tagByte = mfrc522.uid.uidByte[j];
      byte eepromByte = EEPROM.read(i + j);
      if (tagByte != eepromByte) {
        match = false;
        break;
      }
    }
    if (match) {
      // The tag ID matches, delete it from EEPROM
      for (byte j = 0; j < mfrc522.uid.size; j++) {
        EEPROM.write(i + j, 0);
      }
      foundTag = true;
      break;
    }
  }

  // Check if the tag ID was found and deleted, and print appropriate message
  if (foundTag) {
    u8g2.clearDisplay();
    u8g2.setFont(u8g2_font_7x14B_tf);
	u8g2.setCursor(4, 30);
	u8g2.print(F("Success!"));
	u8g2.setCursor(10, 50);
	u8g2.print(F("Tag deleted"));
    u8g2.sendBuffer();
    delay(2000);
    
  } else {
    u8g2.clearDisplay();
    u8g2.setFont(u8g2_font_7x14B_tf);
    u8g2.setCursor(4, 30);
	u8g2.print(F("TAG NOT"));
	u8g2.setCursor(10, 50);
	u8g2.print(F("found"));
    u8g2.sendBuffer(); 
    delay(2000);
    
  }

  // Halt tag and stop encryption
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
 }
 




bool checkCardInEEPROM() {
  // Duy?t qua các ô nh? c?a EEPROM d? tìm ki?m ID c?a th?
  for (int i = 0; i < EEPROM.length() - 3; i += 4) {
    byte storedID[4];
        // Ð?c ID dã du?c luu tr? trong EEPROM
    for (int j = 0; j < 4; j++) {
      storedID[j] = EEPROM.read(i + j);
    }
    // So sánh ID c?a th? du?c quét v?i ID dã du?c luu tr? trong EEPROM
    if (memcmp(tagID1, storedID, 4) == 0) {
      // N?u trùng kh?p thì tr? v? true
      return true;
    }
  }
  // N?u không tìm th?y ID c?a th? trong EEPROM thì tr? v? false
  return false;
}
