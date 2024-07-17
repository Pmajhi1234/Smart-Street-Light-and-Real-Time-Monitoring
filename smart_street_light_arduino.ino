#include <boltiot.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <SoftwareSerial.h>

// Replace with your network credentials
const char* ssid = " ";
const char* password = " ";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200, 60000); // Update interval is set to 60 seconds (25200 seconds offset for timezone adjustment)

int sensor1 = 12; // Pin for sensor 1
int led1 = 14; // Pin for LED 1
int count1 = 0; // Counter for sensor 1 activations
unsigned long ledOnTime1 = 0;  // Variable to store the time when LED 1 was turned on
const unsigned long duration = 2000; // Duration for the LED to stay on (in milliseconds)

int sensor2 = 27; // Pin for sensor 2
int led2 = 26; // Pin for LED 2
int count2 = 0; 
unsigned long ledOnTime2 = 0; 

int sensor3 = 33; // Pin for sensor 3
int led3 = 32; // Pin for LED 3
int count3 = 0; 
unsigned long ledOnTime3 = 0; 

const unsigned long interval = 1 * 30 * 1000; // 1 minute interval for checking the time
unsigned long previousMillis = 0; // Variable to store the last time the time was checked
int currentHour = 0; // Variable to store the current hour

void setup() {
  // Initialize serial for debugging
  Serial.begin(9600);
  boltiot.begin(16,17);

  // Initialize sensors and LEDs
  pinMode(sensor1, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(sensor2, INPUT);
  pinMode(led2, OUTPUT);
  pinMode(sensor3, INPUT);
  pinMode(led3, OUTPUT);

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // Initialize NTPClient
  timeClient.begin();
}

void loop() {
  unsigned long currentMillis = millis();

  // This part of the code checks for time per minute, you can change it by changing the value of interval
  if (currentMillis - previousMillis >= interval) {
    // Save the last time you checked the time
    previousMillis = currentMillis;

    // Update the time from NTP server
    timeClient.update();

    // Get the current hour (0-23)
    currentHour = timeClient.getHours();
    Serial.print("Updated current hour: ");
    Serial.println(currentHour);
  }

  Serial.print("Current hour: ");
  Serial.println(currentHour);

  // Check if the current time is between 5 PM and 11 PM (17:00 - 23:00)
  if (currentHour >= 17 && currentHour <= 23) {
    // Check sensor1
    if (digitalRead(sensor1) == LOW) {
      analogWrite(led1, 255); // Turn on LED 1 at full brightness
      count1++; // Increment counter for sensor 1
      ledOnTime1 = millis(); // Record the time LED 1 was turned on
    }
    if ((millis() - ledOnTime1) >= duration) {
      analogWrite(led1, 5); // Dim LED 1 after the duration
    }

    // Check sensor2
    if (digitalRead(sensor2) == LOW) {
      analogWrite(led2, 255); 
      count2++; 
      ledOnTime2 = millis(); 
    }
    if ((millis() - ledOnTime2) >= duration) {
      analogWrite(led2, 5); 
    }

    // Check sensor3
    if (digitalRead(sensor3) == LOW) {
      analogWrite(led3, 255); 
      count3++; 
      ledOnTime3 = millis(); 
    }
    if ((millis() - ledOnTime3) >= duration) {
      analogWrite(led3, 5); // 
    }

    // Send data to Bolt IoT platform
    boltiot.processPushDataCommand(count1, count2, count3);
  }
  
  // Check if the current time is between midnight and 3 AM (00:00 - 03:00), in this period half of the street lights will be switched off.
  else if(currentHour >= 0 && currentHour <= 3) {
    if (digitalRead(sensor1) == LOW) {
      analogWrite(led1, 255); // Turn on LED 1 at full brightness
      count1++; 
      ledOnTime1 = millis(); 
    }
    if ((millis() - ledOnTime1) >= duration) {
      analogWrite(led1, 5); // Dim LED 1 after the duration
    }

    // Check sensor2
    if (digitalRead(sensor2) == LOW || digitalRead(sensor2) == HIGH) {
      analogWrite(led2, 0); // Turn off LED 2 regardless of sensor state
      count2++; 
    }

    // Check sensor3
    if (digitalRead(sensor3) == LOW) {
      analogWrite(led3, 255); // Turn on LED 3 at full brightness
      count3++; 
      ledOnTime3 = millis(); 
    }
    if ((millis() - ledOnTime3) >= duration) {
      analogWrite(led3, 5); // Dim LED 3 after the duration
    }
  }

  // For other hours, turn off all LEDs
  else {
    analogWrite(led1, 0); 
    analogWrite(led2, 0); 
    analogWrite(led3, 0); 
  }
  delay(2000); // Wait for 2 seconds before next loop iteration
}
