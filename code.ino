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

// LED on PWM-capable pin
#define LED_PIN 6  // Ensure this is a PWM pin (D5, D6, D9, D10, or D11)

// Number of samples to average
const int numSamples = 5;
const int minDistance = 2;    // Minimum valid distance
const int maxDistance = 100;  // Beyond this, LED will be off

long getAverageDistance() {
  long totalDistance = 0;
  int validReadings = 0;

  for (int i = 0; i < numSamples; i++) {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 30000);
    if (duration == 0) {
      delay(10);
      continue;
    }

    int distance = duration * 0.034 / 2;

    if (distance >= minDistance && distance <= 400) {
      totalDistance += distance;
      validReadings++;
    }

    delay(10);
  }

  return (validReadings > 0) ? (totalDistance / validReadings) : -1;
}

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.display();
}

void loop() {
  long distance = getAverageDistance();

  Serial.print("Distance: ");
  if (distance == -1) {
    Serial.println("No valid reading");
  } else {
    Serial.print(distance);
    Serial.println(" cm");
  }

  display.clearDisplay();
  display.setCursor(0, 10);

  if (distance == -1) {
    display.println("Distance: --");
  } else {
    display.print("Distance: ");
    display.print(distance);
    display.println(" cm");
  }
  display.display();

  // Adjust LED brightness based on distance
  if (distance != -1 && distance <= maxDistance) {
    int brightness = map(distance, minDistance, maxDistance, 255, 0);
    brightness = constrain(brightness, 0, 255);
    analogWrite(LED_PIN, brightness);
  } else {
    analogWrite(LED_PIN, 0);
  }

  delay(500);
}
