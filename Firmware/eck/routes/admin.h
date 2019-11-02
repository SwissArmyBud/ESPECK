
  // ***************************************
  // ******* ACE.JS SUPPORT START **********
  // ***************************************
  // Send file list
  httpsServer.on(list_path, HTTP_GET, []() {
    if (!httpsServer.authenticate(admin_username, edit_password)) {
      return httpsServer.requestAuthentication();
    }
    fileList_callback();
  });
  // Send live editor
  httpsServer.on(edit_path, HTTP_GET, []() {
    if (!httpsServer.authenticate(admin_username, edit_password)) {
      return httpsServer.requestAuthentication();
    }
    else if (!serveFromFlash("/edit.html")) {
      httpsServer.send(404, "text/plain", "FileNotFound");
    }
  });

  // RESPOND TO [PUT/DELETE] AS [Create/Delete]
  // NOTE - SHOULD THESE BE AUTH PROTECTED?
  httpsServer.on(edit_path, HTTP_PUT, fileCreate_callback);
  httpsServer.on(edit_path, HTTP_DELETE, fileDelete_callback);
  // RESPOND TO [POST] AFTER INITIAL REQUEST, THEN ACCEPT FILE FOR UPLOAD
  httpsServer.on(edit_path, HTTP_POST, []() {
    httpsServer.send(200, "text/plain", "");
  }, fileUploadPartial_callback);
  // ***************************************
  // ********* ACE.JS SUPPORT END **********
  // ***************************************

  // RETURN SYSTEM STATUS IN JSON - SECURITY BUG LEAKS oneTimePass AS DEBUG
  httpsServer.on(status_path, HTTP_GET, []() {
    if (!httpsServer.authenticate(admin_username, status_password)) {
      return httpsServer.requestAuthentication();
    }
    String json = "{";
    json += "\"heap\":" + String(ESP.getFreeHeap());
    json += ", \"secret\":" + String(oneTimePass);
    json += ", \"version\":" + String(FIRMWARE_VERSION);
    json += ", \"tag\":" + String(VERSION_TAG);
    json += "}";
    httpsServer.send(200, "text/json", json);
  });

  // SERVE SPIFFS FILE OR RETURN [404/Not Found]
  httpsServer.onNotFound(sendFileOrNotFound_callback);
