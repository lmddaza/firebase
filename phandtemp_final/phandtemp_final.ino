#include <Arduino.h>
#if defined(ESP32)
#include <FirebaseESP32.h>
#endif

#include <OneWire.h>            // Library for the temperature sensor
#include <DallasTemperature.h>  // Library for the temperature sensor

// Temperature sensor setup
#define ONE_WIRE_BUS 2          // Pin for the temperature sensor
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// pH sensor setup
#define pHpin 34                // Pin for the pH sensor

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

#define WIFI_SSID "akatsuki"
#define WIFI_PASSWORD "N0FreeLOADER!!"
#define API_KEY "AIzaSyAcfqwMQSPENZOECuIoZi1U-HlQCGGaHK4" // Replace with your Firebase API Key
#define DATABASE_URL "https://test-b113d-default-rtdb.asia-southeast1.firebasedatabase.app/" // Replace with your Firebase Realtime Database URL
#define USER_EMAIL "mariejodene@gmail.com" // Replace with your Firebase user email
#define USER_PASSWORD "jodenecute" // Replace with your Firebase user password

// Define Firebase Data object
FirebaseData firebaseData;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;

void setup() {
  // Initialize temperature sensor
  sensors.begin();

  // Initialize serial communication
  Serial.begin(9600);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("CONNECTED:");
  Serial.println(WiFi.localIP());

  config.api_key = API_KEY;

  /* Assign the user sign-in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long-running token generation task */
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Firebase.setDoubleDigits(2);
}

void loop() {
  // Read temperature value
  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);

  // Read pH value
  int sensorValue = analogRead(pHpin);
  float pH = map(sensorValue, 0, 4095, 0, 14); // Map the sensor reading to pH scale (ESP32 has a 12-bit ADC, range 0-4095)

  // Print values to the serial monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.printf("Set Temp... %s\n", Firebase.setDouble(firebaseData, "/Sensors/Temp", temperature) ? "ok" : firebaseData.errorReason().c_str());
  Serial.printf("Get Temp... %s\n", Firebase.getDouble(firebaseData, "/Sensors/Temp") ? String(firebaseData.to<double>()).c_str() : firebaseData.errorReason().c_str());

  delay(1000);  // Delay for 1 second
}