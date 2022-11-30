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

#define DEBOUNCE_TIME 200u


// UNCOMMENT THE STRUCTURE FOR THE TIMER YOU WLL USE
// volatile cowpi_timer16bit_t *timer;
// volatile cowpi_timer8bit_t *timer;
volatile uint8_t *timer_interrupt_masks;


//Added to use ioports and other polling_lab stuff
volatile cowpi_ioport_t *ioports;
volatile cowpi_i2c_t *i2c;  
volatile int operand_one = 0;
volatile int operand_two = 0;

const uint8_t keys[4][4] = {
  { 0x1, 0x2, 0x3, 0xA },
  { 0x4, 0x5, 0x6, 0xB },
  { 0x7, 0x8, 0x9, 0xC },
  { 0xF, 0x0, 0xE, 0xD }
};

void setup() {
  cowpi_stdio_setup(9600);
  cowpi_set_display_i2c_address(0x27);
  cowpi_setup(LCD1602 | I2C);
  cowpi_lcd1602_set_backlight(true);
  initialize_io();
  //button is at D2
  attachInterrupt(digitalPinToInterrupt(2), handle_buttonpress, CHANGE);
  //keypress is at D3
  attachInterrupt(digitalPinToInterrupt(3), handle_keypress, CHANGE);

  // timer = ...;
  // timer->control = ...;
  // timer->compareA = ...;
  // timer_interrupt_masks = ...;
  // timer_interrupt_masks[...] |= ...;
}

void loop() {
  ;
}

void initialize_io(void) {
  ioports = (cowpi_ioport_t *)(cowpi_io_base + 0x3);
  i2c = (cowpi_i2c_t *)(cowpi_io_base + 0x98);
  //cowpi_lcd1602_set_send_halfbyte_function(send_halfbyte);
}

/**
*@brief Handles whenever the button is pressed
*/
void handle_buttonpress() {

  static unsigned long last_buttonpress = 0uL;

  //static unsigned long last_buttonpress = 0uL;
  unsigned long now = millis();
  if (now - last_buttonpress > DEBOUNCE_TIME) {
  last_buttonpress = now;
  

  if(cowpi_left_button_is_pressed()){
      printf("left button is pressed! \n");
      cowpi_illuminate_left_led();    
  } else{
      printf("left button is not pressed! \n");
      cowpi_deluminate_left_led();
  }
  if(cowpi_right_button_is_pressed()){
      printf("right button is pressed! \n");
      cowpi_illuminate_right_led();
  } else{
      printf("right button is not pressed! \n");
      cowpi_deluminate_right_led();
  }
  printf("\n");
  }
  

  //cowpi_deluminate_right_led();

  //cowpi_deluminate_right_led();
}


/**
*@brief Handles whenever the keypad is pressed, need to get the right keypress
*/
void handle_keypress() {
  static unsigned long last_keypress = 0uL;
  printf("key is pressed! \n");
  uint8_t key_pressed = 0xFF;
  //static unsigned long last_keypress = 0uL;
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

  printf("key pressed is: %d \n", key_pressed);  
  ioports[D0_D7].output &= 0x0F;
  if(key_pressed < 255){
  char actual_char = ' ';
  cowpi_lcd1602_return_home();
  if(key_pressed < 0xA){
     actual_char = key_pressed + '0';
  } else if(key_pressed <= 0xF){
     actual_char = key_pressed + 55;
  }
  //issue I'm having is the key pressed is turning to 255 immediately after the button was pressed
  
  cowpi_lcd1602_send_character(actual_char);
  }
}

