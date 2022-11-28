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
  attachInterrupt(digitalPinToInterrupt(), handle_buttonpress(), mode);
  attachInterrupt(digitalPinToInterrupt(ioports[D0_D7].output), handle_keypress(), mode);
  //attachInterrupt(digitalPinToInterrupt(ioports[D0_D7].output), handle_keypress(), mode);
  //don't know what mode should be or the pin_number
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
  //if(leftbuttonpressed)
  cowpi_illuminate_left_led();
  //cowpi_deluminate_right_led();
  //if(rightbuttonpressed)
  cowpi_illuminate_right_led();
  //cowpi_deluminate_right_led();
}

void handle_keypress(void) {
  uint8_t key_pressed = 0xFF;
  unsigned long now = millis();
  if (now - last_keypress > DEBOUNCE_TIME) {
    last_keypress = now;

    for (int8_t i = 0; i < 4; i++){
      ioports[D0_D7].output |= 0xF0;
      ioports[D0_D7].output &= ~(1 << (i+4));
      delayMicroseconds(1);
      for (int8_t j = 0; j < 4; j++){
        if(!(ioports[D14_D19].input & (1 << j)))
        {
          key_pressed = keys[i][j];
        }

      }
    }    
  }
  ioports[D0_D7].output &= 0x0F;
  cowpi_lcd1602_place_character(0x0, key_pressed+'0');
}

