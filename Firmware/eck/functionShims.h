
// ---------
// APPLIANCE
// ----------------------------------------------------------
void applianceSetup();
void applianceLoop();

// ---------
// DISCOVERY
// ----------------------------------------------------------
void startDiscovery();
void runDiscovery();

// ---------
// DNS
// ----------------------------------------------------------
void startDNS();
void runDNS();

// ---------
// FILE SYSTEM
// ----------------------------------------------------------
void startFileSystem();
void loadSystemMode(bool);
void saveSystemMode(bool);
void generateNewOneTimePass();

// ---------
// HTTP
// ----------------------------------------------------------
bool serveFromFlash(String);
// --> HTTP CALLBACKS
void fileUpload_callback();
void fileDelete_callback();
void fileCreate_callback();
void fileList_callback();
void fileUploadPartial_callback();
void sendFileOrNotFound_callback();

// ---------
// STATE
// ----------------------------------------------------------

// ---------
// UPDATER
// ----------------------------------------------------------
void startUpdater();
void updateProgress_callback(int16_t);

// ---------
// WIFI
// ----------------------------------------------------------
void handleWifiScanResults();
void processCredentials(String, bool);
void runWifi();
