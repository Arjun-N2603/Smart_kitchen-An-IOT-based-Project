#define BLYNK_TEMPLATE_ID           "TMPL3m-lKA-kq"
#define BLYNK_TEMPLATE_NAME         "Quickstart Template"
#define BLYNK_AUTH_TOKEN            "8F9VEyQZXIhGI55bpKxikfKIJhaHMIYs"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

char ssid[] = " ";
char pass[] = " ";

DHT dht(D3, DHT11); // DHT sensor is connected to pin D3
BlynkTimer timer;

// Define pins for flame sensor, original buzzer, MQ2 sensor, and new buzzer
#define FLAME_SENSOR_PIN D4
#define BUZZER_PIN D7
#define MQ2_SENSOR_PIN A0
#define NEW_BUZZER_PIN D2

// Function to send temperature and humidity values to Blynk
void sendSensorData() {
 float h = dht.readHumidity();
 float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

 if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
 }
 Serial.print("Humidity: ");
 Serial.print(h);
 Serial.print(" %\t");
 Serial.print("Temperature: ");
 Serial.print(t);
 Serial.println(" *C");
 Blynk.virtualWrite(V5, h); // V5 is for Humidity
 Blynk.virtualWrite(V6, t); // V6 is for Temperature
}

// Function to check flame sensor and control original buzzer
void flame() {
 int flameValue = digitalRead(FLAME_SENSOR_PIN);
 if (flameValue == HIGH) {
    // Flame detected, turn on the original buzzer
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("No flame detected, turning off the flame buzzer.");
 } else {
    // No flame detected, turn off the original buzzer
    digitalWrite(BUZZER_PIN, HIGH);
    Serial.println("Flame detected, turning on the flame buzzer.");
 }
}

// Function to check MQ2 sensor and control new buzzer
void checkGas() {
 int gasValue = analogRead(MQ2_SENSOR_PIN); // Use analogRead for MQ2 sensor
 Serial.print("Gas Sensor Value: ");
 Serial.println(gasValue); // Print the gas sensor value to the Serial Monitor

 if (gasValue > 650) { // Assuming 650 is the threshold for high gas value
    // High gas value detected, turn on the new buzzer
    Serial.println("High gas value detected, turning on the gas buzzer.");
    digitalWrite(NEW_BUZZER_PIN, HIGH);
 } else {
    // Gas value is normal, turn off the new buzzer
    Serial.println("Normal gas value detected.");
    digitalWrite(NEW_BUZZER_PIN, LOW);
 }

 // Send the gas sensor value to Blynk
 Blynk.virtualWrite(V3, gasValue); // V3 is for Gas Sensor Value
}

BLYNK_WRITE(V0) {
 int value = param.asInt();
 Blynk.virtualWrite(V1, value);
}

void myTimerEvent() {
 Blynk.virtualWrite(V2, millis() / 1000);
}

void setup() {
 Serial.begin(38400);
 Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
 dht.begin(); // Initialize the DHT sensor
 pinMode(FLAME_SENSOR_PIN, INPUT); // Set flame sensor pin as input
 pinMode(BUZZER_PIN, OUTPUT); // Set original buzzer pin as output
 pinMode(MQ2_SENSOR_PIN, INPUT); // Set MQ2 sensor pin as input
 pinMode(NEW_BUZZER_PIN, OUTPUT); // Set new buzzer pin as output
 timer.setInterval(1000L, sendSensorData); // Set up the timer to send sensor data every second
 timer.setInterval(1000L, myTimerEvent); // Keep the existing timer event
 timer.setInterval(1000L, flame); // Set up the timer to check flame and control original buzzer every second
 timer.setInterval(1000L, checkGas); // Set up the timer to check gas and control new buzzer every second
}

void loop() {
 Blynk.run();
 timer.run();
}
