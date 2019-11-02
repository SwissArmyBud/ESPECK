

// DNS SERVER FOR LOCAL WIFI NETWORK CREATION
void startDNS(){
  if( (OP_STATE==CONFIGURATION) || (OP_STATE==ADMINISTRATION)){
    // Track attempts to start DNS service
    uint8_t attempts = 0;
    bool started = false;
    String domain = "www.";
    domain += apSSID;
    domain += ".com";
    while ( !started && (attempts++ < DNS_INIT_ATTEMPT_LIMIT) ) {
      Serial.println("  - attempting to start DNS responder...");
      started = dnsServer.start(DNS_PORT_BINDING, domain, WiFi.softAPIP());
      if(!started){
        delay(500);
      }
    }
    if(started){
      dnsServer.setTTL(300);
      dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
      Serial.println("  - started...");
    } else {
      Serial.println("  - failed, no more retries...");
    }
  } else {
    Serial.println("  - skipping, wrong state...");
  }
}

void runDNS(){
  dnsServer.processNextRequest();
}
