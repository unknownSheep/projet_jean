#define ARDUINOJSON_USE_LONG_LONG 1

#include <TimeLib.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>
#include <WiFiUDP.h>
#include <NTPClient.h>


ESP8266WiFiMulti WiFiMulti;
WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "pool.ntp.org");

void setup() {
  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("redmi", "motdepasse");
//  WiFiMulti.addAP("freebox_IYHYJI", "f7peterselie+");
}

void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED))
  {
//    String rep = doRequest("https://euw1.api.riotgames.com/lol/spectator/v4/active-games/by-summoner/nd-Hs3NUtQxSujekbANIOFXjE3IJ3bccHQPc668QYj5D0cwR41oN8bQikQ?api_key=RGAPI-1883218c-7ebf-4802-b262-b93d11983a7f");
    String rep = doRequest("https://euw1.api.riotgames.com/lol/spectator/v4/active-games/by-summoner/7nqMfbimsivZUoTIv_At9yoSzBDUEVhRByKhErgUaKUXiL8?api_key=RGAPI-5483d786-8476-4579-b6d3-89eab98ed17e");
//    String rep = doRequest("https://euw1.api.riotgames.com/lol/summoner/v4/summoners/by-name/TranMinhVIE?api_key=RGAPI-3cc904fb-cae7-4e7c-b09f-0c337b58d663");
    Serial.println(rep);

    if(rep == "ERROR") // ERROR =>
    {

    }
    else if (rep == "OFF") // OFFLINE =>
    {
      
    }
    else // ONLINE
    {
      DynamicJsonBuffer jsonBuffer;

      JsonObject& root = jsonBuffer.parseObject(rep);

      if (!root.success()) {
        Serial.println("parseObject() failed");
        delay(10000);
        return;
      }
      long long start_time = root["gameStartTime"];
      jsonBuffer.clear();
      Serial.println(getTime() - (start_time/1000));
    }
//    delay(1000);
  }
}


long long getTime()
{
  timeClient.update();
  long long now = timeClient.getEpochTime();
  return now;
}


String doRequest(String url)
{
    String result;
  
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    client->setInsecure();

    HTTPClient https;

    //Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, url)) {  // HTTPS

      https.useHTTP10(true);
      //Serial.print("[HTTPS] GET...\n");
      int httpCode = https.GET();
            
      // httpCode will be negative on error
      if (httpCode > 0)
      {
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
        {
          result = https.getString();
        }else
        {
          result = "OFF";
//          result = https.getString();

        }
      } else
      {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        result = "ERROR";
      }

      https.end();
    } else
    {
      Serial.printf("[HTTPS] Unable to connect\n");
      result = "ERROR";
    }
    return result;
  }



























  
