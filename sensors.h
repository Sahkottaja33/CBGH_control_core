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

#define ONE_WIRE_BUS 4

// ── Change these: replace 0x00-values with real addresses ──
// Example address: { 0x28, 0xFF, 0x3A, 0x12, 0x00, 0x00, 0x00, 0xAB }
#define ADDR_T_ILMA { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define ADDR_T_MAA  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define ADDR_T_ULKO { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
// ─────────────────────────────────────────────────────────

// Error-value ei the sensor is not responding
#define SENSOR_ERROR -999.0

void  initSensors();
float readTilma();
float readTmaa();
float readTulko();

#endif
