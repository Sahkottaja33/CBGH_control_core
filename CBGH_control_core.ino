/*
 * Thermal storage- temperature monitoring and fan controls
 * 
 * Hardware:
 *  - ESP32-S3-DevKitC-1
 *  - DS18B20 x3 (GPIO4, 1-Wire)
 *  - MH-SD Card Module (SPI: CS=10, MOSI=11, SCK=12, MISO=13)
 *  - Shelly Plug S (HTTP, ESP32 AP-mode)
 * 
 * Included files:
 *  - projekti.ino    : Main program, setup(), loop(), WiFi AP
 *  - sensors.h/.cpp  : DS18B20-sensor reading logic
 *  - fan_control.h/.cpp : Fan logic + HTTP-controls for Smart plug
 *  - data_logger.h/.cpp : SD-card, CSV-logging
 */

#include <WiFi.h>
#include "sensors.h"
#include "fan_control.h"
#include "data_logger.h"

// ─────────────────────────────────────────────
//  MAGIC NUMBER: aseta lanseeraushetken Unix-timestamp
//  Hae osoitteesta https://www.unixtimestamp.com
//  juuri ennen laitteen käyttöönottoa
// ─────────────────────────────────────────────
#define LAUNCH_EPOCH 1718700000UL

// WiFi AP -asetukset
#define AP_SSID     "Maalampo-AP"
#define AP_PASSWORD "salasana123"   // vaihda haluamaksesi, väh. 8 merkkiä

// Intervallit
#define FAN_CHECK_INTERVAL_MS    (30UL * 1000UL)      // 30 sekuntia
#define LOG_INTERVAL_MS          (15UL * 60UL * 1000UL) // 15 minuuttia

unsigned long lastFanCheck = 0;
unsigned long lastLog      = 0;


void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }
  delay(1000);
  Serial.println("=== Maalämpövarasto käynnistyy ===");

  // Starts the Wifi Access Point 
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  Serial.print("AP käynnissä, IP: ");
  Serial.println(WiFi.softAPIP());

  initSensors();
  initDataLogger(LAUNCH_EPOCH);
  initFanControl();

  Serial.println("=== Alustus valmis ===");
}


void loop() {
  unsigned long now = millis();

  // Reads values as fresh values
  float T_ilma = readTilma();
  float T_maa  = readTmaa();
  float T_ulko = readTulko();

  // Fan controls every 30 seconds
  if (now - lastFanCheck >= FAN_CHECK_INTERVAL_MS) {
    lastFanCheck = now;
    updateFanControl(T_ilma, T_maa);
  }

  // Logs data every 15 minutes
  if (now - lastLog >= LOG_INTERVAL_MS) {
    lastLog = now;
    unsigned long timestamp = LAUNCH_EPOCH + (now / 1000UL);
    logData(timestamp, T_ilma, T_maa, T_ulko, getFanState());
  }

  delay(1000); // small break
}
