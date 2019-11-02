
// ---------
// APPLIANCE
// ----------------------------------------------------------
#define BLINK_LED 0
#define ONE_SECOND_IN_MS 1000
long blinkPeriod = 250;

// ---------
// DISCOVERY
// ----------------------------------------------------------
#define UPD_ANNOUNCE_PORT 1920
#define UDP_ANNOUNCE_TIME 5000
unsigned long broadcastTimer = 0;
static WiFiUDP udpInterface;

// ---------
// DNS
// ----------------------------------------------------------
#define DNS_INIT_ATTEMPT_LIMIT 5
#define DNS_PORT_BINDING 53
static DNSServer dnsServer;

// ---------
// FILE SYSTEM
// ----------------------------------------------------------
static const char* ssidFile = "./ssid.cnf";
static const char* passFile = "./pass.cnf";
static const char* modeFile = "./sys.mode";

// ---------
// HTTP
// ----------------------------------------------------------
// SETUP BUFFER FOR FILE REPLY CHUNKING
#define RESPONSE_BUFFER_SIZE 1024
static unsigned char responseBuffer[RESPONSE_BUFFER_SIZE + 1] = {};
// GENERATED SSL KEYS, UTILITY INCLUDED
#include "509keys.h"
ESP8266WebServerSecure httpsServer(443);
// ROUTE PATHS AND AUTH STRINGS
static const char* list_path = "/list";
static const char* list_password = "list";
static const char* edit_path = "/edit";
static const char* edit_password = "edit";
static const char* status_path = "/status";
static const char* status_password = "status";
// FILE REFERENCE FOR SSL UPLOAD SERVICE
static File uploadFile;
// CATCH HEADERS FOR 304 CODE RETURNS
// NOTE - ALL HEADERS *MUST* BE ANNOUNCED HERE TO BE PARSED ON INCOMING REQUESTS
static const char * headerkeys[] = {
  "If-Modified-Since",
  "X-ESP-CONFIG-WIFI",
  "X-ESP-VALUE-TEST"
};

// ---------
// STATE
// ----------------------------------------------------------
// BASIC STATE MACHINE ENUM
enum State {
  UNDEFINED,
  ADMINISTRATION,
  NORMAL,
  CONFIGURATION
} OP_STATE = UNDEFINED;
static unsigned long currentTime = 0;
uint8_t modeInt[1] = {};

// ---------
// UPDATER
// ----------------------------------------------------------
static const char* update_path = "/update";
static const char* admin_username = "admin";
static char* oneTimePass = "123456"; // MUST be init to a different val!
static int16_t updateStatus = 0;
static UpdateServer httpsUpdater(true);

// ---------
// WIFI
// ----------------------------------------------------------
// LIMIT OF AP CONNECTION ATTEMPT BEFORE DROP BACK INTO CONFIG
#define CONNECTION_ATTEMPT_LIMIT 5
// MAX CHARACTERS OF SSID/PASSWORD LENGTH - LONGER VALUES WILL NOT WORK
// NOTE - This size seems a little small, maybe 64? Check std?
#define SETTINGS_BUFFER_SIZE 40
static char rwSSID[SETTINGS_BUFFER_SIZE + 1] = {};
static char rwPASS[SETTINGS_BUFFER_SIZE + 1] = {};
// DEFAULT CONFIGURATION NETWORK NAME/PASS
// NOTE - SSID is used as domain under DNS, must be domain legal (ie no spaces)
static const char* apSSID = "especk";
static const char* apPASS = "especkpass";
// DEFAULT NETWORK LAYOUT IS DEVICE AT 192.168.0.1 ON CLASS C 255.255.255.0
static IPAddress localAddress(192,168,0,1);
static IPAddress subnet(255,255,255,0);
// MIN LENGTH FOR ANY AND MAX LENGTH FOR SSID AND PATH
#define MIN_CONF_LENGTH_CAPTURE 3
// NOTE - These values may conflict with rwSSID and rwPASS, code check req'd
#define MAX_SSID_LENGTH_CAPTURE 25
#define MAX_PASS_LENGTH_CAPTURE 64
// SCAN RESULT LIMIT AND SCAN TIMEOUT/TRACKER
#define WIFI_SCAN_RESULT_LIMIT 6
#define AP_SCAN_PERIOD 10000
static unsigned long lastScanTime = 0;
// BIG ENOUGH FOR MAX_SSID_LENGTH_CAPTURE TIMES LENGTH PLUS "EXTRA"
static const size_t SCAN_RESULT_BUFFER_SIZE = 256;
static char scanResults[SCAN_RESULT_BUFFER_SIZE + 1] = {};
