#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

// ─────────────────────────────────────────────
//  data_logger.h
//  SD-card + CSV-writing
//
//  SPI-pins: CS=10, MOSI=11, SCK=12, MISO=13
//
//  CSV-structure:
//  timestamp,T_ilma,T_maa,T_ulko,fan_state
//  1718700045,21.50,18.25,19.75,ON
//
//  File name: /data.csv 
//  If the file allready exists, continues writing in that file.
//  Writes to the end of the file (to avoid overwriting data).
// ─────────────────────────────────────────────
#define RGB_LED_PIN 38
#define SD_CS   10
#define CSV_FILE "/data.csv"
#define LOG_INTERVAL_MS (15UL * 60UL * 1000UL)

void initDataLogger(unsigned long launchEpoch);

void logData(unsigned long timestamp,
             float T_ilma, float T_maa, float T_ulko,
             bool fanOn);
void addSkippedIntervals(unsigned long count);

#endif
