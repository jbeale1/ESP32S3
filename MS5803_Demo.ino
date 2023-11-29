
/******************************************************************************
  SparkFun_MS5803_Demo.ino   Demo Program for MS5803 pressure sensors.
  Casey Kuhns @ SparkFun Electronics
  7/20/2014
  https://github.com/sparkfun/MS5803-14BA_Breakout/
  modified by J.Beale 28-Nov-2023

  The MS58XX MS57XX and MS56XX by Measurement Specialties is a low cost I2C pressure
  sensor.  This sensor can be used in weather stations and for altitude
  estimations. It can also be used underwater for water depth measurements.

  Resources:
  This library uses the Arduino Wire.h to complete I2C transactions.

  Development environment specifics:
	IDE: Arduino 1.0.5
	Hardware Platform: Arduino Pro 3.3V/8MHz
	T5403 Breakout Version: 1.0

  **Updated for Arduino 1.8.8 5/2019**
  License: Please see LICENSE.md for more details.
  Distributed as-is; no warranty is given.
******************************************************************************/

#include <Wire.h>
#include <SparkFun_MS5803_I2C.h> // Click here to get the library: http://librarymanager/All#SparkFun_MS5803-14BA

// Begin class with selected address
// available addresses (selected by jumper on board)
// default is ADDRESS_HIGH

//  ADDRESS_HIGH = 0x76
//  ADDRESS_LOW  = 0x77
MS5803 sensor(ADDRESS_HIGH); // Instantiate the sensor using ADDRESS_HIGH
//MS5803 sensor; // Or, from v1.1.3, we can also do this. The address will default to ADDRESS_HIGH

//Create variables to store results
float temperature_c, temperature_f;
double pressure_abs, pressure_relative, altitude_delta, pressure_baseline;

// Create Variable to store altitude in (m) for calculations;
double base_altitude = 105.0; // local altitude in meters

void setup() {

  Wire.begin();
  Serial.begin(9600);
  sensor.reset();   //Retrieve calibration constants for conversion math.
  sensor.begin(Wire, 0x76); // start comms with sensor

  pressure_baseline = sensor.getPressure(ADC_4096);
  Serial.println("degC,degF,mbar_abs,mbar_rel,deltaH_m");
}

  // Available ADC precision is 8,9,10,11,12 bits:
  // ADC_256
  // ADC_512
  // ADC_1024
  // ADC_2048
  // ADC_4096

void loop() {

int numAvg = 100;

  temperature_c = 0;
  pressure_abs = 0;

  for(int i=0; i<numAvg; i++) {
    temperature_c += sensor.getTemperature(CELSIUS, ADC_4096);  
    pressure_abs += sensor.getPressure(ADC_4096);  // mbar
    delay(50);
  }
  temperature_c /= numAvg;  // get average value of this many readings
  pressure_abs /= numAvg;

  pressure_relative = sealevel(pressure_abs, base_altitude);
  altitude_delta = altitude(pressure_abs , pressure_baseline);
  temperature_f = (temperature_c * 1.8) + 32;

  Serial.print(temperature_c,3);
  Serial.print(", ");
  Serial.print(temperature_f,3);
  Serial.print(", ");
  Serial.print(pressure_abs,2);
  Serial.print(", ");
  Serial.print(pressure_relative,2);
  Serial.print(", ");
  Serial.println(altitude_delta);

} // end loop()

// Thanks to Mike Grusin for letting me borrow the functions below from
// the BMP180 example code.

// Given a pressure P (mbar) taken at a specific altitude (meters),
// return the equivalent pressure (mbar) at sea level.
// This produces pressure readings that can be used for weather measurements.
double sealevel(double P, double A)
{
  return (P / pow(1 - (A / 44330.0), 5.255));
}

// Given a pressure measurement P (mbar) and the pressure at a baseline P0 (mbar),
// return altitude (meters) above baseline.
double altitude(double P, double P0)
{
  return (44330.0 * (1 - pow(P / P0, 1 / 5.255)));
}
