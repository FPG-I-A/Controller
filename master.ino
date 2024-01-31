#include <LiquidCrystal.h>
#include "chars.h"

// Buttons
#define FEAT_DOWN 21
#define LEFT 20
#define FLIP 19
#define RIGHT 18
#define FEAT_UP 17
#define CHANGE_DISPLAY_TYPE 16
#define IN_START 15

// LCD
#define RS 7
#define EN 6
#define PIN4 5
#define PIN5 4
#define PIN6 3
#define PIN7 2

// FPGA
#define FPGA_BUSY 13

// Fixed point representation
#define DECIMAL_POINT 5
#define DECIMAL_INC (1.0 / pow(2, DECIMAL_POINT))

// Standard config
#define N_FEATURES 5
#define LAST_FEATURE (N_FEATURES - 1)
#define LAST_PAGE (N_FEATURES / 2)


// ---------- GLOBAL VARIABLES ----------

// Button debounce
unsigned long last_debounce_time = 0;
unsigned long debounce_delay = 250;

// IO
uint8_t cursor = 0;
uint8_t values[N_FEATURES] = {0};
uint8_t id_value = 0;
uint8_t mask = (1 << cursor);

// State machine
enum state {
  SET_FEATURES_BIN,
  SET_FEATURES_DEC,
  CHECK_FEATURES,
  PROCESSING,
  FINISH,
  QUANTITY
};
state handle_state = SET_FEATURES_BIN;

// LCD 
LiquidCrystal lcd(RS, EN, PIN4, PIN5, PIN6, PIN7);
unsigned int checking_page = 0;

// FPGA
int result_class = 0;


// ---------- INTERRUPTS ----------

// Left and right
void int_left() {
  if ((millis() - last_debounce_time) > debounce_delay){
    last_debounce_time = millis();
    switch(handle_state) {
      case SET_FEATURES_BIN: {
        cursor++;
        cursor %= 8;
        mask = (1 << cursor);
        update_lcd();
      } break;
      
      case SET_FEATURES_DEC: {
        values[id_value]--;
        update_lcd();
      } break;
    }
  }
}

void int_right() {
  if ((millis() - last_debounce_time) > debounce_delay){
    last_debounce_time = millis();
    switch (handle_state) {
      case SET_FEATURES_BIN: {
        cursor--;
        cursor %= 8;
        mask = (1 << cursor);
        update_lcd();
      } break;
      case SET_FEATURES_DEC: {
        values[id_value]++;
        update_lcd();
      } break;
    }
  }
}

// Feature left and right
void int_feat_down() {
  if ((millis() - last_debounce_time) > debounce_delay){
    last_debounce_time = millis();
    
    switch (handle_state) {
      case SET_FEATURES_BIN:
      case SET_FEATURES_DEC: {
        
        if (id_value == 0) {
          id_value = LAST_FEATURE;
          update_lcd();
          return;
        };

        id_value--;
        id_value %= N_FEATURES;
        update_lcd();
      }

      case CHECK_FEATURES: {
        checking_page--;
        if (checking_page * 2 - 1 > N_FEATURES && checking_page != 0) {
          checking_page = N_FEATURES / 2;
        }
        update_lcd();
      }
    }
  }
}

void int_feat_up() {
  if ((millis() - last_debounce_time) > debounce_delay){
    last_debounce_time = millis();

    switch (handle_state) {
      case SET_FEATURES_BIN:
      case SET_FEATURES_DEC: {
        id_value++;
        id_value %= N_FEATURES;
        update_lcd();
      }
      case CHECK_FEATURES: {
        checking_page++;
        if (checking_page * 2 + 1 > N_FEATURES) {
          checking_page = 0;
        }
        update_lcd();
      }
    }
  }
}

// FPGA interrupts
void int_fpga_busy() {
  if ((millis() - last_debounce_time) > debounce_delay){
    last_debounce_time = millis();
    switch (handle_state) {
      case (PROCESSING): {
        
        handle_state = FINISH;
        // Read FGPA result
        result_class = 1; // TBD

        // Display result
        update_lcd();
      }
    }
  }
}

// Other interrupts
void int_start() {
  if ((millis() - last_debounce_time) > debounce_delay){
    last_debounce_time = millis();
    switch (handle_state) {
      
      case SET_FEATURES_BIN:
      case SET_FEATURES_DEC: {
        handle_state = CHECK_FEATURES;
        checking_page = 0;
        update_lcd();
      } break;

      case CHECK_FEATURES: {
        handle_state = PROCESSING;
        start_fpga();
        update_lcd();
      } break;

      case FINISH: {
        handle_state = SET_FEATURES_BIN;
        update_lcd();
      } break;
    }
  }
};

void int_flip() {
  if ((millis() - last_debounce_time) > debounce_delay){
    last_debounce_time = millis();
    switch (handle_state) {
      case SET_FEATURES_BIN: {
        values[id_value] ^= mask;
        update_lcd();
      } break;

      case SET_FEATURES_DEC: {
        values[0] = 0;
        values[1] = 0;
        values[2] = 0;
        values[3] = 0;
        update_lcd();
      } break;

      case CHECK_FEATURES: {
        handle_state = SET_FEATURES_BIN;
        update_lcd();
      }
    }
  }
}

void int_change_display_type() {
  if(((millis() - last_debounce_time) > debounce_delay)) {
      last_debounce_time = millis();
    switch (handle_state) {
      case SET_FEATURES_BIN: {
        handle_state = SET_FEATURES_DEC;
        update_lcd();
      } break;

      case SET_FEATURES_DEC: {
        handle_state = SET_FEATURES_BIN;
        update_lcd();
      }
    }
  }
}


// ---------- AUXILARY FUNCTIONS ----------

// Type conversion
void print_bits(uint8_t x) {
  for (int idx = 0; idx < 8; idx++){
    int resultado = x & ( 1 << (7 - idx )) ? 1 : 0;
    Serial.print(resultado);
  }
}

void print_bits_lcd(uint8_t x) {
  for (int idx = 0; idx < 8; idx++){
    int resultado = x & ( 1 << (7 - idx)) ? 1 : 0;
    lcd.print(resultado);
  }
}

float fixed2float(uint8_t fixed, uint8_t decimal) {
  float result = 0.0;
  int counter = 0;
  
  // Parte inteira
  for (int id_bit = decimal; id_bit < 8; id_bit++) {
    bool add = fixed & ( 1 <<(id_bit)) ? 1 : 0;
    if (add) {
      result += pow(2, counter);
    }
    counter++;
  }

  // Parte fracionaria
  counter = 1;
  for (int id_bit = decimal - 1; id_bit > -1; id_bit--) {
    bool add = fixed & ( 1 << id_bit ) ? 1 : 0;
    if (add) {
      result += 1 / pow(2, counter);
    }
    counter++;
  }
  return result;
}

// Display functions
void init_lcd() {
  lcd.begin(16, 2);
  lcd.createChar(0, arrow_up);
  lcd.createChar(1, arrow_down);
  lcd.createChar(2, happy_face);

  lcd.home();
  lcd.write("FPG-I-A");
  lcd.setCursor(0, 1);
  lcd.write("AI on FGPA!");
  lcd.setCursor(15, 1);
  lcd.write(byte(2));

  delay(1000);
  update_lcd();

}

void update_lcd() {
  lcd.clear();
  lcd.home();
  switch (handle_state) {
    case (SET_FEATURES_BIN): {
      lcd.print("Feature: ");
      lcd.print(id_value);
      lcd.setCursor(0, 2);
      lcd.print("Value: ");
      print_bits_lcd(values[id_value]);

      // Display arrows
      if (id_value != 0) {
        lcd.setCursor(15, 0);
        lcd.write(byte(0));
      }

      if (id_value < N_FEATURES - 1) {
        lcd.setCursor(15, 1);
        lcd.write(byte(1));
      }

      lcd.setCursor((8 - cursor) + 6, 2);

    } break;

    case (SET_FEATURES_DEC): {
      lcd.print("Feature: ");
      lcd.print(id_value);
      lcd.setCursor(0, 2);
      lcd.print("Value: ");
      lcd.print(fixed2float(values[id_value], DECIMAL_POINT), DECIMAL_POINT);
      
      // Display arrows
      if (id_value != 0) {
        lcd.setCursor(15, 0);
        lcd.write(byte(0));
      }

      if (id_value < N_FEATURES - 1) {
        lcd.setCursor(15, 1);
        lcd.write(byte(1));
      }
    } break;

    case (CHECK_FEATURES) :{
      // display features
      for (int feat = checking_page * 2; feat < min(2 * (checking_page + 1), N_FEATURES); feat++) {
        lcd.print("Feat ");
        lcd.print(feat);
        lcd.print(": ");
        lcd.print(fixed2float(values[feat], DECIMAL_POINT), DECIMAL_POINT);
        lcd.setCursor(0, 1);
      }

      // Display arrows
      if (checking_page != 0) {
        lcd.setCursor(15, 0);
        lcd.write(byte(0));
      }

      if (checking_page * 2 + 1 < N_FEATURES) {
        lcd.setCursor(15, 1);
        lcd.write(byte(1));
      }
    } break;

    case (PROCESSING): {
      lcd.print("Waiting FPGA to");
      lcd.setCursor(0, 1);
      lcd.print("finish process..");
    } break;

    case (FINISH): {
      lcd.print("Process finished");
      lcd.setCursor(0, 1);
      lcd.print("Class: ");
      lcd.print(result_class);
    } break;
  }
}
  

// FGPA controlling functions
void start_fpga(){} //TBD


// ---------- ARDUINO NEEDED FUNCTIONS ----------

void setup() {
  // I/O buttons
  pinMode(FEAT_DOWN, INPUT_PULLUP);
  pinMode(LEFT, INPUT_PULLUP);
  pinMode(FLIP, INPUT_PULLUP);
  pinMode(RIGHT, INPUT_PULLUP);
  pinMode(FEAT_UP, INPUT_PULLUP);
  pinMode(CHANGE_DISPLAY_TYPE, INPUT_PULLUP);
  pinMode(IN_START, INPUT_PULLUP);

  attachInterrupt(FEAT_DOWN, int_feat_down, FALLING);
  attachInterrupt(LEFT, int_left, FALLING);
  attachInterrupt(FLIP, int_flip, FALLING);
  attachInterrupt(RIGHT, int_right, FALLING);
  attachInterrupt(FEAT_UP, int_feat_up, FALLING);
  attachInterrupt(CHANGE_DISPLAY_TYPE, int_change_display_type, FALLING);
  attachInterrupt(IN_START, int_start, FALLING);

  // FPGA I/O
  pinMode(FPGA_BUSY, INPUT_PULLUP);
  attachInterrupt(FPGA_BUSY, int_fpga_busy, FALLING);

  init_lcd();

}

void loop() {
  Serial.println(handle_state);
  switch (handle_state) {
    case (SET_FEATURES_BIN): {
      lcd.cursor();
      delay(250);
      lcd.noCursor();
      delay(250);
    } break;
  }
}
