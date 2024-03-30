#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>


// variables for measured values
int NumberClients;


// put function declarations here:
int myFunction(int, int);

// create web server
ESP8266WebServer webServer(80);

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

  webServer.on("/", handle_OnConnect);
  webServer.onNotFound(handle_NotFound);

  webServer.begin();
  Serial.println("HTTP server started");
}

void loop() {
  NumberClients = WiFi.softAPgetStationNum();
  delay(3000);
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}

void handle_OnConnect() {
  //temperature = dht.readTemperature();
  //humidity = dht.readHumidity();
  webServer.send(200, "text/html", buildHtml( ));
}

void handle_NotFound(){
  webServer.send(404, "text/plain", "Not found");
}

String buildHtml(int _NumberClients){
  String page = "<!DOCTYPE html> <html>\n";
  page +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\"/>\n";
  page +="<meta charset=\"UTF-8\"/>";
  page +="<meta http-equiv=\"refresh\" content=\"5\"/>";
  page +="<title>WeMos D1 mini Temperature & Humidity Report</title>\n";
  page +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  page +="body{margin-top: 50px;} h1 {color: #444444;margin: 20px auto 30px;}\n";
  page +="h2 {color: #0d4c75;margin: 50px auto 20px;}\n";
  page +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  page +="</style>\n";
  page +="</head>\n";
  page +="<body>\n";
  page +="<div id=\"webpage\">\n";
  page +="<h2>WeMos Lolin D1 mini</h2><h1>Anzahl Clients Report</h1>\n";
 
  page +="<p>Anzahl der Clients: ";
  page +=(int)_NumberClients;
  page +=" °C</p>";
   
  page +="</div>\n";
  page +="</body>\n";
  page +="</html>\n";
  return page;
}