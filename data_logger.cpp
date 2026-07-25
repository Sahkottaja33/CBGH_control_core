#include "data_logger.h"
#include "time_manager.h"
#include <SPI.h>
#include <SD.h>
#include <Adafruit_NeoPixel.h>

// ─────────────────────────────────────────────
//  data_logger.cpp — korjattu versio
//
//  Muutokset:
//  1. Tiedostokahva suljetaan AINA myös virheessä
//  2. SD-kortti tarkistetaan joka kirjoituksella
//     → jos kortti puuttuu, kirjoitetaan 0-arvot
//     kun se kytketään takaisin
//  3. Skipattuja jaksoja seurataan laskurilla
//     ja kirjoitetaan nollarivinä kun SD palaa
// ─────────────────────────────────────────────
static Adafruit_NeoPixel statusLed(1, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);

static void setSdStatusLed(bool ok) {
  if (ok) {
    statusLed.setPixelColor(0, statusLed.Color(0, 35, 0));   // dim green
  } else {
    statusLed.setPixelColor(0, statusLed.Color(35, 0, 0));   // dim red
  }
  statusLed.show();
}
static bool          sdReady        = false;
static unsigned long skippedCount   = 0;  // montako logia on skippattu ilman SD:tä
static unsigned long lastTimestamp  = 0;  // viimeisin yritetty timestamp

// Yrittää alustaa SD:n, palauttaa true jos onnistui
static bool yritysSdAlustus() {
  if (!SD.begin(SD_CS)) {
    return false;
  }
  // Luo otsikkorivi jos tiedosto puuttuu
  if (!SD.exists(CSV_FILE)) {
    File f = SD.open(CSV_FILE, FILE_WRITE);
    if (f) {
      f.println("timestamp,T_ilma,T_maa,T_ulko,fan_state");
      f.close();
      Serial.println("CSV-tiedosto luotu: " CSV_FILE);
    } else {
      SD.end();
      return false;
    }
  }
  return true;
}

void initDataLogger(unsigned long launchEpoch) {
  statusLed.begin();
  statusLed.setBrightness(50);
  
  SPI.begin(12, 13, 11, SD_CS);
  sdReady = yritysSdAlustus();
  setSdStatusLed(sdReady);

  if (sdReady) {
    Serial.println("SD-kortti alustettu.");
  } else {
    Serial.println("VAROITUS: SD-korttia ei löydy käynnistyksessä.");
    Serial.println("  Laite toimii ilman SD:tä ja yrittää uudelleen kirjoitettaessa.");
  }

  Serial.print("Launch epoch: ");
  Serial.println(launchEpoch);
}

// Kirjoittaa yhden CSV-rivin — kutsutaan sekä normaalisti
// että skippattujen rivien täyttämiseen nollilla
static void kirjoitaRivi(File &f, unsigned long ts,
                          float T_ilma, float T_maa, float T_ulko,
                          bool fanOn) {
  f.print(ts);
  f.print(",");
  if (T_ilma <= -100.0) f.print("0"); else f.print(T_ilma, 2);
  f.print(",");
  if (T_maa  <= -100.0) f.print("0"); else f.print(T_maa,  2);
  f.print(",");
  if (T_ulko <= -100.0) f.print("0"); else f.print(T_ulko, 2);
  f.print(",");
  f.println(fanOn ? "ON" : "OFF");
}

void logData(unsigned long timestamp,
             float T_ilma, float T_maa, float T_ulko,
             bool fanOn) {

  lastTimestamp = timestamp;

  // Jos SD ei ole käytössä, yritetään alustaa uudelleen
  if (!sdReady) {
    Serial.println("SD ei käytössä, yritetään uudelleenalustusta...");
    sdReady = yritysSdAlustus();
    setSdStatusLed(sdReady);

    if (!sdReady) {
      skippedCount++;
      Serial.printf("SD edelleen poissa. Skipattuja jaksoja: %lu\n", skippedCount);
      return;
    }
    Serial.println("✓ SD-kortti palasi! Täytetään skippatut jaksot nollilla.");
  }

  // Avataan tiedosto — suljetaan AINA lopuksi
  File f = SD.open(CSV_FILE, FILE_APPEND);
  if (!f) {
    Serial.println("VIRHE: CSV-tiedostoa ei voitu avata!");
    sdReady = false;
    setSdStatusLed(false);
    skippedCount++;
    return;
  }

  // Kirjoitetaan skippatut jaksot nollilla
  if (skippedCount > 0) {
    unsigned long jaksoSek = LOG_INTERVAL_MS / 1000UL;
    for (unsigned long i = skippedCount; i > 0; i--) {
      unsigned long skipTs = timestamp - (i * jaksoSek);
      kirjoitaRivi(f, skipTs, 0, 0, 0, false);
    }
    Serial.printf("Kirjoitettiin %lu nollarivit skipatuille jaksoille.\n", skippedCount);
    skippedCount = 0;
  }

  // Kirjoitetaan varsinainen rivi
  kirjoitaRivi(f, timestamp, T_ilma, T_maa, T_ulko, fanOn);
  f.close(); // suljetaan AINA

  confirmLoggedTimestamp(timestamp);

  Serial.printf("LOG %lu | ilma=%.2f maa=%.2f ulko=%.2f tuuletin=%s\n",
                timestamp, T_ilma, T_maa, T_ulko, fanOn ? "ON" : "OFF");
}

void addSkippedIntervals(unsigned long count) {
  skippedCount += count;
}


