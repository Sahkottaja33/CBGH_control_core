#include "fan_control.h"
#include <WiFi.h>
#include <HTTPClient.h>

// ─────────────────────────────────────────────
//  fan_control.cpp
//Controls a Shelly Plug S with a HTTP GET- request 
// ─────────────────────────────────────────────

static bool fanState = false; // current state

// Sends an HTTP-request to Shelly plug
static void shellySend(bool turnOn) {
  HTTPClient http;
  String url = "http://";
  url += SHELLY_IP;
  url += "/rpc/Switch.Set?id=0&on=";
  url += turnOn ? "true" : "false";

  Serial.print("Shelly:");
  Serial.print(url);

  http.begin(url);
  http.setTimeout(5000);
  int httpCode = http.GET();
  http.end();

  if (httpCode == 200) {
    Serial.print("Shelly: fan ");
    Serial.println(turnOn ? "ON" : "OFF");
  } else {
    Serial.print("Shelly HTTP-error: ");
    Serial.println(httpCode);
    fanState = !turnOn;
  }
}

void initFanControl() {
  fanState = false;
  shellySend(false); // Ensures that the fan is off when starting
  Serial.println("Tuuletinhallinta alustettu.");
}

void updateFanControl(float T_ilma, float T_maa) {
  // Checks for temperature sensor errors
  if (T_ilma < -100.0 || T_maa < -100.0) {
    Serial.println("Warning: False sensor value, fan logic will be overwritten"); 
    return;
  }

  float diff = T_ilma - T_maa; //Positive = air is warmer than the ground 

  bool shouldBeOn = false;

  if (diff > TEMP_DIFF_ON) {
    //  AThe air is clearly warmer than the ground → ventilation transfers heat into the soil.
    shouldBeOn = true;
    Serial.printf("Tuuletin päälle: T_ilma(%.1f) > T_maa(%.1f) + %.1f\n",
                  T_ilma, T_maa, TEMP_DIFF_ON);
  } else if (diff < -TEMP_DIFF_OFF_BELOW) {
    // “The soil is clearly warmer than the air → ventilation prevents heat loss.
    // HUOM: koodiapina tarkista kirjallisuudesta logiikka!!!!!
    shouldBeOn = true;
    Serial.printf("Tuuletin päälle: T_maa(%.1f) > T_ilma(%.1f) + %.1f\n",
                  T_maa, T_ilma, TEMP_DIFF_OFF_BELOW);
  } else {
    // Teperatures are nearing eachother → no ventilation
    shouldBeOn = false;
    Serial.printf("Tuuletin pois: ero %.1f°C (kynnys +/-%.1f/%.1f)\n",
                  diff, TEMP_DIFF_ON, TEMP_DIFF_OFF_BELOW);
  }

  // Sends a command only if the state has changed (No useless HTTP-requests)
  if (shouldBeOn != fanState) {
    fanState = shouldBeOn;
    shellySend(fanState);
  }
}

bool getFanState() {
  return fanState;
}
