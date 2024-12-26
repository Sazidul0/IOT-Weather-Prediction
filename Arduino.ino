
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define WIND_SENSOR_PIN A0
#define CALIBRATION_COEFFICIENT 5.0

Adafruit_BME280 bme;

void setup() {
  Serial.begin(9600);
  
  // Initialize BME280 sensor
  bool status = bme.begin(0x76);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor.");
    while (1);
  }

  // Wait for sensor to stabilize
  delay(2000);
}

void loop() {
  // Read sensor data
  int sensorValue = analogRead(WIND_SENSOR_PIN);
  float outvoltage = sensorValue * (5.0 / 1023.0);
  float windSpeed = outvoltage * CALIBRATION_COEFFICIENT;
  
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F;

  // Send data in a comma-separated format: temperature, humidity, pressure, windSpeed
  Serial.print(temperature);
  Serial.print(",");
  Serial.print(humidity);
  Serial.print(",");
  Serial.print(pressure);
  Serial.print(",");
  Serial.println(windSpeed);

  delay(1000);  // Delay before next reading
}