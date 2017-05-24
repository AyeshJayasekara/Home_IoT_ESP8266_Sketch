/*********************************************************************************
  Pubnub Subscribe 
*********************************************************************************/
#include <ESP8266WiFi.h>
#include "VirtualDelay.h"
#include <math.h>  
VirtualDelay virtualDelay(millis);
#include "ShiftRegister74HC595.h"
const char* g_ssid       = "Dialog 4G";
const char* g_password   = "*******";
const char* g_host       = "pubsub.pubnub.com";
const char* g_pubKey     = "pub-c-89d87bfb-89a1-4ae9-941c-631dd1ea5dbd";
const char* g_subKey     = "sub-c-e7c47d7a-13be-11e7-a9ec-0619f8945a4f";
const char* g_channel    = "0";
String      timeToken    = "0";
double temp=0;
int soil=0;
int rain=0;
bool decodePending = true;
bool decodePending2 = true;
WiFiClient client;


//int SER_Pin = 5;   //pin 14 on the 75HC595 
//int RCLK_Pin = 4;  //pin 12 on the 75HC595
//nt SRCLK_Pin = 0; //pin 11 on the 75HC595
/*
  ShiftRegister74HC595.h - Library for easy control of the 74HC595 shift register.
  Created by Timo Denk (www.simsso.de), Nov 2014.
  Additional information are available on http://shiftregister.simsso.de/
  Released into the public domain. kaludamalu
*/



// create shift register object (number of shift registers, data pin, clock pin, latch pin)
ShiftRegister74HC595 sr (4, 5, 4, 0); 

void GET();
void POST();
void getNextTimeToken(String l);
void readTemp();
void readRain();
void readSoil();
void decodeLine(String l);
void applyChanges(String msg);
void setDisplay(int T);
void setup()
{
  Serial.begin(9600);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(g_ssid);
  
  WiFi.begin(g_ssid, g_password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  sr.setAllLow();
}


void loop()
{
//  delay(10); 
  //DATA FORMAT : http://pubsub.pubnub.com /publish/pub-key/sub-key/signature/channel/callback/message
  //SUB FORMAT :  http://pubsub.pubnub.com /subscribe/sub-key/channel/callback/timetoken
  //if(virtualDelay.done(1000))
 readTemp();
 readSoil();
 readRain();
 POST();
 delay(1000);
   GET();
if(temp>33)
  sr.set(7,HIGH);
  else
  sr.set(7,LOW);
  
}

void getNextTimeToken(String l){
      //l=l.substring(l.lastIndexOf(',')).substring(2,l.length()-2)
    l=l.substring(l.lastIndexOf(','));
    l=l.substring(2,l.length()-2);
    //Serial.println(l);
    if(l == timeToken){
      timeToken= "0";
    }
    else{
      timeToken = l;
      decodePending=true;
    }
    
}
void decodeLine(String l){
      decodePending=false;
      l = l.substring(4,l.indexOf(',')-2);
      Serial.println(l);
      delay(100);
      applyChanges(l);
  }

void applyChanges(String msg){
  for(int i =0 ; i < 8 ; i ++)
    //Serial.println(msg.charAt(i));
        if(msg.charAt(i)=='0')
          sr.set(i,LOW);
          else
          sr.set(i,HIGH);
}

void POST(){
  const int l_httpPort = 80;
  String puburl = "/publish/";
  puburl += g_pubKey;
  puburl += "/";
  puburl += g_subKey;
  puburl += "/0/";
  //puburl += '"';
  puburl += g_channel;
 // puburl += '"';
  puburl += "/0/";
  puburl += '"';
  puburl += (int)temp;
  puburl += '!';
  puburl += soil;
  puburl += '!';
  puburl += rain;
  puburl += '!';
  puburl += '"';
if (!client.connect("pubsub.pubnub.com", 80))
  {
    Serial.println("connection failed");
    return;
  }
  else{
    Serial.println("Connected to PUBNUB PUBLISH");
  }
  client.print(String("GET ") + puburl + " HTTP/1.1\r\n" +
               "Host: " + g_host + "\r\n"+
                "Connection: close\r\n\r\n");
  delay(1);
  while (client.available())
  {
    String line = client.readStringUntil('\r');
    if (line.endsWith("]"))
    {
        Serial.println(line);
    }
  }
  client.stop();
  client.flush(); 
}

void GET(){
  String url = "/subscribe/";
  url += g_subKey;
  url += "/";
  url += g_channel;
  url += "/0/";
  url += timeToken;
  //WiFiClient client;
  const int l_httpPort = 80;
if (!client.connect("pubsub.pubnub.com", 80))
  {
    Serial.println("connection failed");
    return;
  }
  else{
    Serial.println("Connected to PUBNUB");
  }
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + g_host + "\r\n"+
               "Connection: close\r\n\r\n");
               delay(1);
  while (client.available())
  {
    String line = client.readStringUntil('\r');
    if (line.endsWith("]"))
    {
        Serial.println(line);
        getNextTimeToken(line);
        if(decodePending)
        decodeLine(line);
    }
  }
client.stop();
client.flush();
}

void readTemp(){
  int pretemp = temp;
  sr.set(13,LOW);
  sr.set(14,LOW);
  sr.set(15,LOW);
  delay(100);
  temp=analogRead(A0);
  //temp=temp-1024;
  temp= ((1024-temp)/1024.0 -0.5)*100;
  if(pretemp != (int)temp)
  setDisplay(temp);
}
void readSoil(){
  sr.set(13,LOW);
  sr.set(14,LOW);
  sr.set(15,HIGH);
  delay(100);
  soil=analogRead(A0);
}
void readRain(){
  sr.set(13,LOW);
  sr.set(14,HIGH);
  sr.set(15,LOW);
  delay(100);
  rain=analogRead(A0);
}
 void setDisplay(int T){
    int a = T/10;
    int b = T%10;
    switch(b){
      case 0:
      sr.set(28,LOW);
      sr.set(29,LOW);
      sr.set(30,LOW);
      sr.set(31,LOW);
      break;
      case 1:
      sr.set(28,HIGH);
      sr.set(29,LOW);
      sr.set(30,LOW);
      sr.set(31,LOW);
      break;
      case 2:
      sr.set(28,LOW);
      sr.set(29,HIGH);
      sr.set(30,LOW);
      sr.set(31,LOW);
      break;
      case 3:
      sr.set(28,HIGH);
      sr.set(29,HIGH);
      sr.set(30,LOW);
      sr.set(31,LOW);
      break;
      case 4:
      sr.set(28,LOW);
      sr.set(29,LOW);
      sr.set(30,HIGH);
      sr.set(31,LOW);
      break;
      case 5:
      sr.set(28,HIGH);
      sr.set(29,LOW);
      sr.set(30,HIGH);
      sr.set(31,LOW);
      break;
      case 6:
      sr.set(28,LOW);
      sr.set(29,HIGH);
      sr.set(30,HIGH);
      sr.set(31,LOW);
      break;
      case 7:
      sr.set(28,HIGH);
      sr.set(29,HIGH);
      sr.set(30,HIGH);
      sr.set(31,LOW);
      break;
      case 8:
      sr.set(28,LOW);
      sr.set(29,LOW);
      sr.set(30,LOW);
      sr.set(31,HIGH);
      break;
      case 9:
      sr.set(28,HIGH);
      sr.set(29,LOW);
      sr.set(30,LOW);
      sr.set(31,HIGH);
      break;
    }
    switch(a){
      case 0:
      sr.set(24,LOW);
      sr.set(25,LOW);
      sr.set(26,LOW);
      sr.set(27,LOW);
      break;
      case 1:
      sr.set(24,HIGH);
      sr.set(25,LOW);
      sr.set(26,LOW);
      sr.set(27,LOW);
      break;
      case 2:
      sr.set(24,LOW);
      sr.set(25,HIGH);
      sr.set(26,LOW);
      sr.set(27,LOW);
      break;
      case 3:
      sr.set(24,HIGH);
      sr.set(25,HIGH);
      sr.set(26,LOW);
      sr.set(27,LOW);
      break;
      case 4:
      sr.set(24,LOW);
      sr.set(25,LOW);
      sr.set(26,HIGH);
      sr.set(27,LOW);
      break;
      case 5:
      sr.set(24,HIGH);
      sr.set(25,LOW);
      sr.set(26,HIGH);
      sr.set(27,LOW);
      break;
      case 6:
      sr.set(24,LOW);
      sr.set(25,HIGH);
      sr.set(26,HIGH);
      sr.set(27,LOW);
      break;
      case 7:
      sr.set(24,HIGH);
      sr.set(25,HIGH);
      sr.set(26,HIGH);
      sr.set(27,LOW);
      break;
      case 8:
      sr.set(24,LOW);
      sr.set(25,LOW);
      sr.set(26,LOW);
      sr.set(27,HIGH);
      break;
      case 9:
      sr.set(24,HIGH);
      sr.set(25,LOW);
      sr.set(26,LOW);
      sr.set(27,HIGH);
      break;
    }
 }





















