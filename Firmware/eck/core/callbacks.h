
// UPLOAD HANDLER FOR ACE.JS
void fileUploadPartial_callback() {
  Serial.println("  --> File Upload");
  if (httpsServer.uri() != edit_path) {
    return;
  }
  HTTPUpload& upload = httpsServer.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) {
      filename = "/" + filename;
    }
    Serial.print("  - name: ");
    Serial.println(filename);
    uploadFile = SPIFFS.open(filename, "w");
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (uploadFile) {
      uploadFile.write(upload.buf, upload.currentSize);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) {
      uploadFile.close();
    }
    Serial.print("  - size: ");
    Serial.println(upload.totalSize);
  }
}

// SEND NOT FOUND 404
void sendFileOrNotFound_callback() {
  if (!serveFromFlash(httpsServer.uri())) {
    httpsServer.send(404, "text/plain", "FileNotFound");
  }
}

// PARSE AND SENT FILE LIST TO CURRENT CLIENT
void fileList_callback() {
  Serial.println("  Getting list...");
  if (!httpsServer.hasArg("dir")) {
    httpsServer.send(500, "text/plain", "BAD ARGS");
    return;
  }

  String path = httpsServer.arg("dir");
  Serial.println("  --> path: " + path);
  Dir dir = SPIFFS.openDir(path);
  String output = "[";
  while (dir.next()) {
    File entry = dir.openFile("r");
    if (output != "[") {
      output += ',';
    }
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += String(entry.name()).substring(1);
    output += "\"}";
    entry.close();
  }

  output += "]";
  Serial.println("  --> list: " + output);
  httpsServer.send(200, "text/json", output);
}

// DELETE FILE FROM SPIFFS ON REQUEST OF ACE.JS
void fileDelete_callback() {
  if (httpsServer.args() == 0) {
    return httpsServer.send(500, "text/plain", "BAD ARGS");
  }
  String path = httpsServer.arg(0);
  Serial.println("File Delete: " + path);
  if (path == "/") {
    return httpsServer.send(500, "text/plain", "BAD PATH");
  }
  if (!SPIFFS.exists(path)) {
    return httpsServer.send(404, "text/plain", "FileNotFound");
  }
  SPIFFS.remove(path);
  httpsServer.send(200, "text/plain", "");
}

// HANDLE CREATE NEW FILE IN SPIFFS FOR ACE.JS
void fileCreate_callback() {
  if (httpsServer.args() == 0) {
    return httpsServer.send(500, "text/plain", "BAD ARGS");
  }
  String path = httpsServer.arg(0);
  Serial.println("File Create: " + path);
  if (path == "/") {
    return httpsServer.send(500, "text/plain", "BAD PATH");
  }
  if (SPIFFS.exists(path)) {
    return httpsServer.send(500, "text/plain", "FILE EXISTS");
  }
  File file = SPIFFS.open(path, "w");
  if (file) {
    file.close();
  } else {
    return httpsServer.send(500, "text/plain", "CREATE FAILED");
  }
  httpsServer.send(200, "text/plain", "");
}
