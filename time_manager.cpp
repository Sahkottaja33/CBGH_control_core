#include "time_manager.h"
#include <Arduino.h>
#include <SD.h>
#include "data_logger.h"
#include "config.h"

// ... (kaikki aiempi koodi sellaisenaan) ...
static unsigned long baseEpoch       = 0;  // Unix time at millis()=0 equivalent
static unsigned long millisAtBase    = 0;  // millis() when baseEpoch was set
static unsigned long lastLoggedTs    = 0;  // last timestamp successfully written to CSV
static bool          timeIsValid     = false;

// ── Read last timestamp from CSV ─────────────
// Opens CSV and scans to the last line to find
// the most recent logged timestamp.
static unsigned long readLastTimestampFromCSV() {
  if (!SD.exists(CSV_FILE)) {
    Serial.println("[TimeManager] No CSV file found, starting fresh.");
    return 0;
  }

  File f = SD.open(CSV_FILE, FILE_READ);
  if (!f) {
    Serial.println("[TimeManager] Could not open CSV to read last timestamp.");
    return 0;
  }

  unsigned long lastTs = 0;
  String line = "";
  String lastDataLine = "";

  // Scan through all lines, keep the last non-header line
  while (f.available()) {
    char c = f.read();
    if (c == '\n') {
      line.trim();
      if (line.length() > 0 && !line.startsWith("timestamp")) {
        lastDataLine = line;
      }
      line = "";
    } else {
      line += c;
    }
  }
  // Handle file without trailing newline
  line.trim();
  if (line.length() > 0 && !line.startsWith("timestamp")) {
    lastDataLine = line;
  }
  f.close();

  if (lastDataLine.length() > 0) {
    // First field is the timestamp
    int commaIdx = lastDataLine.indexOf(',');
    if (commaIdx > 0) {
      lastTs = (unsigned long)lastDataLine.substring(0, commaIdx).toInt();
    }
  }

  return lastTs;
}

// ── Set base time ─────────────────────────────
static void setBase(unsigned long epochNow) {
  baseEpoch    = epochNow;
  millisAtBase = millis();
  timeIsValid  = true;
  Serial.print("[TimeManager] Base time set to: ");
  Serial.println(epochNow);
}

// ── Init ──────────────────────────────────────
void initTimeManager(unsigned long launchEpoch) {
  Serial.println("[TimeManager] Initializing...");

  unsigned long csvTs = readLastTimestampFromCSV();

  if (csvTs > 0) {
    Serial.print("[TimeManager] Last logged timestamp from CSV: ");
    Serial.println(csvTs);
    lastLoggedTs = csvTs;

    // Use CSV timestamp + 1 log interval as estimated current time
    // This is a safe minimum — user should correct via serial if needed
    unsigned long estimatedNow = csvTs + (LOG_INTERVAL_MS / 1000UL);

    // If launchEpoch is set and is more recent, prefer it
    if (launchEpoch > estimatedNow) {
      setBase(launchEpoch);
    } else {
      setBase(estimatedNow);
      Serial.println("[TimeManager] WARNING: Time estimated from CSV.");
      Serial.println("  If power was out for a while, correct the time via serial:");
      Serial.println("  Type a Unix timestamp and press Enter.");
    }
  } else if (launchEpoch > 0) {
    setBase(launchEpoch);
    Serial.println("[TimeManager] Using LAUNCH_EPOCH from config.");
  } else {
    Serial.println("[TimeManager] WARNING: No time reference available!");
    Serial.println("  Please set the time via serial monitor.");
    Serial.println("  Type a Unix timestamp and press Enter.");
    timeIsValid = false;
  }
}

// ── Current timestamp ─────────────────────────
unsigned long getCurrentTimestamp() {
  if (!timeIsValid) return 0;
  unsigned long elapsed = (millis() - millisAtBase) / 1000UL;
  return baseEpoch + elapsed;
}

// ── Update (call from loop) ───────────────────
void updateTimeManager() {
  // Currently just drives serial correction check
  // Could be extended for NTP sync if WiFi available
}

// ── Serial time correction ────────────────────
bool handleSerialTimeCorrection() {
  if (!Serial.available()) return false;

  String input = Serial.readStringUntil('\n');
  input.trim();

  // Must be a plausible Unix timestamp (> year 2026)
  unsigned long newTs = (unsigned long)input.toInt();
  const unsigned long MIN_VALID_TS = 1783703690UL; // 10.7.2026

  if (newTs < MIN_VALID_TS) {
    Serial.println("[TimeManager] Rejected: not a valid Unix timestamp.");
    Serial.println("  Get one from https://www.unixtimestamp.com");
    return false;
  }

  if (newTs <= lastLoggedTs) {
    Serial.print("[TimeManager] REJECTED: new timestamp (");
    Serial.print(newTs);
    Serial.print(") <= last logged timestamp (");
    Serial.print(lastLoggedTs);
    Serial.println("). This would overwrite existing data!");
    Serial.println("  Enter a timestamp AFTER the last logged entry.");
    return false;
  }

  unsigned long skipped = newTs - getCurrentTimestamp();
  Serial.print("[TimeManager] Time accepted. Approx. ");
  Serial.print(skipped / 3600);
  Serial.println(" hour(s) will be filled with zero rows.");

  setBase(newTs);
  return true;
}

// ── Confirm written timestamp ─────────────────
void confirmLoggedTimestamp(unsigned long ts) {
  if (ts > lastLoggedTs) {
    lastLoggedTs = ts;
  }
}

unsigned long getLastLoggedTimestamp() {
  return lastLoggedTs;
}

unsigned long getSkippedSeconds() {
  if (!timeIsValid || lastLoggedTs == 0) return 0;
  unsigned long now = getCurrentTimestamp();
  if (now > lastLoggedTs) return now - lastLoggedTs;
  return 0;
}
