
void startUpdater(){

  if(OP_STATE == ADMINISTRATION){
    Serial.println("  - starting Update Service...");

    // INIT NEW OTP BEFORE INIT UPDATER
    generateNewOneTimePass();
    // SET [GET/POST] PATH AND AUTH VALUES
    httpsUpdater.setup(&httpsServer, update_path, admin_username, oneTimePass);

    // Add function handlers for update service
    httpsUpdater.setRequestFnHandler([&](){
      if (!serveFromFlash("/update.html")) {
        httpsServer.send(404, "text/plain", "Can't find update application!");
      }
    });
    httpsUpdater.setProgressFnHandler(updateProgress_callback);
  }

}

// GENERATES AND DOES **NOT** IMPLEMENT A NEW ONE-TIME-PASS
void generateNewOneTimePass(){
  MD5Builder md5;
  md5.begin();
  md5.add( oneTimePass );
  md5.add( String(ESP.getCycleCount()) );
  delay(2);
  md5.add( String(ESP.getCycleCount()) );
  md5.calculate();
  strncpy( oneTimePass, md5.toString().c_str(), strlen(oneTimePass) );
  // Note - SECURITY LEAK OF oneTimePass OVER SERIAL PORT
  Serial.print("  **** New One-Time-Pass ****\n  - ");
  Serial.println(oneTimePass);
}

// HANDLE UPDATE PROGRESS FROM UPDATE SERVER
void updateProgress_callback(int16_t progress){
  // VALUES OVER 0 INDICATE POSITIVE PROGRESS
  if (progress > 0) {
    updateStatus += progress/1024;
    Serial.print("  --> update progress:");
    Serial.println(updateStatus);
  } else if (progress == 0){ // UpdateServer SENDS 0 ON FINISH/SUCCESS UPDATE
    Serial.println("Update was success!");
    httpsServer.client().setNoDelay(true);
    // INFORM OF SUCCESS
    if (!serveFromFlash("/updateOk.html")) {
      // Send success page or basic refresh
      httpsServer.send(200, PSTR("text/html"), "<META http-equiv=\"refresh\" content=\"10;URL=/\">Update Success! Rebooting...\n");
    }
    // GIVE THE SOFTDEVICE A CHANCE TO EMPTY TX BUFFERS
    delay(1000);
    httpsServer.client().stop();
    // WILL EFFECTIVELY FORCE A NEW ONE-TIME-PASS
    delay(1000);
    ESP.restart();
  } else if (progress < 0) { // UpdateServer SENDS (-) ERROR CODE ON FAILURE
    Serial.println("Update was failure!");
    // PROBLEM WITH UDPATE - GEN NEW ONE-TIME-PASS AND INFORM USER OF FAIL
    generateNewOneTimePass();
    httpsUpdater.setOneTimePass(oneTimePass);
    if (!serveFromFlash("/updateEr.html")) {
      // Send error page or basic refresh
      httpsServer.send(200, F("text/html"), "<META http-equiv=\"refresh\" content=\"10;URL=/\">Error " + String(progress) + "! Rebooting...\n");
    }
  }
}
