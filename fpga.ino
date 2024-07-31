void start_fpga() {
  for (int idx = 0; idx < N_FEATURES; idx++) {
    assign_fpga_pins(feats[idx], values[idx]);
  }
  start_processing = true;
}

void assign_fpga_pins(uint8_t *feat, int value) {
  Serial.println(value);
  for (int idx = 0; idx < N_BITS; idx++) {
    int resultado = value & (1 << idx) ? 1 : 0;
    digitalWrite(feat[idx], resultado);
  }
}
