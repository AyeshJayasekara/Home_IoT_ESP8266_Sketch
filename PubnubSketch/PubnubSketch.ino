/*********************************************************************************
  ESP8266 Pubnub IoT Project
  Author : Ayesh Jayasekara , Kosala Surawimala 
*********************************************************************************/
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <String.h>
#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12
#define D7 13

const char* g_ssid       = "cookiemonster";
const char* g_password   = "animated29143886";
const char* g_host       = "pubsub.pubnub.com";
const char* g_pubKey     = "pub-c-784c01b6-34fa-46e2-aba7-5eebb8efac0e";
const char* g_subKey     = "sub-c-764a7b9e-16a3-11e7-bb8a-0619f8945a4f";
const char* g_channel    = "kozi";
String      timeToken    = "0";
int isSet=0;

typedef enum RET{
  FAILURE = 0,
  SUCCESS
}RET_VALUE;
/*********************************************************************************
  Function Name     : setup
  Description       : Initialize the Serial Communication with baud 115200
  Parameters        : void
  Return            : void
*********************************************************************************/

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println();

}
/*********************************************************************************
  Function Name     : loop
  Description       : Connect to the Wifi & PUBNUB REST API with TCP/IP Connection
                    and Subscribe for messages sent from Android Application.
  Parameters        : void
  Return            : void
*********************************************************************************/
void loop()
{
  if(isSet==0){
      isSet=1;

  Serial.print("Connecting to ");
  Serial.println(g_ssid);

  WiFi.begin(g_ssid, g_password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
      
      pinMode(D0,OUTPUT);
      digitalWrite(D0,LOW);
      pinMode(D1,OUTPUT);
      digitalWrite(D1,LOW);
      pinMode(D2,OUTPUT);
      digitalWrite(D2,LOW);
      pinMode(D3,OUTPUT);
      digitalWrite(D3,LOW);
      pinMode(D4,OUTPUT);
      digitalWrite(D4,LOW);
      pinMode(D5,OUTPUT);
      digitalWrite(D5,LOW);
      pinMode(D6,OUTPUT);
      digitalWrite(D6,LOW);
      pinMode(D7,OUTPUT);
      digitalWrite(D7,LOW);

      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
    }
  
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
      string_parser(line);
      Serial.println(line);
      
      //json_handler(string_parser(line));
    }
  }
  Serial.println("closing connection");
  delay(5000);
}

void Decript(char c){
  switch (c){
    case 'a':
         digitalWrite(D0,HIGH);
         break;
    case 'b':
         digitalWrite(D0,LOW);
         break;
    default:
      Serial.println("No Choice! HUTTO!");   
  }
}




/*
uint8_t json_handler(String p_json) {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json_parsed = jsonBuffer.parseObject(p_json);
  if (!json_parsed.success())
  {
    Serial.println("parseObject() failed");
    timeToken = "0";
    return FAILURE;
  }
  String key_parsed = json_parsed["text"];
  Serial.println(key_parsed);
  return SUCCESS;
}
*/
String string_parser(String data) {
  char bufferr[200];
  memset(bufferr, '\0', 200);

  int flower_count = 0;
  String json;

  typedef enum {
    STATE_INIT = 0,
    STATE_START_JSON,
    STATE_END_JSON,
    STATE_SKIP_JSON,
    STATE_INIT_TIME_TOKEN,
    STATE_START_TIME_TOKEN,
    STATE_END_TIME_TOKEN
  } STATE;
  STATE state = STATE_INIT;

  data.toCharArray(bufferr, 200);

  for (int i = 1; i <= strlen(bufferr); i++) {
    if (bufferr[i] == '[' && bufferr[i + 2] == '{') {
      state = STATE_START_JSON;
      json = "\0";
      continue;
    }
    else if (bufferr[i] == '[' && bufferr[i + 4] == '"') {
      state = STATE_INIT_TIME_TOKEN;
      json = "{}";
      timeToken = "\0";
      continue;
    }
    switch (state) {
      case STATE_START_JSON:
        if (bufferr[i] == '[') {
          break;
        }
        else if (bufferr[i] == '{') {
          flower_count++;
          json += bufferr[i];
        }
        else if (bufferr[i] == '}') {
          flower_count--;
          json += bufferr[i];
          if (flower_count <= 0) {
            state = STATE_END_JSON;
          }
        }
        else {
          json += bufferr[i];
          if (bufferr[i] == '}') {
            state = STATE_END_JSON;
          }
        }
        break;
      case STATE_END_JSON:
        if (bufferr[i] == ']' && bufferr[i + 2] == '"') {
          state = STATE_INIT_TIME_TOKEN;
          timeToken = "\0";
        }
        break;
      case STATE_INIT_TIME_TOKEN:
        if (bufferr[i] == '"') {
          state = STATE_START_TIME_TOKEN;
          timeToken = "\0";
        }
        break;
      case STATE_START_TIME_TOKEN:
        if (bufferr[i] == '"' && bufferr[i + 1] == ']') {
          state = STATE_END_TIME_TOKEN;
          break;
        }
        timeToken += bufferr[i];
        break;
      case STATE_END_TIME_TOKEN:
        break;
      default:
        break;
    }
  }
  return json;
}

//End of the Code
