#include <LiquidCrystal.h>

// Auxiliary functions
void print_bits(uint8_t x);
void print_bits_lcd(uint8_t x);
void print_bits_serial(uint8_t x);
float fixed2float(uint8_t fixed, uint8_t decimal);
int negate(int value);


// Serial funcionts
void update_serial();
