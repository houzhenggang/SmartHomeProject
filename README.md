# SmartHomeProject
I started creating my Smart Home with openhab and mysensors.com - Arduino Nodes. But now I updated the whole setup to ESP8266 powered Wifi-Node, which communicate via MQTT with an mosquitto-Server on an openhab runtime.
Stay tuned for the developement.

## H801
This Project is working with the H801, too. It may be the first proffesionell product with an integrated ESP8266, which can be flashed with your own Sketch via the well accessable pins.

The 5 Dimmer-Channels (R, G, B, W1 and W2) are connected to GPIO 15, 13, 12, 14 and 4.

So your sketch shall look like:
```byte LED_pin[] = {12, 13, 15, 14, 4};```
