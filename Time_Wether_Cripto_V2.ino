#include <Wire.h> // Library for I2C communication
#include <ESP8266WiFi.h> // Library for Wi-Fi connectivity
#include <ESP8266HTTPClient.h> // Library for HTTP requests
#include <ArduinoJson.h> // Library for parsing JSON
#include <Adafruit_GFX.h> // Graphics library for the OLED display
#include <Adafruit_SSD1306.h> // Library for the SSD1306 OLED display
#include <Fonts/FreeSerif9pt7b.h> // Font for the OLED display
#include <Fonts/FreeSerifBoldItalic9pt7b.h> // Another font for the OLED display
#include <NTPClient.h> // Library for NTP (Network Time Protocol)
#include <WiFiUdp.h> // Library for UDP communication

// Replace with your network credentials
const char* ssid = "your_SSID"; // SSID of the Wi-Fi network
const char* password = "your_PASSWORD"; // Password of the Wi-Fi network

// OLED display configuration
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin (not used)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // Initialize the OLED display

// Time configuration
const long utcOffsetInSeconds = 10800; // UTC offset in seconds
char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" }; // Array of days of the week
WiFiUDP ntpUDP; // UDP instance for NTP
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds); // NTP client instance

// Weather configuration
String openWeatherMapApiKey = "your_OpenWeatherMap_API_key"; // API key for OpenWeatherMap
String city = "your_city"; // City name for weather data
String countryCode = "your_country_code"; // Country code for weather data
String jsonBuffer; // Buffer to store JSON data
float temp_C = 0.0; // Variable to store temperature in Celsius
int humidity = 0; // Variable to store humidity
float wind_speed_float = 0.0; // Variable to store wind speed
String sky_clearance; // Variable to store sky condition
unsigned long previousWeatherMillis = 0; // Timestamp for the last weather update

// Crypto configuration
const unsigned long CriptoRefreshInterval = 150000; // Refresh interval for cryptocurrency prices (150 seconds)
unsigned long CriptopreviousMillis = 0; // Timestamp for the last cryptocurrency update
int btcUsd, ethUsd, solUsd; // Variables to store cryptocurrency prices
float btcUsdChange, ethUsdChange, solUsdChange; // Variables to store cryptocurrency price changes

// Function declarations
void connectToWiFi(); // Function to connect to Wi-Fi
void checkConnection(); // Function to check Wi-Fi connection
void displayTime(); // Function to display the time
void displayWeather(); // Function to display the weather
void displayCrypto(); // Function to display cryptocurrency prices
void updateWeather(); // Function to update weather data
String httpGETRequest(const char* serverName); // Function to perform HTTP GET request
void fetchCryptoPrices(); // Function to fetch cryptocurrency prices
String formatWithCommas(int value); // Function to format numbers with commas

void setup() {
  Serial.begin(115200); // Initialize serial communication at a baud rate of 115200
  Wire.begin(D6, D5); // Initialize I2C communication with specified SDA and SCL pins

  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed")); // Print error message if the display initialization fails
    for (;;); // Infinite loop to stop further execution
  }

  connectToWiFi(); // Connect to the Wi-Fi network
  timeClient.begin(); // Start the NTP client to get time
  updateWeather(); // Fetch initial weather data
  fetchCryptoPrices(); // Fetch initial cryptocurrency prices
}

void loop() {
  checkConnection(); // Check Wi-Fi connection and reconnect if necessary
  unsigned long currentMillis = millis(); // Get the current time in milliseconds

  // Fetch cryptocurrency prices at regular intervals
  if (currentMillis - CriptopreviousMillis >= CriptoRefreshInterval) {
    CriptopreviousMillis = currentMillis; // Update the timestamp
    fetchCryptoPrices(); // Fetch new cryptocurrency prices
  }

  display.clearDisplay(); // Clear the display buffer
  display.setFont(NULL); // Reset the font
  display.setTextSize(1); // Set text size to 1
  display.setTextColor(WHITE); // Set text color to white

  displayTime(); // Display the current time
  displayWeather(); // Display the current weather information
  displayCrypto(); // Display the current cryptocurrency prices

  display.display(); // Update the display with the buffer content

  // Fetch weather data at regular intervals
  if (currentMillis - previousWeatherMillis >= 60000) {
    previousWeatherMillis = currentMillis; // Update the timestamp
    updateWeather(); // Fetch new weather data
  }
}

void connectToWiFi() {
  display.clearDisplay(); // Clear the display buffer
  display.setFont(&FreeSerif9pt7b); // Set the font
  display.setTextSize(1); // Set text size to 1
  display.setTextColor(WHITE); // Set text color to white
  display.setCursor(0, 12); // Set the cursor position
  display.println("Connecting to:"); // Print "Connecting to:" on the display
  display.setFont(NULL); // Reset the font
  display.setCursor(0, 20); // Set the cursor position
  display.println(ssid); // Print the SSID on the display
  display.display(); // Update the display with the buffer content

  WiFi.begin(ssid, password); // Start Wi-Fi connection with provided credentials
  unsigned long startAttemptTime = millis(); // Get the current time in milliseconds
  int progress = 0; // Variable to store progress percentage

  // Wait for connection until timeout
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 30000) {
    // Update loading bar
    int barWidth = map(progress, 0, 100, 0, SCREEN_WIDTH - 4); // Calculate the width of the loading bar
    display.fillRect(2, SCREEN_HEIGHT - 30, barWidth, 8, WHITE); // Draw the loading bar on the display
    display.display(); // Update the display with the buffer content

    delay(200); // Wait for 200 milliseconds
    progress = map(millis() - startAttemptTime, 0, 30000, 0, 100); // Calculate progress percentage
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi network with IP Address: " + WiFi.localIP().toString()); // Print IP address on serial monitor
    display.clearDisplay(); // Clear the display buffer
    display.setTextSize(1); // Set text size to 1
    display.setTextColor(WHITE); // Set text color to white
    display.setCursor(0, 0); // Set the cursor position
    display.println("Connected to WiFi"); // Print "Connected to WiFi" on the display
    display.setCursor(0, 17); // Set the cursor position
    display.println(WiFi.localIP().toString()); // Print IP address on the display
    display.display(); // Update the display with the buffer content
    delay(2000); // Wait for 2 seconds
  } else {
    Serial.println("Failed to connect to WiFi"); // Print error message on serial monitor
    display.clearDisplay(); // Clear the display buffer
    display.setTextSize(1); // Set text size to 1
    display.setTextColor(WHITE); // Set text color to white
    display.setCursor(0, 0); // Set the cursor position
    display.println("Failed to connect to WiFi"); // Print error message on the display
    display.display(); // Update the display with the buffer content
    delay(2000); // Wait for 2 seconds
    connectToWiFi(); // Retry connection
  }
}

void checkConnection() {
  // If Wi-Fi connection is lost, reconnect
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost!"); // Print error message on serial monitor
    display.clearDisplay(); // Clear the display buffer
    display.setFont(&FreeSerif9pt7b); // Set the font
    display.setTextSize(1); // Set text size to 1
    display.setTextColor(WHITE); // Set text color to white
    display.setCursor(0, 12); // Set the cursor position
    display.println("WiFi connection lost! Reconnecting..."); // Print error message on the display
    display.display(); // Update the display with the buffer content
    connectToWiFi(); // Reconnect to Wi-Fi
  }
}

void displayTime() {
  timeClient.update(); // Update the time from the NTP server
  int hours = timeClient.getHours(); // Get the current hour
  int minutes = timeClient.getMinutes(); // Get the current minute

  bool isAM = true; // Variable to store AM/PM status
  if (hours >= 12) {
    isAM = false; // Set PM status if hour is 12 or greater
  }
  if (hours == 0) {
    hours = 12; // Convert 0 hour to 12
  } else if (hours > 12) {
    hours -= 12; // Convert 24-hour format to 12-hour format
  }

  // Blinking colon logic
  static bool colonVisible = true; // Variable to store colon visibility status
  static unsigned long lastBlinkMillis = 0; // Timestamp for the last blink
  const unsigned long blinkInterval = 500; // Blink interval in milliseconds
  unsigned long currentMillis = millis(); // Get the current time in milliseconds

  if (currentMillis - lastBlinkMillis >= blinkInterval) {
    colonVisible = !colonVisible; // Toggle colon visibility
    lastBlinkMillis = currentMillis; // Update the timestamp
  }

  display.setTextSize(2); // Set text size to 2
  display.setCursor(0, 0); // Set the cursor position
  
  // Display hours in 00 format
  if (hours < 10) {
    display.print('0'); // Print leading zero if hours are less than 10
  }
  display.print(hours); // Print hours
  
  display.setCursor(20, 0); // Set the cursor position
  display.print(colonVisible ? ":" : " "); // Print colon or space based on visibility status
  display.setCursor(30, 0); // Set the cursor position

  // Display minutes in 00 format
  if (minutes < 10) {
    display.print('0'); // Print leading zero if minutes are less than 10
  }
  display.print(minutes); // Print minutes

  // Draw a vertical line separator
  for (int i = 0; i < 16; i++) {
    display.drawPixel(60, i, SSD1306_WHITE); // Draw a pixel at specified position
  }
}

void displayWeather() {
  display.setTextSize(2); // Set text size to 2
  display.setCursor(70, 0); // Set the cursor position
  display.print(temp_C, 1); // Print temperature with one decimal place

  display.setTextSize(1); // Set text size to 1
  display.setCursor(0, 20); // Set the cursor position
  display.print(humidity); // Print humidity
  display.print("% "); // Print percentage symbol

  display.setCursor(25, 20); // Set the cursor position
  display.print(" "); // Print a space
  display.print(wind_speed_float, 1); // Print wind speed with one decimal place
  display.print(" m/s "); // Print "m/s" (meters per second)
}

void displayCrypto() {
  // Format cryptocurrency prices with commas
  String btcUsdStr = formatWithCommas(btcUsd); // Format Bitcoin price
  String ethUsdStr = formatWithCommas(ethUsd); // Format Ethereum price
  String solUsdStr = formatWithCommas(solUsd); // Format Solana price

  display.setTextSize(1); // Set text size to 1
  display.setCursor(0, 30); // Set the cursor position
  display.print("BTC:"); // Print "BTC:"
  display.print(btcUsdStr); // Print formatted Bitcoin price
  display.setCursor(65, 30); // Set the cursor position
  display.print(btcUsdChange); // Print Bitcoin price change

  display.setCursor(0, 40); // Set the cursor position
  display.print("ETH: "); // Print "ETH:"
  display.print(ethUsdStr); // Print formatted Ethereum price
  display.setCursor(65, 40); // Set the cursor position
  display.print(ethUsdChange); // Print Ethereum price change

  display.setCursor(0, 50); // Set the cursor position
  display.print("SOL: "); // Print "SOL:"
  display.print(solUsdStr); // Print formatted Solana price
  display.setCursor(65, 50); // Set the cursor position
  display.print(solUsdChange); // Print Solana price change
}

void updateWeather() {
  // Construct the API URL for fetching weather data
  String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;
  jsonBuffer = httpGETRequest(serverPath.c_str()); // Perform HTTP GET request
  DynamicJsonDocument doc(1024); // Create a JSON document
  deserializeJson(doc, jsonBuffer); // Parse the JSON response

  // Parse and update weather data
  temp_C = doc["main"]["temp"].as<float>() - 273.15; // Convert temperature from Kelvin to Celsius
  humidity = doc["main"]["humidity"].as<int>(); // Get humidity
  wind_speed_float = doc["wind"]["speed"].as<float>(); // Get wind speed
  sky_clearance = doc["weather"][0]["description"].as<String>(); // Get sky condition description
}

String httpGETRequest(const char* serverName) {
  WiFiClient client; // Create a Wi-Fi client
  HTTPClient http; // Create an HTTP client
  http.begin(client, serverName); // Initialize HTTP request
  int httpResponseCode = http.GET(); // Send the GET request
  String payload = "{}"; // Default payload

  if (httpResponseCode > 0) {
    payload = http.getString(); // Get the response payload
  } else {
    Serial.println("Error code: " + String(httpResponseCode)); // Print error code
  }
  http.end(); // Close the connection
  return payload; // Return the payload
}

void fetchCryptoPrices() {
  if (WiFi.status() == WL_CONNECTED) { // Check if connected to Wi-Fi
    HTTPClient http; // Create an HTTP client
    WiFiClientSecure client; // Create a secure Wi-Fi client
    client.setInsecure(); // Disable SSL certificate validation
    http.begin(client, "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin,ethereum,solana&vs_currencies=usd&include_24hr_change=true"); // Initialize HTTP request
    int httpCode = http.GET(); // Send the GET request

    if (httpCode > 0) {
      String payload = http.getString(); // Get the response payload
      DynamicJsonDocument doc(2048); // Create a JSON document
      deserializeJson(doc, payload); // Parse the JSON response

      // Parse and update cryptocurrency prices and changes
      btcUsd = doc["bitcoin"]["usd"].as<int>(); // Get Bitcoin price
      btcUsdChange = doc["bitcoin"]["usd_24h_change"].as<float>(); // Get Bitcoin price change
      ethUsd = doc["ethereum"]["usd"].as<int>(); // Get Ethereum price
      ethUsdChange = doc["ethereum"]["usd_24h_change"].as<float>(); // Get Ethereum price change
      solUsd = doc["solana"]["usd"].as<int>(); // Get Solana price
      solUsdChange = doc["solana"]["usd_24h_change"].as<float>(); // Get Solana price change
    }
    http.end(); // Close the connection
  }
}

String formatWithCommas(int value) {
  // Format integer values with commas
  String result = ""; // Initialize result string
  String valueStr = String(value); // Convert value to string
  int len = valueStr.length(); // Get the length of the string

  for (int i = 0; i < len; i++) {
    result += valueStr[i]; // Append character to result
    if ((len - i - 1) % 3 == 0 && i != len - 1) {
      result += ','; // Append comma if necessary
    }
  }

  return result; // Return the formatted string
}
