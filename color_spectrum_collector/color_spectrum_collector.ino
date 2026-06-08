#include <ColorConverterLib.h>
#include <Arduino_APDS9960.h>
#include <tinyml4all.h>

// Create an instance of the tinyml4all data collection utilities
using tinyml4all::promptString;
using tinyml4all::promptInt;
using tinyml4all::printCSV;

// Define RGB LED pins
#define LEDR 22  // Pin for Red LED
#define LEDG 23  // Pin for Green LED
#define LEDB 24  // Pin for Blue LED

void setup(void) {
  // Start serial communication for debugging and data output
  Serial.begin(9600);

  // Wait for the serial connection to be established
  while (!Serial) {
    delay(10);
  }

  // Initialize APDS-9960 sensor
  if (!APDS.begin()) {
    Serial.println("Error initializing APDS-9960 sensor.");
  }

  // Initialize RGB pins as outputs
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);

  // Initialize LEDs to off state
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);
  delay(500);
}

void loop(void) {
  // Get fruit name and number of samples from user
  String fruit = promptString("Which fruit is this?");
  int numSamples = promptInt("How many samples to capture?");

  // Variables to hold sensor readings and color conversions
  int r, g, b, c, p;
  double hue, saturation, value, rgb_sum;

  // Turn on RGB LEDs to illuminate the fruit for better color readings
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, LOW);
  digitalWrite(LEDB, LOW);
  delay(500);

  for (int i = 0; i < numSamples; i++) {
    // Wait for proximity and color reading is available
    while (!APDS.colorAvailable() || !APDS.proximityAvailable()) {}

    // Read the color and proximity data
    APDS.readColor(r, g, b, c);
    p = APDS.readProximity();

    // Calculate the sum of RGB values for normalization
    rgb_sum = r + g + b;

    // If the board is close and well enough lit, collect the data
    if (p == 0 && c > 10 && rgb_sum > 0) {
      // Normalize RGB values to account for brightness variations
      double r_norm = r / rgb_sum;
      double g_norm = g / rgb_sum;
      double b_norm = b / rgb_sum;

      // Convert RGB to HSL and HSV color spaces
      RGBConverter::RgbToHsv(r, g, b, hue, saturation, value);

      // Print the collected data in CSV format for easy import into analysis tools
      printCSV(r_norm, g_norm, b_norm, hue, saturation, value, fruit);
      delay(250);
    }
  }

  // Turn off RGB LEDs after data collection
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);
}