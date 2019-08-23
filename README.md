# wifi_beehive_lamps
Uses an ESP8266 NodeMCU WIFI dev. board to update a three lamp Western Electric "beehive" status lamp

This project updates a 3-lamp Western Electric "beehive" lamp set for WIFI operation. It may be used to indicate various conditions in an Asterisk PBX by inserting "wget" statements in the code to control the three LEDs. A minimalist web server runs on the NodeMCU board to permit LED control and status check from a web browser.

To build, simply connected the anode of each LED through a seies current-limiting resistor. The output of the NodeMCU board pins can only supply 10mA at 3.3 volts. Some LEDs drop 2.5 volts, so the resistor values are fairly critical and small. Connect the small piezo speaker directly to the NodeMCU pin and ground.

Use the following connections:

int whiteled = D1; 
int greenled = D2; 
int redled = D5; 
int beeper = D6; 

The pin IDs used by the Arduino IDE are marked on the TOP of the NodeMCU board. Do not use the NoseMCU native pin IDs or the GPIO IDs. See Google for pin maps showing the three pin numbering schemes.

I simply wire wrapped connecting wires to the socket of the NodeMCU, but use whatever construction method suits you. Power the unit through the USB cable for convenience, or make connections directly to the board. I used a small USB power bank to power mine for portable operation.

I used a 75 ohm resistor for a white LED with a measured 2.5 volt drop and 150 ohm resistor for a red LED with a 2.0 volt drop. I used a cool white LED beneath the green and white domes and a red LED under the red dome. The colors and resistor values provided balenced brightness with ny beehive unit. Current was about 8mA per LED.

See repository for photos of a modified WE beehive unit. Of course, any three LED setup may be used. I removed the 48 volt switchnoard bayonet base lamps from the unit. The NodeMCU module fit neatly into the base. I used strips of prototyping board to mount the LEDs and current control resistors. Hot melt glue secured everything in place, including the small piezo speaker.

You need to load the ESP8266 board definitions into the Arduino IDE before compiling. I used the default board settings.

You need to hard-code your SSID and wifi password into the code. After loading the first time, open the serial console on the Arduino IDE. The program will print the connection status and the assigned DHCP IP address. Use the indicated URL to manually control the LEDs.\

The code runs a lamp test on power-up, lighting each LED in turn, then beeping once. The unit indicates WIFI status by giving a very brief flash on the green LED every 15 seconds to indicate the unit is connected. If the green LED is switched on, the green LED will blink "off" in the same fashion as a status and heartbeat indicator.

Similarly, if WIFI connectivity is lost, the red LED will blink every 15 secondss along with a low beep. Inverse blinking is used if the red LED is already lit.

When an LED is illuminated, it gives a beep in a unique tone for that color LED. When the LED is turned off, it gives a somewhat shorter beep at the same frequency. Multiple on or off commands will each give a beep even if the LED is already on or off.

The following Linux commands may also be used to remotely control the LEDs from a machine on the same local LAN. Substitute the IP address displayed in the serial console:


wget -q http://xxx.xxx.xxx.x/WHITELED=OFF
wget -q http://xxx.xxx.xxx.x/REDLED=OFF
wget -q http://xxx.xxx.xxx.x/GREENLED=OFF
wget -q http://xxx.xxx.xxx.x/WHITELED=ON
wget -q http://xxx.xxx.xxx.x/REDLED=ON
wget -q http://xxx.xxx.xxx.x/GREENLED=ON

I use in my  Asterisk sysstem to indicate the status of a group of trunks. The lamp lights if there is at least one call to a specific group of trunks or extensions active. I set up a global variable to track incoming calls in extensions.conf in the [globals] area:

[globals]
CNETCOUNT=0

Then, there needs to be a single entry point context for all calls that the LED tracks. Multiple entry points work too if the same code captures all extensions to be tracked:

exten => _X.,1,SetGlobalVar(MFTRUNKCOUNT=$[${CNETCOUNT} + 1]
exten => _X.,n,System(wget -q http://192.168.1.6/GREENLED=ON)

This code increments the global call counter and turns on the LED.

In the "h" extension of all contexts where the call may terminate (be sure to include any nacro contexts called in the extension):

exten => h,1,SetGlobalVar(CNETCOUNT=$[${CNETCOUNT} - 1]
exten => h,n,GotoIf($[ ${CNETCOUNT} > 0]?endcall:ledoff)
exten => h,n(ledoff),System(wget -q http://192.168.1.6/GREENLED=OFF)
exten => h,n(endcall),HangUp

This decrements the call counter and turns off the LED if no call remain active.

It is helpfult to set up an extension to reset counters and LEDs if things get out of sync:

exten => 888,1,SetGlobalVar(PMFPSTNCOUNT=0)
exten => 888,n,SetGlobalVar(CNETCOUNT=0)
exten => 888,n,SetGlobalVar(MFTRUNKCOUNT=0
exten => 888,n,System(wget -q http://192.168.1.6/WHITELED=OFF)
exten => 888,n,System(wget -q http://192.168.1.6/REDLED=OFF)
exten => 888,n,System(wget -q http://192.168.1.6/GREENLED=OFF)
exten => 888,n,PlayTones(!1400/500)
exten => 888,n,Wait(1)
exten => 888,n,Hangup


