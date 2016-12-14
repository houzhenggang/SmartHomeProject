# SmartHomeProject
I started creating my Smart Home with openhab and mysensors.com - Arduino Nodes. But now I updated the whole setup to ESP8266 powered Wifi-Node, which communicate via MQTT with an mosquitto-Server on an openhab runtime.
Stay tuned for the developement.

## terraRGB
terraRGB is a simple source code, which will run on your ESP-Nodes and will provide a easy to setup solution, to get your in and outputs running with openhab via MQTT.

### Basic Concept
#### Buttons
In Button Topics there will be published numbers of clicks or a continuous holdingdown of min. 1,5s as "-1" value.
Holding a button for 15s will reset the config and boot in config mode.

### Commercial Hardware
There are several hardware components based on ESP8266 on the market, which can easy be flashed with this or your own firmware.
* [H801](commercial_h801.md) (5-channel LED Dimmer) [DC 5-24V]
* [Sonoff](commercial_sonoff.md) (Relay module) [AC 90-250V 10A]
