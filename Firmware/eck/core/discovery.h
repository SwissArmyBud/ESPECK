
// MESSAGE GENERATOR/SENDER FOR DISCOVERY SERVICE
void broadcastAnnouncement(){
  // AUTO-DISCOVERY PROTOCOL
  if(OP_STATE == NORMAL){
    String msg = String(ESP.getChipId());
    Serial.print("  - Chip ID: ");
    Serial.println(msg);

    msg = getMD5(msg);
    msg = "ECK-AutoDiscovery:" + msg;
    Serial.print("  - A/D Msg: ");
    Serial.println(msg);

    IPAddress broadcastIP(255, 255, 255, 255);

    udpInterface.beginPacket( broadcastIP, UPD_ANNOUNCE_PORT );
    udpInterface.write(msg.c_str());
    udpInterface.endPacket();
  }
}

void startDiscovery(){
  Serial.println("  - nothing to do...");
}

void runDiscovery(){
  // ANNOUNCE TIMEOUT
  if (currentTime - broadcastTimer > UDP_ANNOUNCE_TIME){
    Serial.println("  --> ANNOUNCING PRESENCE");
    broadcastAnnouncement();
    broadcastTimer = currentTime;
  }
}
