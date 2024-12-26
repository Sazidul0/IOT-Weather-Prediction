# Real-Time Rain Prediction System Using Machine Learning

## Overview

This project implements a real-time rain prediction system using machine learning techniques. The system collects weather data from sensors and predicts the likelihood of rain using a trained machine learning model.

## Table of Contents

- [Introduction](#introduction)
- [Hardware Requirements](#hardware-requirements)
- [Hardware Connections](#hardware-connections)
- [Usage](#usage)
- [Project Flow](#project-flow)
- [License](#license)

## Introduction

The Real-Time Rain Prediction System utilizes an Arduino Uno and an ESP32 to gather environmental data such as temperature, humidity, pressure, and wind speed. This data is then sent to a Flask API running in the cloud, which uses a machine learning model to predict if it will rain the next day. The predictions are based on real-time data collected from the sensors.

## Hardware Requirements

- Arduino Uno
- ESP32
- BME280 Sensor (for temperature, humidity, and pressure)
- Anemometer (for wind speed)
- Jumper wires
- Breadboard

## Hardware Connections

### Arduino Uno Connections

- **BME280 Sensor:**
  - VCC to 3.3V
  - GND to GND
  - SDA to A4
  - SCL to A5

- **Anemometer:**
  - Connect the output pin of the anemometer to the analog pin A0 on the Arduino.

### ESP32 Connections

- **Serial Communication with Arduino:**
  - Connect the TX pin of the Arduino to GPIO 16 (RX) of the ESP32.
  - Connect the RX pin of the Arduino to GPIO 17 (TX) of the ESP32.

## Usage

- Connect the Arduino and ESP32 to your computer.
- Ensure both devices are powered and connected to the same network.
- Access the web interface by navigating to `http://<ESP32_IP_ADDRESS>/` in your web browser.
- The interface will display real-time weather data and the rain prediction.
- The machine learning algorithm is running in the cloud, and the API endpoint for predictions is `https://iot-weather-prediction.onrender.com/predict`. This endpoint takes four values: AvgTemp, Humidity9am, Pressure9am, and WindSpeed9am, which are sent from the ESP32 to the server for rain prediction.

## Project Flow

1. **Data Collection:**
   - The Arduino Uno collects real-time weather data using the BME280 sensor (temperature, humidity, pressure) and the anemometer (wind speed).
   - The collected data is formatted and sent to the ESP32 via serial communication.

2. **Data Transmission:**
   - The ESP32 receives the data from the Arduino and hosts a web server to display the live data.
   - The ESP32 also prepares the data for transmission to the cloud API.

3. **Machine Learning Prediction:**
   - The ESP32 sends the collected data to the cloud-based Flask API at the endpoint `https://iot-weather-prediction.onrender.com/predict`.
   - The API processes the incoming data and uses a pre-trained machine learning model to predict the likelihood of rain.

4. **Result Retrieval:**
   - The API returns the prediction result (e.g., whether it will rain tomorrow) back to the ESP32.
   - The ESP32 updates the web interface to display the prediction result alongside the live weather data.

5. **User  Interaction:**
   - Users can access the web interface to view real-time weather data and the rain prediction, providing an interactive experience.

## License

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
