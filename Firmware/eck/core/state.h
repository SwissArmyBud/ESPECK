
void startStateMachine(){
  loadSystemMode(false);
  Serial.printf("  - mode --> %i\n", OP_STATE);
}

void runStateMachine(){

  // RUN THE BACK-END SERVICE CALLER (RUN EACH STATE IN SERVICE)
  switch (OP_STATE) {
    case UNDEFINED : {
      if(true){
        Serial.println("Exiting UNDEFINED to CONFIG...");
        OP_STATE = CONFIGURATION;
        saveSystemMode(true);
      }
      break;
    }
    case NORMAL : {

      // RUN AUTO-DISCOVERY PROTOCOL
      runDiscovery();

      break;
    }
    case CONFIGURATION : {

      // ANNOUNCE AS NEEDED
      runDiscovery();
      // HANDLE DNS REQUESTS
      runDNS();
      // TRIGGER AP SCAN
      runWifi();

      break;
    }
    case ADMINISTRATION : {

      // ANNOUNCE AS NEEDED
      runDiscovery();
      // HANDLE DNS REQUESTS
      runDNS();

      break;
    }
    default : {
      // ALWAYS SET STATE OR YOU'RE GOING TO HAVE A BAD TIME
      Serial.println("OOPS! Don't forget to set your state!");
      OP_STATE = CONFIGURATION;
      saveSystemMode(true); // RESTART
      break;
    }
  }

}
