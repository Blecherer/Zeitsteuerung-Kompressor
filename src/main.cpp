#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include "RTClib.h"
#include <SPI.h>

// variables for measured values
int NumberClients;
char wochentage[7][12] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};

// put function declarations here:
int myFunction(int, int);

// create web server
ESP8266WebServer webServer(80);

RTC_DS3231 rtc;

void handleRoot()
{
  // digitalWrite(led, 1);
  webServer.send(200, "text/plain", "hello from esp8266!");
  // digitalWrite(led, 0);
}

void handleNotFound()
{
  // digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += webServer.uri();
  message += "\nMethod: ";
  message += (webServer.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += webServer.args();
  message += "\n";
  for (uint8_t i = 0; i < webServer.args(); i++)
  {
    message += " " + webServer.argName(i) + ": " + webServer.arg(i) + "\n";
  }
  webServer.send(404, "text/plain", message);
  // digitalWrite(led, 0);
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  if (!rtc.begin())
  {
    Serial.print("Finde keine RTC");
    while (true)
      ;
  }

  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Zeit vom Compiler
  // rtc.adjust(DateTime(2022, 11, 21, 13, 0, 0)); // J, M, T, Std, Min, Sek

  Serial.print("Konfiguriere soft-AP ... ");
  boolean result = WiFi.softAP("WemosAP", "geheim123");

  Serial.print("Verbindung wurde ");
  if (result == false)
  {
    Serial.println("NICHT ");
  }
  Serial.print("erfolgreich aufgebaut!");

  webServer.on("/", handleRoot);

  webServer.on("/inline", []()
  { webServer.send(200, "text/plain", "this works as well"); });

  webServer.onNotFound(handleNotFound);

  webServer.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  NumberClients = WiFi.softAPgetStationNum();
  webServer.handleClient();

  DateTime now = rtc.now();
  char buf1[] = "hh:mm";
  Serial.println(now.toString(buf1));
  char buf2[] = "YYMMDD-hh:mm:ss";
  Serial.println(now.toString(buf2));
  char buf3[] = "Today is DDD, MMM DD YYYY";
  Serial.println(now.toString(buf3));
  char buf4[] = "DD.MM.YYYY";
  Serial.println(now.toString(buf4));
  char buf5[] = "MM/DD/YYYY";
  Serial.println(now.toString(buf5));
}