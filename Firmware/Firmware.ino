
// ********************************************************
// --------------- FIRMWARE TAG/VER -----------------------
// ********************************************************
static const char* FIRMWARE_VERSION = "0.5b";
static const char* VERSION_TAG = "OLYMPUS";

// ********************************************************
// --------------- INCLUDE FRAMEWORK ----------------------
// ********************************************************
// LIBRARY IMPORTS - INCLUDING FOR APPLIANCE
#include "libs.h"
// ECK-SPECIFIC SCAFFOLDING AND SETUP
#include "eck.h"
// RUNNER HANDLES SETUP/LOOP FOR BOTH ECK AND APPLIANCE
#include "runner.h"

// ********************************************************
// -------------- APPLIANCE SETUP -------------------------
// ********************************************************
void applianceSetup(){

  // SET BLINK LED TO OUTPUT FOR HEARTBEAT
  Serial.println("  starting heartbeat LED...");
  pinMode(BLINK_LED, OUTPUT);

}

// ********************************************************
// ---------------- APPLIANCE LOOP ------------------------
// ********************************************************
void applianceLoop(){

  // BLINK THE LED
  digitalWrite(BLINK_LED, (currentTime % ONE_SECOND_IN_MS) < blinkPeriod );

}
