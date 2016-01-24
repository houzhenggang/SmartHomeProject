/****************************************
 *
 *
 *        terraRGB
 *          the standard Sketch for
 *
 *
 *
 *
 */
#include <FS.h>                   //this needs to be first, or it all crashes and burns... - Not Shure why it's needed
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <MQTT.h>
//for FileSystem
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#include <Bounce2.h>

#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson

#define CLIENT_ID "Bett"
#define DIMM 0
#define INSTANT 1
#define FLASH 2
#define RAINBOW 3
#define SAWTOOTH 4
#define PULSE 5



// Standard values for the MQTT config mode
const char *mqtt_server = "test.mosquitto.net";
const char *mqtt_port = "1883";
const char *mqtt_clientid = "ESPtest";

// create MQTT object
//MQTT myMqtt(mqtt_server, mqtt_server, 1883);
MQTT myMqtt(CLIENT_ID, "192.168.4.4", 1883);


//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

boolean bIsConnected = false;

byte LED_mode = DIMM;
int LED_speed = 0;
int LED_pin[] = {12, 14, 16, 5};
int LED_step = 0;
double LED_timer;
byte LED_out[] = {0, 0, 0, 0};
byte LED_val[] = {0, 0, 0, 0};



// #### Switch / Button ####
#define SWITCH_MODE_NORM  0
#define SWITCH_MODE_MULTI 1
#define SWITCH_MODE_PIR 2

#define SWITCH_HOLDTIME 1000

String mqtt_Buttons[0] = getPath("Button0");
const byte switch_child_pins[] = {5};
const byte switch_mode_pins[] = {SWITCH_MODE_MULTI};
Bounce debouncer[ sizeof(switch_child_pins) ]; 
boolean switch_oldValue[ sizeof(switch_child_pins) ];

// #### MultiButton Variables #### 
double switch_downtimer[ sizeof(switch_child_pins) ];
double switch_endtimer[ sizeof(switch_child_pins) ];
byte switch_multitab[ sizeof(switch_child_pins) ];

//Button on this pin will reset the Wifi settings if hold when turned on
int BTN_pin = (int) switch_child_pins[0];

void setup() {
  pinMode(BTN_pin, INPUT_PULLUP);
  // Configure the RGB PWM Output
  for (int i = 0; i < 3; i++) {
    pinMode(LED_pin[i], OUTPUT);
  }
  analogWriteRange(100);

  Serial.begin(115200);
  delay(1000);

  Serial.println("Booting ESP8266");
  Serial.print("ChipID: ");
  Serial.println(ESP.getChipId());
  Serial.print("FlashChipID: ");
  Serial.println(ESP.getFlashChipId());
  Serial.print("Flash Speed: ");
  Serial.println(ESP.getFlashChipSpeed());
  Serial.print("Flash Size: ");
  Serial.println(ESP.getFlashChipSize());

  Serial.println("mounting FS...");

  readConfig();
  //myMqtt = MQTT(mqtt_server, mqtt_server, 1883);
  //end read

  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_mqtt_server("mqtt_server", "MQTT Server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("mqtt_port", "MQTT Port", mqtt_port, 8);
  WiFiManagerParameter custom_mqtt_clientid("mqtt_clientid", "Client ID", mqtt_clientid, 64);

  WiFiManager wifiManager;

  // If BTN is held down when turned on, Reset Wifi Settings
  if (digitalRead(BTN_pin) == LOW) {
    Serial.println("BTN remained pressed... resetting Wifi");
    wifiManager.resetSettings();
  }

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  // id/name, placeholder/prompt, default, length
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_clientid);

  if (! wifiManager.autoConnect("terraRGB")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  /*
  while (WiFi.status() != WL_CONNECTED) {
    delay(10);
    LED_step += 5;
    if (LED_step >= 255) {
      Serial.print(".");
      LED_step = 0;
    }
    SetLED(LED_step, LED_step, LED_step);
  }*/

  //read updated parameters
  mqtt_server = custom_mqtt_server.getValue();
  mqtt_port = custom_mqtt_port.getValue();
  mqtt_clientid = custom_mqtt_clientid .getValue();

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    writeConfig();
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Connecting to MQTT server");

  // setup MQTT Callback Functions
  myMqtt.onConnected(myConnectedCb);
  myMqtt.onDisconnected(myDisconnectedCb);
  myMqtt.onPublished(myPublishedCb);
  myMqtt.onData(myDataCb);

  delay(10);
}

//
void loop() {

  if (bIsConnected) {
    updateLED();
    Buttons();
  } else {
    // try to connect to mqtt server
    myMqtt.connect();
    delay(50); //wait for bIsConnected to be set correctly!
    if (bIsConnected) {
      // Successfully connected to mosquitto Server
      Serial.println("Successfull");
      SetLED(0, 255, 0);
      delay(200);
      SetLED(255, 255, 0);
      delay(200);
      SetLED(255, 0, 255);
      delay(200);
      SetLED(0, 0, 0);
    }
    else {
      // Connection failed
      Serial.println("Connection to MQTT failed");
      SetLED(255, 0, 0);
      delay(200);
      SetLED(0, 0, 0);
      delay(200);
      SetLED(255, 0, 0);
      delay(500);
      SetLED(0, 0, 0);
    }
  }

  delay(1);
}
