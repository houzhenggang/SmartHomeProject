void Buttons() {
  for (byte i = 0; i < sizeof(switch_child_pins); i++)
  {
    if (switch_mode_pins[i] == SWITCH_MODE_MULTI) {
      MultiButton(i);
    }
    else {

      debouncer[i].update();
      byte value = debouncer[i].read();
      if (value != switch_oldValue[i])
      {
        PrintDebug("[Button] Toggled", INFO);
        PrintDebug(String(i), ADD);
        String valueStr(0);
        myMqtt.publish(mqtt_Buttons[i], valueStr);
        //myMqtt.publish(mqtt_Buttons[i], value == HIGH ? "1" : "0");
      }
      switch_oldValue[i] = value;
    }
  }
}

void MultiButton(int id) {
  debouncer[id].update();
  byte value = debouncer[id].read();
  if (value != switch_oldValue[id])
  {
    if (value == LOW) {
      //Button down, start Timer
      PrintDebug("[MultiButton] HIT!", DEBUG);
      switch_downtimer[id] = millis();
      switch_endtimer[id] = millis() + SWITCH_HOLDTIME;
      switch_oldValue[id] = LOW;
      switch_multitab[id] ++;
    }
    else {
      //Button RELEASED
      PrintDebug("[MultiButton] released", DEBUG);
      switch_oldValue[id] = HIGH;
      if (switch_multitab[id] > 0) {
        switch_endtimer[id] = millis() + SWITCH_HOLDTIME / 2;
      }
    }
  }
  else {
    if (switch_endtimer[id] < millis() && value == LOW && switch_multitab[id] == 1) {
      PrintDebug("[MultiButton] Holding Button", DEBUG);
      PrintDebug(String(id), ADD);
      String valueStr(-1);
      myMqtt.publish(mqtt_Buttons[id], valueStr);
      switch_downtimer[id] = 0;
      switch_endtimer[id] = 0;
      switch_multitab[id] = 0;
    }
    else if (switch_endtimer[id] < millis() && switch_multitab[id] >= 1) {
      PrintDebug("[MultiButton] Btn" + String(id) + " " + String(switch_multitab[id]) + String(" TAPS"), DEBUG);

      String valueStr(switch_multitab[id]);
      myMqtt.publish(mqtt_Buttons[id], valueStr);
      switch_downtimer[id] = 0;
      switch_endtimer[id] = 0;
      switch_multitab[id] = 0;
    }
  }
}
