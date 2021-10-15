#include <Arduino.h>
#include <SimpleDHT.h>

//#define DHT_SENSOR_TYPE DHT11
#define DHT_SENSOR_PIN 8
#define THERMISTOR_PIN 0

#define THERMISTOR_RESISTOR 10000.0
#define THERMISTOR_VOLTAGE 5.0

SimpleDHT11 dht11(DHT_SENSOR_PIN);

bool climateMeasureHeater(float *temperature) {
  int tempReading = analogRead(THERMISTOR_PIN);
  // Serial.println(tempReading);
  // This is OK
  double voltage = 1023.0 / tempReading - 1.0;
  double tempK = log(THERMISTOR_RESISTOR * voltage);
  tempK = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * tempK * tempK )) * tempK );       //  Temp Kelvin
  *temperature = float(tempK - 273.15);

  return true ;
}

bool climateMeasureEnvironment(float *temperature, float *humidity) {
  static unsigned long timestamp = millis();
  int err = SimpleDHTErrSuccess;
  
  /* Measure once every four seconds. */
  if(millis() - timestamp > 3000ul) {
    if ((err = dht11.read2(temperature, humidity, NULL)) != SimpleDHTErrSuccess) {
      return false;
    }
    timestamp = millis();
    return true ;
  }

  return false ;
}

void climateInitialize() {
  // pinMode(THERMISTOR_PIN, OUTPUT);
}