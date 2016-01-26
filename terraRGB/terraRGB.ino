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

#define CLIENT_ID "BettESP"
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

WiFiManagerParameter custom_mqtt_server("mqtt_server", "MQTT Server", mqtt_server, 40);
WiFiManagerParameter custom_mqtt_port("mqtt_port", "MQTT Port", mqtt_port, 8);
WiFiManagerParameter custom_mqtt_clientid("mqtt_clientid", "Client ID", mqtt_clientid, 64);


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

//DEBUG values
#define ADD -1
#define INFO 10
#define DEBUG 20
#define SUCC 240
#define ERR 250
#define DEBUG_LEVEL 0

boolean debug_omitfollowing = false;

void PrintDebug(String text, int level = DEBUG) {

  if (level >= DEBUG_LEVEL) {
    debug_omitfollowing = false;
    Serial.print("\n");
    switch (level) {
      case INFO:
        Serial.print("[INFO] ");
        break;
      case DEBUG:
        Serial.print("[DEBUG] ");
        break;
      case ERR:
        Serial.print("[ERROR] ");
        break;
    }
  }
  else if (level != ADD)
    debug_omitfollowing = true;

  if (!debug_omitfollowing) {
    Serial.print(text);
  }
}
boolean bIsConnected = false;


// ############ LED / PWM Outputs ############
byte LED_mode = DIMM;
int LED_speed = 0;

byte LED_pin[] = {12, 14, 16, 10};
String mqtt_LED[ sizeof(LED_pin) ];
int LED_step = 0;
double LED_timer;
byte LED_out[] = {0, 0, 0, 0};
byte LED_val[] = {0, 0, 0, 0};

// ############ Switch / Button ############
#define SWITCH_MODE_NORM  0
#define SWITCH_MODE_MULTI 1
#define SWITCH_MODE_PIR 2

#define SWITCH_HOLDTIME 1000

const byte switch_child_pins[] = {5};
const byte switch_mode_pins[] = {SWITCH_MODE_MULTI};
String mqtt_Buttons[ sizeof(switch_child_pins) ];
Bounce debouncer[ sizeof(switch_child_pins) ];
boolean switch_oldValue[ sizeof(switch_child_pins) ];

// #### MultiButton Variables ####
double switch_downtimer[ sizeof(switch_child_pins) ];
double switch_endtimer[ sizeof(switch_child_pins) ];
byte switch_multitab[ sizeof(switch_child_pins) ];

//Button on this pin will reset the Wifi settings if hold when turned on
int BTN_pin = (int) switch_child_pins[0];

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("[[[[[[[[[[ESP8266]]]]]]]]");
  Serial.print("ChipID: ");
  Serial.println(ESP.getChipId());
  Serial.print("FlashChipID: ");
  Serial.println(ESP.getFlashChipId());
  Serial.print("Flash Speed: ");
  Serial.println(ESP.getFlashChipSpeed());
  Serial.print("Flash Size: ");
  Serial.println(ESP.getFlashChipSize());
  Serial.println("[[[[[[[[[[[[[|]]]]]]]]]]]]]");
  Serial.println();
  Serial.println("terraRGB Version 0.0");
  Serial.println();

  Serial.print("[RGB] Configure PWM-Outputs (");
  // Configure the RGB PWM Output
  for (int i = 0; i < sizeof(LED_pin); i++) {
    Serial.print(i);

    //Create MQTT Path for LED Channel
    String path = "LED";
    path += String(i);
    mqtt_LED[i] = getPath(path);
  }
  analogWriteRange(100);  //PWM Output-Range in percent!
  Serial.println(") --- DONE");

  //Configure and present all Switch Sensors
  Serial.print("[Button] Configure Switches, Buttons and Inputs (");
  for (int i = 0; i < sizeof(switch_child_pins); i++) {
    Serial.print(i);
    // Configure Input Pins with Internal Pullup Resistor!
    pinMode(switch_child_pins[i], INPUT_PULLUP);
    switch_oldValue[i] == HIGH;

    //Diese Zeile ist für PIRS
    if (switch_mode_pins[i] == SWITCH_MODE_PIR) {
      pinMode(switch_child_pins[i], INPUT);
    }

    //Create MQTT Path for Button Channel
    String path = "Button";
    path += String(i);
    mqtt_Buttons[i] = getPath(path);

    // After setting up the button, setup debouncer
    debouncer[i].attach(switch_child_pins[i]);
    debouncer[i].interval(5);
  }
  Serial.println(") --- DONE");


  readConfig();
  //end read

  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length

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
  mqtt_clientid = custom_mqtt_clientid.getValue();

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    writeConfig();
  }

  PrintDebug("", SUCC);
  PrintDebug("[WiFi] connected", SUCC);
  PrintDebug("[WiFi] IP address: ", SUCC);
  Serial.print(WiFi.localIP()); //printDebug would print IP as Integer!


  // setup MQTT Callback Functions
  myMqtt = MQTT(mqtt_clientid, mqtt_server, 1883);
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
    //Buttons();

  } else {

    // try to connect to mqtt server
    PrintDebug("[MQTT] Trying to connect to server", INFO);
    myMqtt.connect();
    delay(50); //wait for bIsConnected to be set correctly!

    if (bIsConnected) {
      // Successfully connected to mosquitto Server
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
      PrintDebug(" --- FAILED", ADD);
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
