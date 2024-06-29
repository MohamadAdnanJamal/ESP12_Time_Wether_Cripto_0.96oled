ESP8266 Weather and Crypto Display
This Arduino sketch for the ESP8266 microcontroller connects to Wi-Fi to fetch and display the current time, weather information, and cryptocurrency prices on an OLED screen.

Features
Wi-Fi Connectivity: Connects to a specified Wi-Fi network.
Time Display: Fetches the current time from an NTP server and displays it.
Weather Data: Retrieves weather information from OpenWeatherMap.
Cryptocurrency Prices: Fetches prices from CoinGecko.
OLED Display: Shows time, weather, and cryptocurrency data on an OLED screen.
Requirements
ESP8266 microcontroller
SSD1306 OLED display
Arduino IDE with the following libraries installed:
Wire
ESP8266WiFi
ESP8266HTTPClient
ArduinoJson
Adafruit_GFX
Adafruit_SSD1306
NTPClient
WiFiUdp
Libraries Installation
You can install the required libraries via the Arduino Library Manager:

Open the Arduino IDE.
Go to Sketch > Include Library > Manage Libraries...
Search for and install the following libraries:
ESP8266WiFi
ESP8266HTTPClient
ArduinoJson
Adafruit GFX Library
Adafruit SSD1306
NTPClient
WiFiUdp
Setup
Connect the OLED Display to ESP8266:

VCC to 3.3V
GND to GND
SCL to D5
SDA to D6
Configure the sketch:

Replace ssid and password with your Wi-Fi credentials.
Replace openWeatherMapApiKey with your OpenWeatherMap API key.
Modify city and countryCode for your location.
cpp
Copy code
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";
String openWeatherMapApiKey = "your_OpenWeatherMap_API_key";
String city = "your_city";
String countryCode = "your_country_code";
Upload the sketch:
Connect your ESP8266 to the computer.
Select the correct board and port in the Arduino IDE.
Click Upload.
Usage
Once uploaded, the ESP8266 will connect to the Wi-Fi network, fetch the current time, weather data, and cryptocurrency prices, and display them on the OLED screen. The information is updated regularly.

Troubleshooting
Ensure the correct Wi-Fi credentials are provided.
Verify the connections between the ESP8266 and the OLED display.
Check the serial monitor for debugging information.
License
This project is licensed under the MIT License.
