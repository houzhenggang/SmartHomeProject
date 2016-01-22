String mqtt_mode = getPath("mode");
String mqtt_speed = getPath("speed");
String mqtt_R = getPath("R");
String mqtt_G = getPath("G");
String mqtt_B = getPath("B");

void myConnectedCb() {
  Serial.println("connected to MQTT server");
  Serial.println("Start subscribung:");
  Serial.println(mqtt_mode);
  myMqtt.subscribe(mqtt_mode);
  myMqtt.subscribe(mqtt_speed);
  myMqtt.subscribe(mqtt_R);
  myMqtt.subscribe(mqtt_G);
  myMqtt.subscribe(mqtt_B);
  bIsConnected = true;
}

void myDisconnectedCb() {
  Serial.println("disconnected. try to reconnect...");
  bIsConnected = false;
}

void myPublishedCb() {
}

void myDataCb(String& topic, String& data) {
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(data);

  //Wandle Datenwert in Integer um. openhab Dimmerwerte liegen zwischen 0-100
  int val = data.toInt();


  if (topic == mqtt_mode)
    LED_mode = val;
  if (topic == mqtt_speed)
    LED_speed = val;
  else {
    //Wandel Prozentangabe in Binäre PWM Wert um:
    //val *= 2.55;
    Serial.println(val);

    if (topic == mqtt_R)
      LED_val[0] = val;
    else if (topic == mqtt_G)
      LED_val[1] = val;
    else if (topic == mqtt_B)
      LED_val[2] = val;
    else
      Serial.println("ERROR: Data nicht erkannt!");
  }


}

String getPath(String actor) {
  String path = "/";
  path += CLIENT_ID;
  path += "/" + actor;
  return path;
}

