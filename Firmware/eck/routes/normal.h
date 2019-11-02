
  // CAPTIVE PORTAL - ALL ROUTES GO HERE
  httpsServer.onNotFound([]() {
    switch(httpsServer.method()){
      // GET SERVES THE PAGE
      case HTTP_GET:{
        if (!serveFromFlash("/home.html")) {
          httpsServer.send(404, "text/plain", "Home Application Not Found!");
        }
        break;
      }
      // EXAMPLE
      // POST HANDLES INCOMING HEADERS
      case HTTP_POST:{
        // POST TIME REQUEST
        if(httpsServer.hasHeader("X-ESP-VALUE-TEST")){

          // PRINT REQUEST VALUE
          Serial.print("  --> REQUEST INCOMING: ");
          Serial.println(httpsServer.header("X-ESP-VALUE-TEST"));

          // ******************
          // EXAMPLE - SIMPLE RESPOND TO CLIENT
          // ******************
          // httpsServer.send(200, "text/plain", "Got header ok, thanks!");

          // ******************
          // EXAMPLE - UPDATE APPLIANCE PARAMETER FROM INCOMING HEADER
          // ******************
          long headerVal = httpsServer.header("X-ESP-VALUE-TEST").toInt();
          if(headerVal > 0){
            headerVal = headerVal > 950 ? 950 : headerVal < 50 ? 50 : headerVal;
            blinkPeriod = headerVal;
            httpsServer.send(201, "text/plain", "Set blink ok, thanks!");
          } else {
            httpsServer.send(501, "text/plain", "Got packet, but bad value!");
          }

          // ******************
          // EXAMPLE - FORM A WEB REQUEST AND REACT TO RESPONSE FROM SERVER
          // ******************
          // // SETUP DOMAIN INFORMATION
          // char* host = "some.webdomain.com";
          // int httpsPort = 443;
          // char* apiEndpoint = "/api";
          // // START BUILDING REQUEST
          // WiFiClientSecure postRequest;
          // if (!postRequest.connect(host, httpsPort)) {
          //   Serial.println("  - connection failed");
          //   httpsServer.send(404, "text/plain", "Unable to connect...!");
          // } else {
          //   Serial.println("  - making payload");
          //   // PAYLOAD BUILDING
          //   String payload = "time=";
          //   payload += httpsServer.header("X-ESP-VALUE-TEST");
          //   payload += "&cpuID=";
          //   payload += String(ESP.getChipId());
          //   Serial.print("  - ");
          //   Serial.println(payload);
          //   // FORM REQUEST, INJECT PAYLOAD, SEND
          //   postRequest.print(String("POST ") + apiEndpoint + " HTTP/1.1\r\n" +
          //     "Host: " + host + "\r\n" +
          //     "Connection: close\r\n" +
          //     "Accept: */*\r\n" +
          //     "User-Agent: Mozilla/4.0 (compatible; esp8266 Lua; Windows NT 5.1)\r\n" +
          //     "Content-Type: application/x-www-form-urlencoded\r\n" +
          //     "Content-Length: "+ payload.length() +"\r\n" +
          //     "\r\n" + payload + "\r\n");
          //   Serial.println("  - request sent");
          //   // READ RESPONSE WHILE SERVER IS TALKING BACK
          //   while (postRequest.connected()) {
          //     String line = postRequest.readStringUntil('\n');
          //     Serial.print("  - headers received --> ");
          //     Serial.println(line);
          //     if (line == "\r") {
          //       Serial.println("  - headers finished!");
          //       break;
          //     }
          //   }
          //   String line = postRequest.readStringUntil('\n');
          //   // NOW WE HAVE RESPONSE VALUE - EVALUATE AND RESPOND
          //   if ( getValue(line, ':', 1).equals(httpsServer.header("X-ESP-VALUE-TEST")) ) {
          //     Serial.println("  --> MATCHING REQUEST/RESPONSE VALUES");
          //     Serial.print(httpsServer.header("X-ESP-VALUE-TEST"));
          //     Serial.println();
          //     // PASS RESULTS OF REQUEST TO CLIENT JS
          //     httpsServer.send(201, "text/plain", line);
          //   } else {
          //     Serial.println("  --> MISMATCHING REQUEST/RESPONSE VALUES - ERROR");
          //     Serial.print("  - ");
          //     Serial.println(line);
          //     // PASS RESULTS OF REQUEST TO CLIENT JS
          //     httpsServer.send(501, "text/plain", line);
          //   }
          // }
        } else {
          httpsServer.send(404, "text/plain", "Missing request headers...!");
        }
        break;
      }
      default:{
        httpsServer.send(404, "text/plain", "Error with request!");
      }
    }
  });
