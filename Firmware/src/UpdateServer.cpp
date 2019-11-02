#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include "StreamString.h"
#include "UpdateServer.h"

UpdateServer::UpdateServer(bool serial_debug)
{
  _serial_output = serial_debug;
  _server = NULL;
  _username = NULL;
  _password = NULL;
  _authenticated = false;
  _pageRequestCallback = NULL;
  _updateProgressCallback = NULL;
}

void UpdateServer::setup(ESP8266WebServer *server, const char * route, const char * username, const char * password)
{
    _server = server;
    _username = (char *)username;
    _password = (char *)password;

    // handler for the /update form page
    _server->on(route, HTTP_GET, [&](){
      if(_username != NULL && _password != NULL && !_server->authenticate(_username, _password)){
        return _server->requestAuthentication();
      }
      if(_pageRequestCallback){
        _pageRequestCallback();
      }
    });

    // HANDLE /update [POST]
    _server->on(route, HTTP_POST, [&](){
      // HANDLER FOR FILE UPLOAD FINISHED
      // --------------------------------
      if(!_authenticated)
        return _server->requestAuthentication();
      if (_updateProgressCallback){
         Serial.println("-- CODING OUT --");
         Serial.print("  --> ");
         Serial.println(Update.getError());
        _updateProgressCallback(Update.getError());
      }
    },[&](){
      // HANDLER FOR FILE UPLOAD PROGRESS
      // --------------------------------

      // Attach to the server upload object
      HTTPUpload& upload = _server->upload();

      if(upload.status == UPLOAD_FILE_START){
        // IF UPLOAD NEEDS TO START

        _updaterError = String();
        Serial.setDebugOutput(true); // NOTE - Do we need serial debug active?

        // DOUBLE CHECK AUTH
        _authenticated = (_username == NULL || _password == NULL || _server->authenticate(_username, _password));
        if(!_authenticated){
          if (_serial_output)
            Serial.println("-- BAD AUTH ON UPDATE --");
          return;
        }

        // KILL UDP
        WiFiUDP::stopAll();
        // GET MAX SPACE
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if (_serial_output){
          Serial.printf("Update: %s\n", upload.filename.c_str());
          Serial.printf("Size: %u\n", upload.totalSize);
          Serial.printf("Available: %i\n", maxSketchSpace);
        }
        // TRY AND INIT THE UPDATE
        if(!Update.begin(maxSketchSpace)){
          _setUpdaterError();
        }

      } else if(_authenticated && upload.status == UPLOAD_FILE_WRITE && !_updaterError.length()){
        // IF UPLOAD IS IN PROGRESS

        // UPDATE PROGRESS CALLBACK
        if(_updateProgressCallback){
          _updateProgressCallback(upload.currentSize);
        }
        // TRY WRITING CHUNCK TO UPDATE
        if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
          _setUpdaterError();
        }

      } else if(_authenticated && upload.status == UPLOAD_FILE_END && !_updaterError.length()){
        // IF UPLOAD IS DONE

        // IF UPDATE IS OK, CALLBACK
        // IF UPDATE IS NOT OK THEN UPDATE ERROR
        if(Update.end(true)){ //true to set the size to the current progress
          if (_serial_output) {
            Serial.println("-- UPDATE SUCCESS --");
          }
          if(_updateProgressCallback){
            _updateProgressCallback(0); // TRIGGER REBOOT
          }
        } else {
          _setUpdaterError();
        }

      } else if(_authenticated && upload.status == UPLOAD_FILE_ABORTED){
        // IF UPLOAD FAILS

        // END THE UPDATE ATTEMPT
        Update.end();
        if (_serial_output){
          Serial.println("-- UPDATE STOPPED --");
        }
      }
      delay(0); // NOTE - Why is this delay here?
    });
}

void UpdateServer::_setUpdaterError()
{
  Update.printError(Serial);
  StreamString str;
  Update.printError(str);
  _updaterError = str.c_str();
}

void UpdateServer::setRequestFnHandler(void(*fn)())
{
  _pageRequestCallback = fn;
}
void UpdateServer::setProgressFnHandler(void(*fn)(int16_t))
{
  _updateProgressCallback = fn;
}

void UpdateServer::setOneTimePass(char * pass){
  _password = pass;
}
