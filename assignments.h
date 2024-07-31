// Buttons
#define FEAT_DOWN 21
#define LEFT 20
#define FLIP 19
#define RIGHT 18
#define FEAT_UP 17
#define CHANGE_DISPLAY_TYPE 16
#define IN_START 15

// FPGA
#define FPGA_OUT_1 2
#define FPGA_OUT_2 3
#define FPGA_BUSY 4
#define FPGA_INIT 5


// DATA PINS
#define FEAT_0_BIT_0 52
#define FEAT_0_BIT_1 50
#define FEAT_0_BIT_2 48
#define FEAT_0_BIT_3 46
#define FEAT_0_BIT_4 44
#define FEAT_0_BIT_5 42
#define FEAT_0_BIT_6 40

#define FEAT_1_BIT_0 36
#define FEAT_1_BIT_1 34
#define FEAT_1_BIT_2 32
#define FEAT_1_BIT_3 30
#define FEAT_1_BIT_4 28
#define FEAT_1_BIT_5 26
#define FEAT_1_BIT_6 24

#define FEAT_2_BIT_0 53
#define FEAT_2_BIT_1 51
#define FEAT_2_BIT_2 49
#define FEAT_2_BIT_3 47
#define FEAT_2_BIT_4 45
#define FEAT_2_BIT_5 43
#define FEAT_2_BIT_6 41

#define FEAT_3_BIT_0 37
#define FEAT_3_BIT_1 35
#define FEAT_3_BIT_2 33
#define FEAT_3_BIT_3 31
#define FEAT_3_BIT_4 29
#define FEAT_3_BIT_5 27
#define FEAT_3_BIT_6 25
#define FEAT_3_BIT_7 23

uint8_t feat_0[] = {
  FEAT_0_BIT_0,
  FEAT_0_BIT_1,
  FEAT_0_BIT_2,
  FEAT_0_BIT_3,
  FEAT_0_BIT_4,
  FEAT_0_BIT_5,
  FEAT_0_BIT_6,
};

uint8_t feat_1[] = {
  FEAT_1_BIT_0,
  FEAT_1_BIT_1,
  FEAT_1_BIT_2,
  FEAT_1_BIT_3,
  FEAT_1_BIT_4,
  FEAT_1_BIT_5,
  FEAT_1_BIT_6,
};

uint8_t feat_2[] = {
  FEAT_2_BIT_0,
  FEAT_2_BIT_1,
  FEAT_2_BIT_2,
  FEAT_2_BIT_3,
  FEAT_2_BIT_4,
  FEAT_2_BIT_5,
  FEAT_2_BIT_6,
};

uint8_t feat_3[] = {
  FEAT_3_BIT_0,
  FEAT_3_BIT_1,
  FEAT_3_BIT_2,
  FEAT_3_BIT_3,
  FEAT_3_BIT_4,
  FEAT_3_BIT_5,
  FEAT_3_BIT_6,
};


uint8_t *feats[4] = {
  feat_0,
  feat_1,
  feat_2,
  feat_3
};