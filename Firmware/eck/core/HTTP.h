

void startServer(){
  Serial.println("  - initializing HTTPS server...");

  // SET SSL KEYS INTO SERVER
  httpsServer.setServerKeyAndCert_P(rsakey, sizeof(rsakey), x509, sizeof(x509));

  //ask server to track headers
  httpsServer.collectHeaders(headerkeys, sizeof(headerkeys)/sizeof(char*) );

  // Start server
  httpsServer.begin();

  Serial.println("  - server started...");
}

void runServer(){

}

// SET UP SOME BASIC [MIME] PARSING
String getContentType(String filename) {
  if (httpsServer.hasArg("download")) {
    return "application/octet-stream";
  }
  else if ( filename.endsWith(".htm")  ) { return "text/html"; }
  else if ( filename.endsWith(".html") ) { return "text/html"; }
  else if ( filename.endsWith(".css")  ) { return "text/css"; }
  else if ( filename.endsWith(".js")   ) { return "application/javascript"; }
  else if ( filename.endsWith(".png")  ) { return "image/png"; }
  else if ( filename.endsWith(".gif")  ) { return "image/gif"; }
  else if ( filename.endsWith(".jpg")  ) { return "image/jpeg"; }
  else if ( filename.endsWith(".ico")  ) { return "image/x-icon"; }
  else if ( filename.endsWith(".xml")  ) { return "text/xml"; }
  else if ( filename.endsWith(".pdf")  ) { return "application/x-pdf"; }
  else if ( filename.endsWith(".zip")  ) { return "application/x-zip"; }
  else if ( filename.endsWith(".gz")   ) { return "application/x-gzip"; }
  return "text/plain";
}

// ATTEMPT TO SERVE A SPIFFS FILE TO THE CONNECTED CLIENT
bool serveFromFlash(String path) {
  // Send 304 if possible
  Serial.println("  Serve From Flash: " + path);
  if( httpsServer.header("If-Modified-Since").length() ){
    httpsServer.send(304, "text/plain", "");
    Serial.println("  --> sent 304 code...");
    return true;
  }
  // IF 304 NOT POSSIBLE, SEND PAGE AND ENCOURAGE CACHING
  else {
    if (path.endsWith("/")) {
      path += "index.html";
    }
    String contentType = getContentType(path);
    if (SPIFFS.exists(path)) {
      Serial.println("  --> file found");

      // PREPARE FOR READING FILE
      size_t responseSize = 0;
      File file = SPIFFS.open(path, "r");
      httpsServer.setContentLength(file.size());

      // INSTRUCT BROWSER TO BUILD CACHE
      httpsServer.sendHeader(F("Cache-Control"), F("public, max-age=600000"));
      httpsServer.sendHeader(F("age"), F("6000"));
      httpsServer.sendHeader(F("expires"), F("Wed, 30 Sep 2020 10:00:00 GMT"));
      httpsServer.sendHeader(F("last-modified"), F("Tue, 1 Oct 2019 11:00:00 GMT"));
      httpsServer.send(200,contentType,"");

      // SEND FILE CONTENT IF FILE EXISTS AND HAS CONTENT TO SEND
      if(file && file.size()){
        Serial.print("  --> sending chunks... ");
        // WHILE BYTES ARE AVAILABLE, COPY UP TO BUFFER_SIZE AND SERVE CHUNK
        while(file.available()){
          responseSize = file.read(responseBuffer, RESPONSE_BUFFER_SIZE);
          responseBuffer[responseSize] = 0;
          httpsServer.sendContent(reinterpret_cast<const char*>(responseBuffer));
        }
        Serial.println("done!");
        file.close();
        return true;
      }
    }
    // IF SPIFFS CANNOT LOAD PATH
    Serial.println("  --> cannot find file...");
    return false;
  }
}

// ********************************************************
// ----------------- ROUTES RUNNERS -----------------------
// ********************************************************

void startAdministrativeRoutes(){
#include "../routes/admin.h"
}

void startConfigurationRoutes(){
#include "../routes/config.h"
}

void startNormalRoutes(){
#include "../routes/normal.h"
}

void startRoutes(){
  switch (OP_STATE){
    case UNDEFINED:
    case CONFIGURATION:{
      Serial.println("  - starting CONFIG routes...");
      startConfigurationRoutes();
      break;
    }
    case ADMINISTRATION:{
      Serial.println("  - starting ADMIN routes...");
      startAdministrativeRoutes();
      break;
    }
    case NORMAL:{
      Serial.println("  - starting NORMAL routes...");
      startNormalRoutes();
      break;
    }
  }
}
