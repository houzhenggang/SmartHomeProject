void updateLED() {
  switch (LED_mode) {
    case DIMM:
      if(millis()-LED_timer > LED_speed) dimm();
      break;
    default:
    case INSTANT:
      edge();
      break;
    case FLASH:
      flash();
      break;
  }


}

void LEDoutput() {
  for (int i = 0; i < 3; i++) {
    analogWrite(LED_pin[i], LED_out[i]);
  }
}

void flash() {
  //Flashing Speed in Hertz
  int LED_toggle = (LED_speed * (millis() - LED_step) / 500) % 2;

  for (int i = 0; i < 3; i++)
    LED_out[i] = LED_val[i] * LED_toggle;

  LEDoutput();
}

void dimm() {
  for (int i = 0; i < 3; i++) {
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
  SetLED(LED_val[0], LED_val[1], LED_val[2]);
}

void SetLED(byte r, byte g, byte b) {
  LED_out[0] = r;
  LED_out[1] = g;
  LED_out[2] = b;
  LEDoutput();
}

