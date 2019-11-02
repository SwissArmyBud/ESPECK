// ATTEMPT TO LOAD SSID AND PASS BUFFERS FROM SPIFFS FILES
bool loadWifiSettings(){
  Serial.println("  - trying to load settings...");
  if (SPIFFS.exists("/ssid.cnf") && SPIFFS.exists("/pass.cnf")) {

    size_t readSize = 0;
    Serial.println("  - loading SSID from SPIFFS...");
    File file = SPIFFS.open(ssidFile, "r");
    // IF file exists, is large enough, and small enough
    if(file && ( (file.size() >= MIN_CONF_LENGTH_CAPTURE) && (file.size() <= MAX_SSID_LENGTH_CAPTURE) ) ){
      while(file.available()){
        readSize = file.readBytes(rwSSID, MAX_SSID_LENGTH_CAPTURE);
        rwSSID[readSize] = 0;
      }
      file.close();
    } else {
      Serial.println("  - file issue...");
      return false;
    }

    Serial.println("  - loading PASS from SPIFFS...");
    file = SPIFFS.open(passFile, "r");
    // IF file exists, is large enough, and small enough
    if(file && ( (file.size() >= MIN_CONF_LENGTH_CAPTURE) && (file.size() <= MAX_PASS_LENGTH_CAPTURE) ) ){
      readSize = 0;
      while(file.available()){
        readSize = file.readBytes(rwPASS, MAX_PASS_LENGTH_CAPTURE);
        rwPASS[readSize] = 0;
      }
      file.close();
      return true;
    } else {
      Serial.println("  - file issue...");
      return false;
    }
  }
  Serial.println("  - not found!");
  return false;
}


// USE AP CONFIG VALUES TO CREATE A LOCAL WIFI NETWORK
bool createAccessPoint(){
  Serial.println("  - setting up WiFi in AP MODE...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSSID, apPASS);
  WiFi.softAPConfig(localAddress, localAddress, subnet);
  Serial.print("  - Local IP: ");
  Serial.println(WiFi.softAPIP());
}

// USE STA CONFIG VALUES TO CONNECT TO LOCAL WIFI NETWORK
void connectStation(){
  Serial.println("  - setting up WiFi in STA MODE...");
  // Track attempts to connect to local wi-fi
  uint8_t attempts = 0;

  // Set SoftDevice to STATION mode
  WiFi.mode(WIFI_STA);

  // SET USER AND PASS FROM SETTINGS AND ATTEMPT TO CONNECT UNTIL LIMIT REACHED
  while ( (WiFi.waitForConnectResult() != WL_CONNECTED) && (attempts++ < CONNECTION_ATTEMPT_LIMIT) ) {
    // DON'T CHANGE VALUE AFTER WI-FI CONNECTION
    WiFi.begin(reinterpret_cast<const char*>(rwSSID), reinterpret_cast<const char*>(rwPASS));
    if(WiFi.waitForConnectResult() != WL_CONNECTED){
      Serial.println("  - failed, retrying...");
    }
  }

}

// CONNECT TO EXISTING NETWORK IF CONFIG LOADED OR START LOCAL NETWORK
void startWiFi(){
  Serial.println("  - starting Wi-Fi stack...");

  Serial.println("  - clearing AP cache...");
  WiFi.disconnect();

  // ATTEMPT TO LOAD WIFI SETTINGS FROM SPIFFS, IF WE AREN'T CONFIGURING/ADMINISTERING
  if( ((OP_STATE == UNDEFINED) || (OP_STATE == NORMAL)) && loadWifiSettings() ){

    OP_STATE = NORMAL;

    Serial.println("  - connecting to local WiFi...");
    connectStation();

    // CATCH GOOD CONNECTION AND CONTINUE WITH BOOT
    if(WiFi.waitForConnectResult() == WL_CONNECTED){
      Serial.print("  - Local IP: ");
      Serial.println(WiFi.localIP());
      return;
    }

    // FAILED TO CONNECT - NOW WE ARE CONFIGURING
    Serial.println("  - failed, no more retries!");
    OP_STATE = CONFIGURATION;
  }

  // ADMINISTRATION AND CONFIGURATION
  // ALSO, FALLBACK TO SELF-AP MODE
  createAccessPoint();

}


// TAKE CREDENTIALS (FROM HEADER CURRENTLY) AND PROCESS INTO WIFI LOGIN CREDS
void processCredentials(String credentials, bool restartOnDone){

    // TRACK HOW MUCH STRING LENGTH TO COPY
    size_t copySize = 0;

    // GRAB TOKEN 0 FROM DELIMITED STRING
    String ssid = getValue(credentials, ':', 0);
    // COPY THE RESULT STRING TO A STATIC BUFFER
    copySize = MAX_SSID_LENGTH_CAPTURE > ssid.length() ? ssid.length() : MAX_SSID_LENGTH_CAPTURE;
    strncpy( rwSSID, ssid.c_str(), copySize );
    rwSSID[copySize] = 0;

    // GRAB TOKEN 1 FROM DELIMITED STRING
    String pass = getValue(credentials, ':', 1);
    // COPY THE RESULT STRING TO A STATIC BUFFER
    copySize = MAX_PASS_LENGTH_CAPTURE > pass.length() ? pass.length() : MAX_PASS_LENGTH_CAPTURE;
    strncpy( rwPASS, pass.c_str(), copySize );
    rwPASS[copySize] = 0;

    // ATTEMPT TO CONNECT TO LOCAL WIFI USING NEW CREDENTIALS
    Serial.println("  - connecting to local WiFi...");
    Serial.print("  - ");
    Serial.print(rwSSID);
    Serial.print(" / ");
    Serial.println(rwPASS);
    connectStation();

    // CATCH GOOD CONNECTION AND CONTINUE WITH SAVE/RELOAD
    if(WiFi.waitForConnectResult() == WL_CONNECTED){
      Serial.println("  - credentials accepted for processing...");

      // PREPARE FOR SAVING CREDENTIALS
      size_t writeSize = 0;
      File ssidConfFile;
      File passConfFile;

      ssidConfFile = SPIFFS.open(ssidFile, "w");
      passConfFile = SPIFFS.open(passFile, "w");

      if( (ssidConfFile && passConfFile) ){
        // SET TO CONFIGURATION MODE AS FALLBACK
        OP_STATE = CONFIGURATION;

        writeSize = ssidConfFile.write((uint8_t*)rwSSID, strlen(rwSSID));
        if(writeSize > MIN_CONF_LENGTH_CAPTURE){

          writeSize = passConfFile.write((uint8_t*)rwPASS, strlen(rwPASS));
          if(writeSize > MIN_CONF_LENGTH_CAPTURE){

            Serial.println("  - saved credentials to SPIFFS...");
            // SUCCESS, TRY TO ACT NORMAL
            OP_STATE = NORMAL;

          }
        }
        Serial.println("  - finished trying to save credentials...");
        // SAVE CURRENT STATE AFTER SAVE CONFIG ATTEMPT
        saveSystemMode(false);
      } else {
        Serial.println("  - couldn't open files to save credentials...");
      }
    } else {
      Serial.println("  - not connected...");
    }

    if(restartOnDone){
      ESP.restart();
    }
}

// ASYNC RESULT CHECKER AFTER wifi.scanNetworks() called
void handleWifiScanResults(){
  // ASYNC CHECK STACK FOR FINISHED SCAN
  int n = WiFi.scanComplete();
  // IF SCAN IS FINISHED WITH RESULTS
  if(n >= 0){
    Serial.println("\n  --> ASYNC SCAN RESULTS");
    Serial.printf("  found %i network(s)...\n", n);
    if(n > WIFI_SCAN_RESULT_LIMIT){
      n = WIFI_SCAN_RESULT_LIMIT;
    }
    // OPEN UP A JSON ARRAY FOR THE RESULTS OF THE SCAN
    String result = ("{\"networks\":[");
    for (int i = 0; i < n; i++){
      Serial.print("  network - ");
      Serial.println(WiFi.SSID(i));
      // MAINTAIN AT LEAST AN SSID PLUS 20 CHARACTERS IN THE BUFFER FOR CLOSING ETC
      // NOTE - This may be an arbitrary number, vs 1 for std null close
      if( SCAN_RESULT_BUFFER_SIZE > (result.length() + (MAX_SSID_LENGTH_CAPTURE + 20)) && ( WiFi.SSID(i).length() < MAX_SSID_LENGTH_CAPTURE )){
        // ADD A COMMA IF WE AREN'T FIRST
        result += ((i > 0) ? "," : "");
        // PUSH A JSON OBJECT ONTO THE STRING
        result += "{\"id\":\"";
        result += WiFi.SSID(i).substring(0, MAX_SSID_LENGTH_CAPTURE > WiFi.SSID(i).length() ? WiFi.SSID(i).length() : MAX_SSID_LENGTH_CAPTURE);
        result += "\",\"sec\":\"";
        result += WiFi.encryptionType(i) == ENC_TYPE_NONE ? "N":"Y";
        result += "\"}";
      }
    }
    // CLOSE THE JSON ARRAY
    result += ("]}");
    // COPY THE RESULT STRING TO A STATIC BUFFER
    strncpy( scanResults, result.c_str(), SCAN_RESULT_BUFFER_SIZE );
    scanResults[result.length()] = 0;
    // DELETE THE SCAN RESULTS FROM THE NETWORK STACK
    WiFi.scanDelete();
  }
}

void runWifi(){
  if(OP_STATE == CONFIGURATION){
    // ASYNC SCAN TRIGGER AND HANDLE
    if (currentTime - lastScanTime > AP_SCAN_PERIOD){
      WiFi.scanNetworks(true);
      Serial.println("\n  --> ASYNC SCAN START");
      lastScanTime = currentTime;
    }
    handleWifiScanResults();
  }
}
