
// INIT THE SPIFFS FILE SYSTEM, OPEN ROOT AND PRINT CONTENTS
// Note - The printing could be a SECURITY LEAK and should be used as DEBUG only
void startFileSystem(){
  Serial.println("  opening SPIFFS on FLASH...");
  SPIFFS.begin();
  // **************
  // DEBUG
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("  - file -> %s , size - %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
  }
  // DEBUG
  // **************
}

// ATTEMPT TO LOAD AN [OP_STATE] FROM SPIFFS FILE AND RESTART IF NEEDED
void loadSystemMode(bool restartOnLoad){
  if (SPIFFS.exists(modeFile)) {
    Serial.println("  - mode file found");
    File file = SPIFFS.open(modeFile, "r");
    if(file && file.size()){
      file.read(modeInt, 1);
      OP_STATE = static_cast<State>(modeInt[0]);
      // NOTE - Let's corral the value at some point, here?
      Serial.printf("  - loaded mode %i...\n", OP_STATE);
      if(restartOnLoad){
        ESP.restart();
      }
      return;
    }
  }
  Serial.println("  - mode CONFIGURATION fallback");
  OP_STATE = CONFIGURATION;
}

// ATTEMPT TO LOAD THE CURRENT [OP_STATE] AND RESTART IF NEEDED
void saveSystemMode(bool restartOnSave){
  Serial.printf("  Moving to mode %i...\n", OP_STATE);
  File file = SPIFFS.open(modeFile, "w");
  modeInt[0] = OP_STATE;
  if(file.write(modeInt,1)){
    Serial.println("  Mode saved!");
  }
  file.close();
  if(restartOnSave){
    ESP.restart();
  }
}
