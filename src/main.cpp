#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include "RTClib.h"
#include <SPI.h>

// variables for measured values
int NumberClients;
char wochentage[7][12] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};
const char *monthName[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
int LEDrot = D5;       // Farbe rot an Pin 5
int LEDgruen = D6;     // Farbe gruen an Pin 6
int dunkel = 0;        // Zahlenwert 0 bedeutet Spannung 0V – also LED aus
int brightness1 = 150; // Zahlenwert zwischen 0 und 255 – gibt die Leuchtstärke der einzelnen Farbe an

int Relay1 = D2;
int Relay2 = D3;

// create web server
ESP8266WebServer webServer(80);

// initialize RTC
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

void printTime()
{
  // print current time
  char date[10] = "hh:mm:ss";
  rtc.now().toString(date);
  Serial.println(date);
}

void setAlarm()
{
  int year = rtc.now().year();
  int month = rtc.now().month();
  int day = rtc.now().day();
  // schedule an Alarm for a certain date (day of month), hour, minute, and second
  DateTime alarmTime1(year, month, day, 10, 00, 0);
  rtc.setAlarm1(alarmTime1, DS3231_A1_Hour);

  // schedule an Alarm for a certain date (day of month), hour, minute, and second
  DateTime alarmTime2(year, month, day, 17, 00, 0);
  rtc.setAlarm2(alarmTime2, DS3231_A2_Hour);
}

void disablePowerOn()
{
  // schalte Relay 1 ein
  digitalWrite(Relay1, HIGH); // An dieser Stelle würde das Relais 1 einschalten
  digitalWrite(Relay2, LOW);  // An dieser Stelle würde das Relais 2 ausschalten
  // schalte LED auf rot
  analogWrite(LEDrot, brightness1);
  analogWrite(LEDgruen, dunkel);
}

void enablePowerOn()
{
  // schalte Relay 2 ein
  digitalWrite(Relay2, HIGH); // An dieser Stelle würde das Relais 2 einschalten
  digitalWrite(Relay1, LOW);  // An dieser Stelle würde das Relais 1 ausschalten
  // schalte LED auf grün
  analogWrite(LEDgruen, brightness1);
  analogWrite(LEDrot, dunkel);
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    while (1)
      ;
  }

  // we don't need the 32K Pin, so disable it
  rtc.disable32K();

  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Zeit vom Compiler
  // rtc.adjust(DateTime(2022, 11, 21, 13, 0, 0)); // J, M, T, Std, Min, Sek

  // set alarm 1, 2 flag to false (so alarm 1, 2 didn't happen so far)
  // if not done, this easily leads to problems, as both register aren't reset on reboot/recompile
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);

  // stop oscillating signals at SQW Pin
  // otherwise setAlarm1 will fail
  rtc.writeSqwPinMode(DS3231_OFF);

  // turn off alarm 2 (in case it isn't off already)
  // again, this isn't done at reboot, so a previously set alarm could easily go overlooked
  rtc.disableAlarm(2);

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

  // Relais Pins
  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);

  // Ausgang RGB LED
  pinMode(LEDgruen, OUTPUT);
  pinMode(LEDrot, OUTPUT);
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

  int stunde0 = 0;
  if (stunde0 == now.hour() and stunde0 == now.minute())
  {
    setAlarm();
  }

  // resetting SQW and alarm 1 flag
  // using setAlarm1, the next alarm could now be configurated
  if (rtc.alarmFired(1))
  {
    printTime();
    enablePowerOn();
    rtc.clearAlarm(1);
    Serial.println("Kompressor kann angeschaltet werden.");
    Serial.println();
  }
  // resetting SQW and alarm 1 flag
  // using setAlarm1, the next alarm could now be configurated
  if (rtc.alarmFired(2))
  {
    printTime();
    disablePowerOn();
    rtc.clearAlarm(2);
    Serial.println("Kompressor ist gesperrt.");
    Serial.println();
  }
}