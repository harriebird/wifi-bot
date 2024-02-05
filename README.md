# wifi-bot
Firmware for the WiFi Controlled robot. Powered by NodeMCU V3, an ESP8266-based
development board. A simple webpage is served to control the robot via WebSocket.
It collects data using the MQ135 Air Quality sensor and sends it to the web
dashboard. It uses both STA (Station) and AP (Access Point) WiFi modes. The AP
mode is used to expose the webpage to control the robot while the STA mode is
used to connect the robot to a router and transmit the sensor data.

## Setup
1. Clone this repository.
2. Install [Adruino IDE](https://www.arduino.cc/en/software).
3. Go to `Preferences > Additional Boards Manager URLs` and add the link below:
    * https://arduino.esp8266.com/stable/package_esp8266com_index.json
4. Open the Boards Manager and install `esp8266`.
5. Set `NodeMCU 1.0 (ESP-12E Module)` as your board.


## Requirements
### Software
* Arduino IDE
* Board:
    * NodeMCU V3
* Libraries:
    * arduinoWebSockets v2.1.4 by Markus Sattler
    * NewPing v1.9.1 by Tim Eckel
    * NTPClient v3.2.0 by Fabrice Weinberg
* VSCodium (optional)

### Hardware
* NodeMCU V3
* Smart Robot Car 2WD Motor Chassis
* 7.4V 2000 mAh LiPo Battery
* L298N Motor Driver Module
* HC-SR04 Ultrasonic Sensor
* MQ135 Air Quality Gas Sensor

## License
Code released under the [MIT License](LICENSE)
