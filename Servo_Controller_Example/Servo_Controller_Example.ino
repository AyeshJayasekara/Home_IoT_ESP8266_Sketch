#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
Servo testservo;

uint32_t next;

int LedPin =13;
void setup() {
  testservo.attach(13, 1000, 2000);

  next = millis() + 500;
  //analogWrite( 13, 255);
}



void loop() {
 static bool rising = true;

  if(millis() > next)
  {
    if(rising)
    {
      testservo.write(0);
      rising = false;
    }
    else
    {
      testservo.write(+180);
      rising = true;
    }

    // repeat again in 3 seconds.
    next += 3000;
  }
}
