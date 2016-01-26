String mqtt_mode = getPath("mode");
String mqtt_speed = getPath("speed");
String mqtt_R = getPath("R");
String mqtt_G = getPath("G");
String mqtt_B = getPath("B");
String mqtt_L = getPath("TobyLamp");

void myConnectedCb() {
  PrintDebug("--- CONNECTED", ADD);
  PrintDebug("[MQTT] Start subscribing:", INFO);

  PrintDebug("->", INFO);
  PrintDebug(mqtt_mode, ADD);
  myMqtt.subscribe(mqtt_mode);

  PrintDebug("->", INFO);
  PrintDebug(mqtt_speed, ADD);
  myMqtt.subscribe(mqtt_speed);

  //Subscribe LED Channels
  for (int i = 0; i < sizeof(LED_pin); i++) {
    PrintDebug("->", INFO);
    PrintDebug(mqtt_LED[i], ADD);
    myMqtt.subscribe(mqtt_LED[i]);
  }


  bIsConnected = true;
}

// Called when Disconnected from MQTT Server
void myDisconnectedCb() {
  PrintDebug("[MQTT] Disconnected!", ERR);
  bIsConnected = false;
}

void myPublishedCb() {
}

// Called when Data is received from MQTT Server
void myDataCb(String& topic, String& data) {
  PrintDebug("[MQTT]", INFO);
  PrintDebug(topic, ADD);
  PrintDebug(":", ADD);
  PrintDebug(data, ADD);

  //Wandle Datenwert in Integer um. openhab Dimmerwerte liegen zwischen 0-100
  int val = data.toInt();

  if (topic == mqtt_mode)
    LED_mode = val;
  if (topic == mqtt_speed)
    LED_speed = val;
  else {
    if (LEDreadMQTT(topic, val))
      PrintDebug("-> LED matched topic", SUCC);
    else
      PrintDebug("Incoming Data not recognized!", ERR);
  }
}

String getPath(String actor) {
  String path = "";
  path += mqtt_clientid;
  path += "/" + actor;
  return path;
}

