#include "data_logger.h"
#include <SPI.h>
#include <SD.h>

// ─────────────────────────────────────────────
//  data_logger.cpp
//  Writes temperature data into an CSV-file on a SD-card
// ─────────────────────────────────────────────

static bool sdReady = false;

void initDataLogger(unsigned long launchEpoch) {
  SPI.begin(12, 13, 11, SD_CS); // SCK, MISO, MOSI, CS

  if (!SD.begin(SD_CS)) {
    Serial.println("Error: SD-card couldn't be found or initializing failed"); 
    sdReady = false;
    return;
  }

  sdReady = true;
  Serial.println("SD-card initialized ");

  // If the file does not exist, create it and write the header row.
  if (!SD.exists(CSV_FILE)) {
    File f = SD.open(CSV_FILE, FILE_WRITE);
    if (f) {
      f.println("timestamp,T_ilma,T_maa,T_ulko,fan_state");
      f.close();
      Serial.println("CSV-tiedosto luotu: " CSV_FILE);
    } else {
      Serial.println("VIRHE: CSV-tiedostoa ei voitu luoda!");
    }
  } else {
    Serial.println("CSV-tiedosto löytyi, jatketaan kirjoittamista.");
  }

  // Prints launch epoch as a reminder
  Serial.print("Launch epoch: ");
  Serial.println(launchEpoch);
}

void logData(unsigned long timestamp,
             float T_ilma, float T_maa, float T_ulko,
             bool fanOn) {
  if (!sdReady) {
    Serial.println("Warining: SD-card not usable, data will be lost"); 
    return;
  }

  File f = SD.open(CSV_FILE, FILE_APPEND);
  if (!f) {
    Serial.println("Error: The CSV-file couldn't be opened for writing"); 
    return;
  }

  // Writes a row: timestamp,T_ilma,T_maa,T_ulko,fan_state 
  f.print(timestamp);
  f.print(",");

  // Temperatures with 2 decimals accuracy
  if (T_ilma <= -100.0) f.print("ERROR"); else f.print(T_ilma, 2);
  f.print(",");
  if (T_maa  <= -100.0) f.print("ERROR"); else f.print(T_maa,  2);
  f.print(",");
  if (T_ulko <= -100.0) f.print("ERROR"); else f.print(T_ulko, 2);
  f.print(",");
  f.println(fanOn ? "ON" : "OFF");

  f.close();

  // Prints to Serial monitor for monitoring
  Serial.printf("LOG %lu | ilma=%.2f maa=%.2f ulko=%.2f tuuletin=%s\n",
                timestamp, T_ilma, T_maa, T_ulko, fanOn ? "ON" : "OFF");
}
