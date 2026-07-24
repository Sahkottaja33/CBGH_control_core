#include "sensors.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "config.h"

// ─────────────────────────────────────────────
//  sensors.cpp — korjattu versio
//  Muutos: kaikki anturit pyydetään kerralla
//  jolloin odotusaika on 750ms eikä 3x750ms
// ─────────────────────────────────────────────

static OneWire           oneWire(ONE_WIRE_BUS);
static DallasTemperature sensors(&oneWire);

static DeviceAddress addrIlma = ADDR_T_ILMA;
static DeviceAddress addrMaa  = ADDR_T_MAA;
static DeviceAddress addrUlko = ADDR_T_ULKO;

// Välimuisti viimeisimmille arvoille
static float viimIlma = SENSOR_ERROR;
static float viimMaa  = SENSOR_ERROR;
static float viimUlko = SENSOR_ERROR;

void initSensors() {
  sensors.begin();
  sensors.setResolution(addrIlma, 12);
  sensors.setResolution(addrMaa,  12);
  sensors.setResolution(addrUlko, 12);
  sensors.setWaitForConversion(true);

  Serial.print("Löydettiin ");
  Serial.print(sensors.getDeviceCount());
  Serial.println(" DS18B20-anturia.");
}

// Lukee kaikki kolme anturia yhdellä 750ms odotuksella
void updateSensors() {
  sensors.requestTemperatures();

  float t;

  t = sensors.getTempC(addrIlma);
  viimIlma = (t == DEVICE_DISCONNECTED_C) ? SENSOR_ERROR : t;

  t = sensors.getTempC(addrMaa);
  viimMaa = (t == DEVICE_DISCONNECTED_C) ? SENSOR_ERROR : t;

  t = sensors.getTempC(addrUlko);
  viimUlko = (t == DEVICE_DISCONNECTED_C) ? SENSOR_ERROR : t;

  if (viimIlma == SENSOR_ERROR) Serial.println("VAROITUS: T_ilma-anturi ei vastaa!");
  if (viimMaa  == SENSOR_ERROR) Serial.println("VAROITUS: T_maa-anturi ei vastaa!");
  if (viimUlko == SENSOR_ERROR) Serial.println("VAROITUS: T_ulko-anturi ei vastaa!");
}

float readTilma() { return viimIlma; }
float readTmaa()  { return viimMaa;  }
float readTulko() { return viimUlko; }


