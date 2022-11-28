/**************************************************************************//**
 *
 * @file calculator.ino
 *
 * @author Samuel Ingledue, Tristan Curd
 *
 * @brief Implementation of a 4-function integer calculator
 *
 ******************************************************************************/

/*
 * Calculator GroupLab assignment and starter code (c) 2021-22 Christopher A. Bohn
 * Calculator GroupLab solution (c) the above-named student
 */

#include "CowPi.h"

#define DEBOUNCE_TIME 20u


// UNCOMMENT THE STRUCTURE FOR THE TIMER YOU WLL USE
// volatile cowpi_timer16bit_t *timer;
// volatile cowpi_timer8bit_t *timer;
volatile uint8_t *timer_interrupt_masks;

void setup() {
  cowpi_stdio_setup(9600);
  cowpi_set_display_i2c_address(0x27);
  cowpi_setup(LCD1602 | I2C);
  cowpi_lcd1602_set_backlight(true);
  // timer = ...;
  // timer->control = ...;
  // timer->compareA = ...;
  // timer_interrupt_masks = ...;
  // timer_interrupt_masks[...] |= ...;
}

void loop() {
  ;
}


void handle_buttonpress(void) {
  ;
}

void handle_keypress(void) {
  ;
}

