#include <Arduino.h>
#include <SimpleDHT.h>

//#define DHT_SENSOR_TYPE DHT11
#define DHT_SENSOR_PIN 6
#define HEATER_BLK_PIN 8
#define HEATER_FAN_PIN 9

#define THERMISTOR_PIN 0
#define THERMISTOR_RESISTOR 10000.0
#define THERMISTOR_VOLTAGE 5.0

SimpleDHT11 dht11(DHT_SENSOR_PIN);

bool climateMeasureHeater(float *temperature) {
  int sensorValue = 0;
  int samples = 5;

  for (int i = 0; i < samples; i++) {
    sensorValue += analogRead(THERMISTOR_PIN);
  }

  sensorValue /=  samples;

  double tempK = log(THERMISTOR_RESISTOR * (1024.0 / (1024.0 - sensorValue) - 1));
  tempK = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * tempK * tempK )) * tempK );       //  Temp Kelvin
  *temperature = tempK - 273.15;  

  return true;
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