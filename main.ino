#include "chars.h"
#include "assignments.h"
#include "display.h"
#include "fpga.h"

// Fixed point representation
#define DECIMAL_POINT 3
#define DECIMAL_INC (1.0 / pow(2, DECIMAL_POINT))

// Standard config
#define N_FEATURES 4
#define N_BITS 7
#define BIGGEST (pow(2, N_BITS - 1) - 1)
#define LAST_FEATURE (N_FEATURES - 1)

// ---------- GLOBAL VARIABLES ----------

// Button debounce
unsigned long last_debounce_time = 0;
unsigned long debounce_delay = 250;

// IO
uint8_t cursor = N_BITS - 2;
uint8_t values[N_FEATURES] = { 0 };
uint8_t id_value = 0;
uint8_t mask = (1 << cursor);


// State machine
enum state {
  SET_FEATURES_BIN,
  SET_FEATURES_DEC,
  CHECK_FEATURES,
  PROCESSING,
  FINISH,
  RESET,
  QUANTITY
};
state handle_state = SET_FEATURES_BIN;

// FPGA
int result_class = 0;
int received_input = 0;
bool defined_class = true;

// ---------- INTERRUPTS ----------

// Left and right
void int_left() {
  if ((millis() - last_debounce_time) > debounce_delay) {

    last_debounce_time = millis();
    switch (handle_state) {
      case SET_FEATURES_BIN:
        {
          cursor++;
          cursor %= (N_BITS - 1);
          mask = (1 << cursor);
        }
        break;

      case SET_FEATURES_DEC:
        {
          if (values[id_value] == 0) {
            values[id_value] = BIGGEST;
          } else {
            values[id_value]--;
          }
        }
        break;
    }
    update_serial();
  }
}

void int_right() {
  if ((millis() - last_debounce_time) > debounce_delay) {

    last_debounce_time = millis();
    switch (handle_state) {
      case SET_FEATURES_BIN:
        {
          if (cursor == 0) {
            cursor = N_BITS - 2;
            break;
          }
          cursor--;
          cursor %= N_BITS;
          mask = (1 << cursor);
        }
        break;
      case SET_FEATURES_DEC:
        {
          if (++values[id_value] > BIGGEST) {
            values[id_value] = 0;
          }
        }
        break;
    }
    update_serial();
  }
}

// Feature left and right
void int_feat_down() {
  if ((millis() - last_debounce_time) > debounce_delay) {
    last_debounce_time = millis();

    switch (handle_state) {
      case SET_FEATURES_BIN:
      case SET_FEATURES_DEC:
        {

          if (id_value == 0) {
            id_value = LAST_FEATURE;
            break;
          };

          id_value--;
          id_value %= N_FEATURES;
          cursor = N_BITS - 2;
        }
    }
    update_serial();
  }
}

void int_feat_up() {
  if ((millis() - last_debounce_time) > debounce_delay) {
    last_debounce_time = millis();

    switch (handle_state) {
      case SET_FEATURES_BIN:
      case SET_FEATURES_DEC:
        {
          id_value++;
          id_value %= N_FEATURES;
        }
    }
    update_serial();
  }
}

// FPGA interrupts
void int_fpga_busy() {
  if ((millis() - last_debounce_time) > debounce_delay) {
    last_debounce_time = millis();
    switch (handle_state) {
      case (PROCESSING):
        {

          handle_state = FINISH;

          // Read FGPA result
          unsigned short class_1 = digitalRead(FPGA_OUT_1);
          unsigned short class_2 = digitalRead(FPGA_OUT_2);
          result_class = 0;
          result_class |= class_1;
          result_class |= (class_2 << 1);
          received_input = class_1 + (class_2 << 1);

          defined_class = (result_class == 0) | (result_class == 1) | (result_class == 2);
        }
    }
    update_serial();
  }
}

// Other interrupts
void int_start() {
  if ((millis() - last_debounce_time) > debounce_delay) {
    last_debounce_time = millis();
    switch (handle_state) {

      case SET_FEATURES_BIN:
      case SET_FEATURES_DEC:
        {
          handle_state = CHECK_FEATURES;
        }
        break;

      case CHECK_FEATURES:
        {
          handle_state = PROCESSING;
          start_fpga();
        }
        break;

      case FINISH:
        {
          handle_state = SET_FEATURES_BIN;
        }
        break;
    }
    update_serial();
  }
};

void int_flip() {
  if ((millis() - last_debounce_time) > debounce_delay) {
    last_debounce_time = millis();
    switch (handle_state) {
      case SET_FEATURES_BIN:
        {
          values[id_value] ^= mask;
        }
        break;

      case SET_FEATURES_DEC:
        {
          for (int idx = 0; idx < N_FEATURES; idx++) {
            values[idx] = 0;
          }
        }
        break;

      case CHECK_FEATURES:
        {
          handle_state = SET_FEATURES_BIN;
        }
    }
    update_serial();
  }
}

void int_change_display_type() {
  if ((millis() - last_debounce_time) > debounce_delay) {
    last_debounce_time = millis();
    switch (handle_state) {
      case SET_FEATURES_BIN:
        {
          handle_state = SET_FEATURES_DEC;
        }
        break;

      case SET_FEATURES_DEC:
        {
          handle_state = SET_FEATURES_BIN;
        }
    }
    update_serial();
  }
}

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
  pinMode(FPGA_OUT_1, INPUT);
  pinMode(FPGA_OUT_2, INPUT);
  pinMode(FPGA_BUSY, INPUT_PULLUP);
  pinMode(FPGA_INIT, OUTPUT);
  digitalWrite(FPGA_INIT, HIGH);
  attachInterrupt(FPGA_BUSY, int_fpga_busy, FALLING);


  for (int j = 0; j < N_BITS; j++) {
    pinMode(feats[0][j], OUTPUT);
  }
  for (int j = 0; j < N_BITS; j++) {
    pinMode(feats[1][j], OUTPUT);
  }
  for (int j = 0; j < N_BITS; j++) {
    pinMode(feats[2][j], OUTPUT);
  }
  for (int j = 0; j < N_BITS; j++) {
    pinMode(feats[3][j], OUTPUT);
  }
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  update_serial();
}

void loop() {
  if (start_processing) {
    digitalWrite(FPGA_INIT, LOW);
    delay(100);
    start_processing = false;
    digitalWrite(FPGA_INIT, HIGH);
  }
  delay(500);
}
