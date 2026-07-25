#include "config.h"
#ifndef FAN_CONTROL_H
#define FAN_CONTROL_H

// ─────────────────────────────────────────────
//  fan_control.h
//  Tuuletinlogiikka + HTTP-ohjaus Shelly Plug S:lle
//  Fan logic + HTTP-controls for Shelly Plug S
//
//  Shelly Plug S IP in a AP-network is usually 192.168.4.2
//  (first device that connects to AP)
//  Check the IP from Shellys own controlpanel.
//
//  Logic (gets checked every 30 seconds):
//    T_ilma > T_maa + 2.0  → fan ON
//    T_ilma < T_maa - 4.0  → fan ON  (ground clearly warmer)
//    T_ilma ≈ T_maa         → fan OFF
//
//  HUOM: logiikka on helppo päivittää kirjallisuuden perusteella!!!
//  muuttamalla kynnysarvoja TEMP_DIFF_ON ja TEMP_DIFF_OFF_BELOW. 
// ─────────────────────────────────────────────

// Shelly Plug S -settings

#define SHELLY_PORT  80

// Temperature Tresholds (°C)
#define TEMP_DIFF_ON         2.0   // T_ilma > T_maa + tämä → ON
#define TEMP_DIFF_OFF_BELOW  4.0   // T_ilma < T_maa - tämä → ON

void initFanControl();
void updateFanControl(float T_ilma, float T_maa);
bool getFanState();   // true = ON, false = OFF

#endif
