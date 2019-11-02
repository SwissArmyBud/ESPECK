
  // CAPTIVE PORTAL - ALL ROUTES GO HERE
  httpsServer.onNotFound([]() {
    Serial.print("  Handling request - ");
    Serial.println(httpsServer.method());
    switch(httpsServer.method()){
      case HTTP_GET:{
        if (!serveFromFlash("/config.html")) {
          httpsServer.send(404, "text/plain", "Error finding configurator app!");
        }
        break;
      }
      case HTTP_POST:{
        // SPA IS SENDING CONFIG INFORMATION - TRY AND USE IT
        if(httpsServer.hasHeader("X-ESP-CONFIG-WIFI")){
          Serial.print("  --> CONFIG INCOMING: ");
          Serial.println(httpsServer.header("X-ESP-CONFIG-WIFI"));
          // TRY AND CONNECT TO PROVIDED CREDENTIALS AND RESTART IF SUCCESS
          processCredentials(httpsServer.header("X-ESP-CONFIG-WIFI"), true);
          httpsServer.send(200, "text/plain", "Thanks for the credentials!");
        } else {
          // SPA IS REQUESTING SCAN RESULTS - SEND THEM
          httpsServer.send(200, "text/json", scanResults);
        }
        break;
      }
      default:{
        httpsServer.send(404, "text/plain", "Error with request!");
      }
    }
  });
