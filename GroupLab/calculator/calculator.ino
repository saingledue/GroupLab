/**************************************************************************/ /**
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
volatile cowpi_timer16bit_t *timer;
// volatile cowpi_timer8bit_t *timer;
volatile uint8_t *timer_interrupt_masks;


//Added to use ioports and other polling_lab stuff
volatile cowpi_ioport_t *ioports;
volatile cowpi_i2c_t *i2c;

volatile int timer_time = 0;
bool is_timedout = 0;

//will be in top row
volatile int32_t operand_one = 0;
//will be in bottom row
volatile int32_t operand_two = NULL;
static char top_row[17] = "                 ";
static char bot_row[17] = "                 ";
bool is_error = 0;
static char operand = ' ';

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

  //trying timer 1
  timer = (cowpi_timer16bit_t *)(cowpi_io_base + 0x60);
  //set waveform selection mode and clock select bits
  timer->control = 0x0304;
  timer->compareA = 0x7c;
  timer_interrupt_masks = cowpi_io_base + 0x4E;
  timer_interrupt_masks[1] |= 0x2;
}

void build_display() {
  update_top_row();
  //printf("updating display with %s \n", num_as_string);
  cowpi_lcd1602_return_home();
  for (int i = 0; i < 17; i++) {
    cowpi_lcd1602_send_character(top_row[i]);
  }
  cowpi_lcd1602_place_cursor(0x40);
  for (int i = 0; i < 17; i++) {
    cowpi_lcd1602_send_character(bot_row[i]);
  }
}


void loop() {

  //30 seconds if left, 7.5 seconds if right
  int timeout_limit = cowpi_left_switch_in_left_position() ? 80 : 30;

  if (timer_time > timeout_limit) {
    is_timedout = true;
    //printf("Timed out! \n");
    cowpi_lcd1602_set_backlight(false);
  } else {
    is_timedout = false;
    //printf("timer not timed out yet \n");
    cowpi_lcd1602_set_backlight(true);
  }
  build_display();
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
  //button pressed
  timer_time = 0;

  static unsigned long last_buttonpress = 0uL;

  //static unsigned long last_buttonpress = 0uL;
  unsigned long now = millis();
  if (now - last_buttonpress > DEBOUNCE_TIME) {
    last_buttonpress = now;

    if (is_timedout) {
      return;
    }

    if (cowpi_left_button_is_pressed()) {
      //negate number
    }
    if (cowpi_right_button_is_pressed()) {
      //clear op
      operand = ' ';
      bot_row[0] = ' ';

      //clear value
      //if op2 is being built, op2 is reset to no defined value, next operation is cleared, and op 1 is displayed
      if (operand_two != NULL) {
        //turn back to null
        operand_two = NULL;
        //bottom row cleared
        for (int i = 1; i < 17; i++) {
          bot_row[i] = ' ';
        }
      } else{
        //else op1 resets to 0 and next op cleared
        operand_one = 0;
      }

    }
  }


  //cowpi_deluminate_right_led();

  //cowpi_deluminate_right_led();
}


/**
*@brief Handles whenever the keypad is pressed, need to get the right keypress
*/
void handle_keypress() {
  static unsigned long last_keypress = 0uL;
  //button pressed
  timer_time = 0;

  uint8_t key_pressed = 0xFF;
  //static unsigned long last_keypress = 0uL;
  unsigned long now = millis();
  if (now - last_keypress > DEBOUNCE_TIME) {
    last_keypress = now;
    //waking up the display doesn't count
    if (is_timedout) {
      return;
    }
    for (int8_t i = 0; i < 4; i++) {
      ioports[D0_D7].output |= 0xF0;
      ioports[D0_D7].output &= ~(1 << (i + 4));
      delayMicroseconds(1);
      for (int8_t j = 0; j < 4; j++) {
        if (!(ioports[D14_D19].input & (1 << j))) {
          key_pressed = keys[i][j];
        }
      }
    }
  }

  printf("key pressed is: %d \n", key_pressed);
  ioports[D0_D7].output &= 0x0F;
  if (key_pressed < 255) {
    //actual check to see what to do
    char actual_char = ' ';
    if (key_pressed < 0xA) {
      //update value
      if (operand_two == NULL) {
        operand_two = key_pressed;
      } else {
        operand_two *= 10;
        if (operand_two < 0) {
          operand_two -= key_pressed;
        } else {
          operand_two += key_pressed;
        }
      }
      actual_char = key_pressed + '0';

      int string_index = 1;
      //right justified
      //get to where the numbers are
      while (((bot_row[string_index] == ' ')) && string_index < 17) {
        string_index++;
      }
      //move all numbers to the left 1
      while (string_index < 17) {
        bot_row[string_index - 1] = bot_row[string_index];
        string_index++;
      }
      string_index--;
      string_index--;
      bot_row[string_index] = actual_char;

    } else if (key_pressed <= 0xE) {
      //steps
      /**
      2. if operand_1 = 0, op1 = op2
      3. otherwise, if there was an op already there & a number in op2, do op1 _= op2
      4. if there's an op and no number, then op just changes

      */
      actual_char = key_pressed + 55;
      if (operand_one == 0) {
        operand_one = operand_two;
        operand_two = NULL;
      } else {
        //there's a val in op1
        if (operand != ' ') {
          //there's already an operand
          if (operand_two != NULL) {
            do_operand();
          }
        }
      }
      //update operand at end no matter what

      //find operand
      switch (actual_char) {
        case 'A':
          operand = '+';
          break;
        case 'B':
          operand = '-';
          break;
        case 'C':
          operand = 'x';
          break;
        case 'D':
          operand = 0xFD;
          break;
        case 'E':
          operand = ' ';
          break;
        default:
          operand = ' ';
          break;
      }

      //place operand
      bot_row[0] = operand;
      //no matter the operation, we'll clear the bottom row after each press
      for (int i = 1; i < 17; i++) {
        bot_row[i] = ' ';
      }
    }
    //issue I'm having is the key pressed is turning to 255 immediately after the button was pressed
    printf("operand 2 is %ld \n", operand_two);
  }
}

void do_operand() {
  printf("Operand before is %ld \n", operand_one);
  switch (operand) {
    case '+':
      operand_one += operand_two;
      break;
    case '-':
      operand_one -= operand_two;
      break;
    case 'x':
      operand_one *= operand_two;
      break;
    case 0xFD:
      if (operand_two != 0) {
        operand_one /= operand_two;
      } else {
        //oops division by 0
        printf("no can do boss \n");
      }
    default:
      break;
  }
  printf("Operand before is %ld \n", operand_one);
  //no matter what, the bottom is cleared
  operand_two = NULL;
}

void update_top_row() {
  //top row has the operand_one value, so we turn that to a string and add it to the rightmost side of the top row
  for (int i = 0; i < 17; i++) {
    top_row[i] = ' ';
  }


  char operand_one_string[17];
  sprintf(operand_one_string, "%ld", operand_one);

  //add to rightmost side
  int num_length = strlen(operand_one_string);
  int starting_index = 16 - num_length;
  for (int i = 0; i < num_length; i++) {
    top_row[i + starting_index] = operand_one_string[i];
  }
}



ISR(TIMER1_COMPA_vect) {
  timer_time++;
}
