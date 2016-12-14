# SmartHomeProject
I started creating my Smart Home with openhab and mysensors.com - Arduino Nodes. But now I updated the whole setup to ESP8266 powered Wifi-Node, which communicate via MQTT with an mosquitto-Server on an openhab runtime.
Stay tuned for the developement.

## terraRGB
terraRGB is a simple source code, which will run on your ESP-Nodes and will provide a easy to setup solution, to get your in and outputs running with openhab via MQTT.

### Commercial ESP Boards Setup
#### H801
This Project is working with the H801, too. It may be the first proffesionell product with an integrated ESP8266, which can be flashed with your own Sketch via the well accessable pins.

The 5 Dimmer-Channels (R, G, B, W1 and W2) are connected to GPIO 15, 13, 12, 14 and 4.

So your sketch shall look like:
     
    byte LED_pin[] = {12, 13, 15, 14, 4};

##### openhab Configuration Example
conf/items/terraRGB.items

    Group	Room	  "Example Room"     <bedroom>
    Group	terraRGB	"terraRGB Settings"   <energy>

    Switch 	terra_RGBon	"terraRGBsWand"				  (Room, terraRGB, Lights)  [ "homekit:Lightbulb" ]
    Dimmer	terra_R		"RGB - Rot"					  	  (terraRGB)		{mqtt=">[mosquitto:terraRGB/LED0:command:*:default]"}
    Dimmer	terra_G		"RGB - Grün"				      (terraRGB)		{mqtt=">[mosquitto:terraRGB/LED1:command:*:default]"}
    Dimmer	terra_B		"RGB - Blau"				  	  (terraRGB)		{mqtt=">[mosquitto:terraRGB/LED2:command:*:default]"}
    Dimmer	terra_W1		"RGB - White1"			    (terraRGB)		{mqtt=">[mosquitto:terraRGB/LED3:command:*:default]"}
    Dimmer	terra_W2		"RGB - White2"			    (terraRGB)		{mqtt=">[mosquitto:terraRGB/LED4:command:*:default]"}
    Dimmer	terra_RGBmode	"RGB - Mode"		  	  (terraRGB)		{mqtt=">[mosquitto:terraRGB/mode:command:*:default]"}
    Dimmer	terra_RGBspd	"RGB - Speed"		  		(terraRGB)		{mqtt=">[mosquitto:terraRGB/speed:command:*:default]"}
    Color  	terra_RGB   	"Cinewall RGB"	<rgb>	(Room, terraRGB)

conf/rules/terraRGB.rules

    var HSBType hsbValue
    var String  redValue
    var String  greenValue
    var String  blueValue

    rule "Set terraRGB value"
    when
            Item terra_RGB changed
    then
            hsbValue = terra_RGB.state as HSBType

            redValue   = hsbValue.red.intValue.toString
            greenValue = hsbValue.green.intValue.toString
            blueValue  = hsbValue.blue.intValue.toString

      if(terra_RGBon.state == ON){
            sendCommand( terra_R, redValue )
            sendCommand( terra_G, greenValue)
            sendCommand( terra_B, blueValue )
      }
      else{
            sendCommand( terra_R, 0 )
            sendCommand( terra_G, 0 )
            sendCommand( terra_B, 0 )
        }
    end

    rule "Set terraRGB ON"
    when
            Item terra_RGBon changed
    then
      if(terra_RGBon.state == ON){
            hsbValue = terra_RGB.state as HSBType

            redValue   = hsbValue.red.intValue.toString
            greenValue = hsbValue.green.intValue.toString
            blueValue  = hsbValue.blue.intValue.toString

            sendCommand( terra_R, redValue )
            sendCommand( terra_G, greenValue)
            sendCommand( terra_B, blueValue )
        }
        else{
            sendCommand( terra_R, 0 )
            sendCommand( terra_G, 0 )
            sendCommand( terra_B, 0 )
        }
    end
