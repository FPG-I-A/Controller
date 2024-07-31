// Auxiliary functions
void print_bits(uint8_t x) {
  for (int idx = 0; idx < N_BITS; idx++) {
    int resultado = x & (1 << (N_BITS - 1 - idx)) ? 1 : 0;
  }
}



void print_bits_serial(uint8_t x) {
  for (int idx = 0; idx < N_BITS; idx++) {
    int resultado = x & (1 << (N_BITS - 1 - idx)) ? 1 : 0;
    Serial.print(resultado);
  }
}


float fixed2float(uint8_t fixed, uint8_t decimal) {
  float result = 0.0;
  int counter = 0;

  // Parte inteira
  for (int id_bit = decimal; id_bit < N_BITS; id_bit++) {
    bool add = fixed & (1 << (id_bit)) ? 1 : 0;
    if (add) {
      result += pow(2, counter);
    }
    counter++;
  }

  // Parte fracionaria
  counter = 1;
  for (int id_bit = decimal - 1; id_bit > -1; id_bit--) {
    bool add = fixed & (1 << id_bit) ? 1 : 0;
    if (add) {
      result += 1 / pow(2, counter);
    }
    counter++;
  }
  return result;
}


// Serial funcionts
void update_serial() {
  Serial.write(27);
  Serial.print("[2J");
  Serial.write(27);
  Serial.print("[H");
  switch (handle_state) {
    case (SET_FEATURES_BIN):
      {
        Serial.print("Configurando valor ");
        Serial.print(id_value);
        Serial.println(" em modo binário");
        Serial.print("Valor: ");
        print_bits_serial(values[id_value]);
        Serial.println("");
        Serial.print("Bit:   ");
        for (int i = N_BITS - 1; i > cursor; i--) Serial.print(" ");
        Serial.println("‾");
      }
      break;

    case (SET_FEATURES_DEC):
      {
        Serial.print("Configurando valor ");
        Serial.print(id_value);
        Serial.println(" em modo decimal");
        Serial.print("Valor: ");
        Serial.print(fixed2float(values[id_value], DECIMAL_POINT), DECIMAL_POINT);
        Serial.print(" = ");
        Serial.println(values[id_value]);
      }
      break;

    case (CHECK_FEATURES):
      {
        Serial.println("Verificando valores");
        // Display features
        for (int feat = 0; feat < N_FEATURES; feat++) {
          Serial.print("Valor ");
          Serial.print(feat);
          Serial.print(": ");
          Serial.print(fixed2float(values[feat], DECIMAL_POINT), DECIMAL_POINT);
          Serial.print(" = ");
          Serial.println(values[feat]);
        }
      }
      break;

    case (PROCESSING):
      {
        Serial.println("Aguardando FPGA finalizar o processamento...");
      }
      break;

    case (FINISH):
      {
        if (defined_class) {
          Serial.println("Processo finalizado");
          Serial.print("Classe: ");
          Serial.println(result_class);
          Serial.println("Pressione iniciar para configurar novos valores de entrada");
        } else {
          Serial.println("Classe inválida");
          Serial.print("Received: ");
          unsigned short received = received_input;
          Serial.print(received & (1 << 0) ? 1 : 0);
          Serial.println(received & (1 << 1) ? 1 : 0);
          Serial.println("Pressione iniciar para configurar novos valores de entrada");
        }
      }
      break;
  }
}