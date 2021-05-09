/*
 * Project Name - Pulse Oximeter
 * Created by - Sashwat K
 * Created on - 09/05/2021 11:00:00
 * Modified on - 09/05/2021 11:00:00
 * GitHub link - https://github.com/sashuu6/pulse-oximeter
 */

#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define SENSOR_ERR_LED 4
#define DISPLAY_ERR_LED 5

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define REPORTING_PERIOD_MS 1000
PulseOximeter pox;
uint32_t tsLastReport = 0;

ESP8266WebServer server(80);

int heartbeat;
int spo2;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Pulse oximeter");
  
  pinMode(SENSOR_ERR_LED, OUTPUT);
  digitalWrite(SENSOR_ERR_LED, LOW);

  pinMode(DISPLAY_ERR_LED, OUTPUT);
  digitalWrite(DISPLAY_ERR_LED, LOW);

  if (!pox.begin()) {
    Serial.println("FAILED");
    digitalWrite(SENSOR_ERR_LED, HIGH);
    for(;;);
   } 
   else {
    pox.setOnBeatDetectedCallback(onBeatDetected);
   }

   if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
    digitalWrite(DISPLAY_ERR_LED, HIGH);
  }
  display.display();
  display.clearDisplay();

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  pox.update();

  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    heartbeat = pox.getHeartRate();
    spo2 = pox.getSpO2()
    display.clearDisplay();
    display.setTextSize(1); 
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.println(heartbeat);
    display.setCursor(0,4);
    display.println(spo2);
    tsLastReport = millis();
  }
}

void onBeatDetected()
{
    Serial.println("Beat!");
}

void handle_OnConnect() {
  server.send(200, "text/html", SendHTML(heartbeat,spo2)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(int heartbeat,int spo2){
   String ptr = "<!DOCTYPE html> <html>\n";
   return ptr;
}
