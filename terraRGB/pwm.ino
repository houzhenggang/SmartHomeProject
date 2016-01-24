void LEDoutput() {
  for (int i = 0; i < sizeof(LED_pin); i++) {
    analogWrite(LED_pin[i], LED_out[i]);
  }
}

void dimm() {
  for (int i = 0; i < sizeof(LED_pin); i++) {
    if (LED_val[i] > LED_out[i])
      LED_out[i] += 1;
    else if (LED_val[i] < LED_out[i])
      LED_out[i] -= 1;
    else
      LED_out[i] = LED_val[i];
  }
  LEDoutput();
  LED_timer = millis();
}

void edge() {
  for (int i = 0; i < sizeof(LED_pin); i++) {
    LED_out[i] = LED_val[i];
  }
}
