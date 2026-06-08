#include <ColorConverterLib.h>
#include <Arduino_APDS9960.h>
#include "NeuralNetworkClassifier.h"
#include "StandardScaler.h"

// Create scaler instance
StandardScaler scaler;

// Array to hold scaled features
float features[StandardScaler::n_features];

// Array to hold scaled features
float features_scaled[StandardScaler::n_features];

// Define an array for class labels
const char* classes[] = { "Ambient", "Apple", "Blue raspberry", "Guyabano", "Lemon", "Orange" };

// Array to hold prediction probabilities
float proba[StandardScaler::n_features];

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
  // Variables to hold sensor readings and color conversions
  int r, g, b, c, p;
  double hue, saturation, value, rgb_sum;

  // Turn on RGB LEDs to illuminate the fruit for better color readings
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, LOW);
  digitalWrite(LEDB, LOW);
  delay(500);

  // Wait for proximity and color reading is available
  while (!APDS.colorAvailable() || !APDS.proximityAvailable()) {}

  // Read the color and proximity data
  APDS.readColor(r, g, b, c);
  p = APDS.readProximity();

  // Calculate the sum of RGB values for normalization
  rgb_sum = r + g + b;

  // Define an index to keep track of the current feature being collected
  int feature_ix = 0;

  // If the board is close and well enough lit, collect the data
  if (p == 0 && c > 10 && rgb_sum > 0) {
    // Normalize RGB values to account for brightness variations
    double r_norm = r / rgb_sum;
    double g_norm = g / rgb_sum;
    double b_norm = b / rgb_sum;

    // Convert RGB to HSL and HSV color spaces
    RGBConverter::RgbToHsv(r, g, b, hue, saturation, value);

    // Store features in the array
    features[feature_ix++] = r_norm;
    features[feature_ix++] = g_norm;
    features[feature_ix++] = b_norm;
    features[feature_ix++] = hue;
    features[feature_ix++] = saturation;
    features[feature_ix++] = value;

    // Check if we have collected all features
    if (feature_ix >= StandardScaler::n_features) {
      // Scale features
      scaler.transform(features, features_scaled);

      // Make prediction
      int32_t y_pred = NeuralNetworkClassifier_predict(features_scaled, StandardScaler::n_features);

      // Get prediction probabilities
      EmlError err = eml_net_predict_proba(&NeuralNetworkClassifier, features_scaled, StandardScaler::n_features, proba, 6);
      if (err == EmlOk) {
        // Print prediction probability
        Serial.print("Predicted Class: ");
        Serial.print(classes[y_pred]);
        Serial.print("\t Probability: ");
        Serial.print(proba[y_pred] * 100, 2);
        Serial.println("%");
      }
      // Reset features index for the next reading
      feature_ix = 0;
    }
  }
  delay(100);
}