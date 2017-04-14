/*********************************************************************************
  Pubnub Subscribe 
*********************************************************************************/
#include <ESP8266WiFi.h>

const char* g_ssid       = "Dialog 4G";
const char* g_password   = "soclab@1";
const char* g_host       = "pubsub.pubnub.com";
const char* g_pubKey     = "pub-c-784c01b6-34fa-46e2-aba7-5eebb8efac0e";
const char* g_subKey     = "sub-c-764a7b9e-16a3-11e7-bb8a-0619f8945a4f";
const char* g_channel    = "kozi";
String      timeToken    = "0";



void setup()
{
  Serial.begin(115200);
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

}


void loop()
{
  WiFiClient client;
  const int l_httpPort = 80;
  if (!client.connect(g_host, l_httpPort))
  {
    Serial.println("connection failed");
    return;
  }
  //DATA FORMAT : http://pubsub.pubnub.com /publish/pub-key/sub-key/signature/channel/callback/message
  //SUB FORMAT :  http://pubsub.pubnub.com /subscribe/sub-key/channel/callback/timetoken

  String url = "/subscribe/";
  url += g_subKey;
  url += "/";
  url += g_channel;
  url += "/0/";
  url += timeToken;
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + g_host + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(10);

  while (client.available())
  {
    String line = client.readStringUntil('\r');
    if (line.endsWith("]"))
    {
        Serial.println(line);
        getNextTimeToken(line);
        decodeLine(line);
    }
  }
  Serial.println("closing connection");
  delay(200);
}

void getNextTimeToken(String l){
      //l=l.substring(l.lastIndexOf(',')).substring(2,l.length()-2)
    l=l.substring(l.lastIndexOf(','));
    l=l.substring(2,l.length()-2);
    Serial.println(l);
    if(l == timeToken)
        timeToken= "0";
        else
    timeToken = l;
}
void decodeLine(String l){
      l = l.substring(4,l.indexOf(',')-2);
      Serial.println(l);
      delay(100);
  }


