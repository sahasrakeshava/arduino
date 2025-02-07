#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// HC-SR04 sensor pins
#define TRIG_PIN 9
#define ECHO_PIN 10

// Built-in LED
#define LED_PIN 13

// Number of samples to average
const int numSamples = 5;

long getAverageDistance() {
  long totalDistance = 0;
  int validReadings = 0;
  
  for (int i = 0; i < numSamples; i++) {
    // Trigger the sensor
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Read the pulse; timeout after 30ms if no echo is received
    long duration = pulseIn(ECHO_PIN, HIGH, 30000);
    if (duration == 0) {
      // Skip this sample if no echo is received
      delay(10);
      continue;
    }

    // Convert duration to distance in cm (speed of sound ~0.034 cm/µs)
    int distance = duration * 0.034 / 2;

    // Accept only valid readings between 2 and 400 cm
    if(distance >= 2 && distance <= 400) {
      totalDistance += distance;
      validReadings++;
    }
    
    delay(10); // Short delay between samples
  }
  
  // Return the average distance; if no valid readings, return -1
  if (validReadings > 0) {
    return totalDistance / validReadings;
  } else {
    return -1;
  }
}

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  
  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;) ; // Loop forever if OLED initialization fails
  }
  
  display.clearDisplay();
  display.setTextSize(2);     // Larger text for better readability
  display.setTextColor(WHITE);
  display.display();
}

void loop() {
  long distance = getAverageDistance();
  
  // Print to Serial Monitor
  Serial.print("Distance: ");
  if (distance == -1) {
    Serial.println("No valid reading");
  } else {
    Serial.print(distance);
    Serial.println(" cm");
  }
  
  // Update OLED display
  display.clearDisplay();
  display.setCursor(0, 10);
  
  // Check if distance is -1 (no valid reading) or the default 16 cm
  if (distance == -1 || distance == 16) {
    display.println("Distance: --");
  } else {
    display.print("Distance: ");
    display.print(distance);
    display.println(" cm");
  }
  display.display();
  
  // LED indicator: turn on LED if object is closer than 10 cm
  if (distance != -1 && distance < 10) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
  
  delay(500);  // Update every 500 ms
}
