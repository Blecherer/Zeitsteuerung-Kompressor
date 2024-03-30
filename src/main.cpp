#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>


// variables for measured values
int NumberClients;

// put function declarations here:
int myFunction(int, int);

// create web server
ESP8266WebServer webServer(80);

void handleRoot() {
  // digitalWrite(led, 1);
  webServer.send(200, "text/plain", "hello from esp8266!");
  // digitalWrite(led, 0);
}

void handleNotFound(){
  // digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += webServer.uri();
  message += "\nMethod: ";
  message += (webServer.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += webServer.args();
  message += "\n";
  for (uint8_t i=0; i<webServer.args(); i++){
    message += " " + webServer.argName(i) + ": " + webServer.arg(i) + "\n";
  }
  webServer.send(404, "text/plain", message);
  // digitalWrite(led, 0);
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  Serial.print("Konfiguriere soft-AP ... ");
  boolean result = WiFi.softAP("WemosAP", "geheim123");
  
  Serial.print("Verbindung wurde ");
  if(result == false){
    Serial.println("NICHT ");
  }
  Serial.print("erfolgreich aufgebaut!");

  webServer.on("/", handleRoot);

  webServer.on("/inline", [](){
    webServer.send(200, "text/plain", "this works as well");
  });

  webServer.onNotFound(handleNotFound);

  webServer.begin();
  Serial.println("HTTP server started");

}

void loop() {
  NumberClients = WiFi.softAPgetStationNum();
  webServer.handleClient();
}