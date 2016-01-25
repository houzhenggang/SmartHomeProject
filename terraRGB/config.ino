void readConfig() {
  Serial.print("[CONFIG] Mounting FileSystem");
  if (SPIFFS.begin()) {
    Serial.println(" --- DONE");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.print("[CONFIG] Opening config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println(" --- DONE");
        Serial.print("[CONFIG] Parsing Configuration");
        
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        //json.printTo(Serial);
        
        if (json.success()) {
          Serial.println(" --- DONE");
          //Serial.println("\nparsed json");

          mqtt_server = json["mqtt_server"];
          mqtt_port = json["mqtt_port"];
          mqtt_clientid = json["mqtt_clientid"];
          //Serial.println(mqtt_server);
          //Serial.println(mqtt_port);
          //Serial.println(mqtt_clientid);
        } else {
          Serial.println(" --- FAILED!!!");
        }
      }
    }
  } else {
    Serial.println(" --- FAILED!!!");
  }
}

void writeConfig() {
  Serial.print("[CONFIG] Saving config");
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["mqtt_server"] = mqtt_server;
  json["mqtt_port"] = mqtt_port;
  json["mqtt_clientid"] = mqtt_clientid;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println(" --- FAILED to open config file for writing");
  }

  json.printTo(Serial);
  json.printTo(configFile);
  configFile.close();
  //end save
}

