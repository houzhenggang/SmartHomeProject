/**********q******************************
 *
 *
 *
 *
 *
 *
 *
 *
 */#include <ESP8266WiFi.h>
#include <MQTT.h>


#define CLIENT_ID "WzRGB"
#define DIMM 0
#define INSTANT 1
#define FLASH 2
#define RAINBOW 3
#define SAWTOOTH 4
#define PULSE 5

// create MQTT object, Enter Your MQTT-Server Data here:
MQTT myMqtt(CLIENT_ID, "192.168.1.1", 1883);

// Enter your Wifi Settings here:
const char* ssid     = "SSID";
const char* password = "Wifi pass";

boolean bIsConnected = false;

byte LED_mode = DIMM;
int LED_speed = 0;
int LED_pin[] = {4, 16, 5};
int LED_step = 0;
double LED_timer;
byte LED_out[] = {0, 0, 0};
byte LED_val[] = {0, 0, 0};

//
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  for (int i = 0; i < 3; i++) {
    pinMode(LED_pin[i], OUTPUT);
  }
  analogWriteRange(100);
  

  while (WiFi.status() != WL_CONNECTED) {
    delay(10);
    LED_step += 5;
    if (LED_step >= 255) {
      Serial.print(".");
      LED_step = 0;
    }
    SetLED(LED_step, LED_step, LED_step);
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
  } else {
    // try to connect to mqtt server
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






