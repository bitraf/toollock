# TheLock
An electronic lock connected via wireless (Wi-Fi) to MsgFlo / MQTT. It can be used for locking things like tools, doors, etc.

## Things needed
Microcontroller - ESP8266 or similar. We used a Wemos D1 board.
actuator - we used a solenoid that is rated for 12V (it actually actuates at about 7V, but slowly), it draws about 0.3 - 0.7 A
mosfet - we used a IPP055N03L
resistor - a 10k ohm resistor for pulldown on the control signal
power supply - a 12V DC power supply for the actuator / solenoid

## Setup
Setup wifi connection: Add a file `Config.h` defining parameters `WIFI_SSID` and `WIFI_PASSWORD`

## State
In progress

## License
MIT
