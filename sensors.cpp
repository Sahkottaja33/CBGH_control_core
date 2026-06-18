#include "sensors.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// ─────────────────────────────────────────────
//  sensors.cpp
//  DS18B20-sensor reading based on their address
// ─────────────────────────────────────────────

static OneWire           oneWire(ONE_WIRE_BUS);
static DallasTemperature sensors(&oneWire);

static DeviceAddress addrIlma = ADDR_T_ILMA;
static DeviceAddress addrMaa  = ADDR_T_MAA;
static DeviceAddress addrUlko = ADDR_T_ULKO;

void initSensors() {
  sensors.begin();
  Serial.print("FOUND ");
  Serial.print(sensors.getDeviceCount());
  Serial.println(" DS18B20-Sensors.");

  // Sets all the sensors to a 12-bit accuracy
  sensors.setResolution(addrIlma, 12);
  sensors.setResolution(addrMaa,  12);
  sensors.setResolution(addrUlko, 12);

  // This is a non‑blocking call; requestTemperatures() returns right away and does not pause execution.
  sensors.setWaitForConversion(false);
}

// Internal function: Request temperature and returns a value
static float readSensor(DeviceAddress addr) {
  sensors.requestTemperaturesByAddress(addr);
  delay(750); // 12-bit conversion takes max 750 ms
  float t = sensors.getTempC(addr);
  if (t == DEVICE_DISCONNECTED_C) {
    Serial.println("Warning: The sensor is not responding!");
    return SENSOR_ERROR;
  }
  return t;
}

float readTilma() { return readSensor(addrIlma); }
float readTmaa()  { return readSensor(addrMaa);  }
float readTulko() { return readSensor(addrUlko); }
