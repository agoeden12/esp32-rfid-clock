# RFID Clock Hardware
This is the ESP32 code that runs the hardware for a physical clock system where members of a workshop can clock in and out of the shop in order to keep track of traffic due to Covid-19 Guidelines.

## Hardware Used
 - ESP-WROOM-32 (NodeMCU-32S)
 - RFID-RC522
 - 0.91" I2C SSD1306 OLED
 - 2x 10k Ohm Resistors
 - 2x Momentary Swiches (type doesn't matter)
 
## Wiring
  #### RFID-RC522
  | RFID-RC522 | ESP32 |
  |------------|-------|
  | RFID_SDA   | 4     |
  | RFID_SCK   | 18    |
  | RFID_MOSI  | 23    |
  | RFID_MISO  | 19    |
  | GND        | GND   |
  | RFID_RST   | 5     |
  | 3.3v       | 3.3v  |
  
  #### SSD1306 OLED
  | SSD1306    | ESP32 |
  |------------|-------|
  | RFID_SDA   | 21    |
  | RFID_SCL   | 22    |
  | VCC        | 3.3v  |
  | GND        | GND   |
  
  #### Momentary Switches
  |   Check In   |   Check Out  |  ESP32  |
  | ----------   | -----------  | ------- |
  | 3.3v         |              | 3.3v    |
  | GND->10k Ohm |              | GND     |
  | GND          |              | 13      |
  |              | 3.3v         | 3.3v    |
  |              | GND->10k Ohm | GND     |
  |              | GND          | 12      |
