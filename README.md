# emlearn Color-to-Fruits

A TinyML project that identifies fruit classes from color sensor readings and runs the classifier directly on a microcontroller.

This repository combines three parts of a complete TinyML workflow:
- **data collection on-device** with an APDS-9960 color sensor
- **model training in Python** inside the notebook at `/tmp/workspace/TronixLab/emlearn-Color-to-Fruits/Color_to_Fruits.ipynb`
- **embedded inference with emlearn** in the Arduino sketch at `/tmp/workspace/TronixLab/emlearn-Color-to-Fruits/color_to_fruits_identifier/color_to_fruits_identifier.ino`

## Project goal

The project uses reflected light measurements from fruit surfaces to classify a sample as one of these labels:
- Ambient
- Apple
- Blue Raspberry
- Guyabano
- Lemon
- Orange

The dataset in `/tmp/workspace/TronixLab/emlearn-Color-to-Fruits/dataset.csv` contains **600 labeled samples**, balanced across the six classes.

## Why this is a TinyML project

Instead of sending sensor data to a server, the trained neural network is exported to C/C++ and executed locally on the board. This keeps inference lightweight, fast, and fully offline.

The deployed model uses only six color features:
- normalized red
- normalized green
- normalized blue
- hue
- saturation
- value

These features are small enough for microcontroller inference while still carrying enough information to separate fruit classes by color.

## TinyML workflow

### 1. Collect labeled color data
The sketch at `/tmp/workspace/TronixLab/emlearn-Color-to-Fruits/color_features_collector/color_features_collector.ino`:
- prompts for the fruit label and number of samples over Serial
- turns on the RGB LEDs for consistent illumination
- reads color and proximity data from the APDS-9960 sensor
- normalizes RGB values and converts them to HSV
- prints labeled CSV rows for dataset creation

### 2. Train and evaluate the model
The notebook at `/tmp/workspace/TronixLab/emlearn-Color-to-Fruits/Color_to_Fruits.ipynb` walks through the ML pipeline:
- load and inspect the dataset
- visualize samples in RGB and HSV color space
- separate features and labels
- one-hot encode target classes
- standardize numeric features
- split the dataset for training, validation, and testing
- train a Keras neural network classifier
- evaluate predictions with accuracy, precision, recall, and F1 score

### 3. Export the model for embedded deployment
After training, the notebook converts the classifier with **emlearn** and generates embedded artifacts used by Arduino code:
- `/tmp/workspace/TronixLab/emlearn-Color-to-Fruits/color_to_fruits_identifier/StandardScaler.h`
- `/tmp/workspace/TronixLab/emlearn-Color-to-Fruits/color_to_fruits_identifier/NeuralNetworkClassifier.h`

This step preserves the same preprocessing and model behavior used during notebook training.

### 4. Run inference on the board
The deployment sketch at `/tmp/workspace/TronixLab/emlearn-Color-to-Fruits/color_to_fruits_identifier/color_to_fruits_identifier.ino`:
- reads live APDS-9960 sensor values
- rebuilds the same six input features used during training
- applies the exported standard scaler
- runs the emlearn-generated neural network
- prints the predicted fruit class and confidence to Serial

## Repository structure

- `/tmp/workspace/TronixLab/emlearn-Color-to-Fruits/Color_to_Fruits.ipynb` - notebook for training, evaluation, and model export
- `/tmp/workspace/TronixLab/emlearn-Color-to-Fruits/dataset.csv` - labeled color dataset used for model development
- `/tmp/workspace/TronixLab/emlearn-Color-to-Fruits/dataset.txt` - text export of the dataset
- `/tmp/workspace/TronixLab/emlearn-Color-to-Fruits/color_features_collector/` - Arduino sketch for collecting labeled samples
- `/tmp/workspace/TronixLab/emlearn-Color-to-Fruits/color_to_fruits_identifier/` - Arduino inference sketch and generated model headers
- `/tmp/workspace/TronixLab/emlearn-Color-to-Fruits/libraries/` - bundled libraries used by the Arduino sketches

## Main components

### Data collection
- `Arduino_APDS9960` for color and proximity sensing
- `ColorConverterLib` for RGB-to-HSV conversion
- `tinyml4all` helpers for serial prompts and CSV output

### Model deployment
- `emlearn` for converting the trained neural network to embedded C/C++
- `StandardScaler.h` for feature normalization on-device
- `NeuralNetworkClassifier.h` for the exported neural network

## Typical usage flow

1. Upload the collector sketch and capture labeled fruit samples.
2. Save the generated rows into the dataset file.
3. Open the notebook and train or retrain the classifier.
4. Export the updated scaler and neural network headers.
5. Upload the identifier sketch to run real-time fruit classification on-device.

## Summary

This repository is a compact example of an end-to-end TinyML application: gather sensor data, train a model, export it with emlearn, and deploy the classifier back to a microcontroller for real-time color-to-fruit identification.
