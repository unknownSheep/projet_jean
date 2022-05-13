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
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

ESP8266WiFiMulti WiFiMulti;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#include <Servo.h>                    // include the Servo library
Servo servo;  


void setup() {
  servo.attach(D5);
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

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
}

void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED))
  {
//    String rep = doRequest("https://euw1.api.riotgames.com/lol/spectator/v4/active-games/by-summoner/nd-Hs3NUtQxSujekbANIOFXjE3IJ3bccHQPc668QYj5D0cwR41oN8bQikQ?api_key=api_key=RGAPI-41211a31-19f0-4274-942d-0cb5ec733d83");
    String rep = doRequest("https://euw1.api.riotgames.com/lol/spectator/v4/active-games/by-summoner/3iA3RUBaWuMJVgCNsIBz63z02M9yR3e3z9gZHwcosNA9GXE?api_key=RGAPI-41211a31-19f0-4274-942d-0cb5ec733d83");
//    String rep = doRequest("https://euw1.api.riotgames.com/lol/summoner/v4/summoners/by-name/TranMinhVIE?api_key=RGAPI-3cc904fb-cae7-4e7c-b09f-0c337b58d663");
    Serial.println(rep);

    if(rep == "ERROR") // ERROR =>
    {
      servo.write(0);
      
      display.clearDisplay();
      display.setTextColor(WHITE);
      
      display.setTextSize(3);
      display.setCursor(0,0);
      display.print("ERROR");
      display.display();
    }
    else if (rep == "OFF") // OFFLINE =>
    {
      servo.write(0);
      display.clearDisplay();
      display.setTextColor(WHITE);
      
      display.setTextSize(7);
      display.setCursor(0,0);
      display.print("OFF");
      display.display();
    }
    else // ONLINE
    {
      servo.write(90);    
      DynamicJsonBuffer jsonBuffer;

      JsonObject& root = jsonBuffer.parseObject(rep);

      if (!root.success()) {
        Serial.println("parseObject() failed");
        return;
      }
      long long start_time = root["gameStartTime"];
      jsonBuffer.clear();
      long long duration = getTime() - (start_time/1000);

      int minutes = floor(duration/60) - 1;
      int seconds = duration % 60;
      
      Serial.println(minutes);
      Serial.println(seconds);
      
      display.clearDisplay();
      display.setTextColor(WHITE);
      
      display.setTextSize(2);
      display.setCursor(0,0);
      display.print("In game :");
      
//      display.setTextSize(3);
//      display.setCursor(32, 32);
//      display.print(duration);
      display.setTextSize(3);
      
      display.setTextSize(3);
      
      String output;
      if (minutes >= 10)
      {
        output.concat(String(minutes));
      }
      else
      {
        output.concat("0");
        output.concat(String(minutes));
      }
      output.concat(":");
      
      if (seconds >= 10)
      {
        output.concat(String(seconds));
      }
      else
      {
        output.concat("0");
        output.concat(String(seconds));
      }
      
      display.setCursor(15, 32);
      display.print(output);
  
      display.display();
    }
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



























  
