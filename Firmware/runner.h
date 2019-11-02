
void setup() {

  Serial.begin(115200);
  Serial.println();
  Serial.println();

  Serial.println("  -------");
  Serial.println("  --> SYSTEM LOAD STARTING");
  Serial.println("  --> FIRMWARE " + String(FIRMWARE_VERSION));
  Serial.println("  --> SYSTEM VERSION TAG " + String(VERSION_TAG));
  Serial.println("  -------");
  Serial.println();
  Serial.println();

  Serial.println();
  Serial.println("  -------");
  Serial.println("  --> STARTING FILE SYSTEM");
  startFileSystem();

  Serial.println();
  Serial.println("  -------");
  Serial.println("  --> LOADING STATE MACHINE");
  startStateMachine();

  Serial.println();
  Serial.println("  -------");
  Serial.println("  --> STARTING WIRELESS STACK");
  startWiFi();

  Serial.println();
  Serial.println("  -------");
  Serial.println("  --> STARTING WEB SERVER");
  startServer();

  Serial.println();
  Serial.println("  -------");
  Serial.println("  --> STARTING WEB ROUTES");
  startRoutes();

  Serial.println();
  Serial.println("  -------");
  Serial.println("  --> STARTING FIRMWARE UPDATER");
  startUpdater();

  Serial.println();
  Serial.println("  -------");
  Serial.println("  --> STARTING DNS SYSTEM");
  startDNS();

  Serial.println();
  Serial.println("  -------");
  Serial.println("  --> STARTING DISCOVERY SYSTEM");
  startDiscovery();

  Serial.println();
  Serial.println("  -------");
  Serial.println("  --> STARTING APPLIANCE");
  applianceSetup();

  Serial.println();
  Serial.println();
  Serial.println("  -------");
  Serial.println("  --> SYSTEM LOAD FINISHED");
  Serial.println("  -------");
  Serial.println();
  Serial.println();

}

// ********************************************************
// ---------------- PROGRAM LOOP --------------------------
// ********************************************************

void loop() {

  // SET CLOCK TIME FOR STATE SLICE
  currentTime = millis();

  // HANDLE APPLIANCE-SPECIFIC TASKS
  applianceLoop(); // MAKE SURE THIS DOESN'T TAKE FOREVER

  // HANDLE ECK-SPECIFIC TASKS
  runStateMachine(); // MAKE SURE THIS DOESN'T TAKE FOREVER
  runServer(); // RUN HTTP/S SERVICING - WILL TAKE FOREVER
}
