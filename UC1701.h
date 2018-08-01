#ifndef UC1701_H
#define UC1701_H

#include <stdint.h>

void uc1701_init(int ss, int cd, int reset);

void uc1701_reset(); // Called by init automatically.

void uc1701_set_scroll(uint8_t line);
void uc1701_add_scroll(int8_t delta);
void uc1701_set_position(uint8_t x, uint8_t y); // Y is in count of 8 pixels. Scroll should be set before calling this function.
void uc1701_draw_bitmap(const void *data, uint8_t n); // Draw n x 8 pixels.
void uc1701_draw_progmem_bitmap(const void *data, uint8_t n); // Draw n x 8 pixels from PROGMEM memory.

#endif // UC1701_H
