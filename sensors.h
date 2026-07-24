#ifndef SENSORS_H
#define SENSORS_H

// ─────────────────────────────────────────────
//  sensors.h
//  DS18B20-sensor controls (1-Wire, GPIO4)
//
//  FILL IN THE SENSOR ADDRESSES in the arrays below.
//  First run your test code (the current version) with all
//  three sensors connected and record the hex address
//  printed in the serial monitor for each sensor.

// ─────────────────────────────────────────────

#define ONE_WIRE_BUS 5


// ──────────────────────────────────────────────

// Error-value if the sensor is not responding
#define SENSOR_ERROR -999.0

void  initSensors();
void  updateSensors();  // kutsu tätä ennen readTilma/Tmaa/Tulko
float readTilma();
float readTmaa();
float readTulko();

#endif