#ifndef BITMAP_LIBRARY_H
#define BITMAP_LIBRARY_H

#include <avr/pgmspace.h> // Required for PROGMEM

extern const unsigned char epd_bitmap_lock [];
extern const unsigned char epd_bitmap_setting [];
extern const unsigned char epd_bitmap_qrcode [];
extern const unsigned char epd_bitmap_pass [];
extern const unsigned char epd_bitmap_add [];
extern const unsigned char epd_bitmap_delete [];
extern const unsigned char epd_bitmap_SE[];
extern const unsigned char* bitmap_icons[3];


extern const unsigned char* bitmap_icons1[3];

extern const unsigned char bitmap_item_sel_outline [];

#endif /* BITMAP_LIBRARY_H */