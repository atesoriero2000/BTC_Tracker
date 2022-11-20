#include <LiquidCrystal.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "Symbols.h"

#define SSID  "xxxxxxxxxxxxxxx"
#define KEY   "xxxxxxxxxxxxxxx"
#define URL   "/v1/bpi/currentprice/USD.json"
#define HOST  "api.coindesk.com"
#define PORT  443

WiFiClientSecure client;
HTTPClient https;
DynamicJsonBuffer jsonBuffer(517);
LiquidCrystal lcd(14, 12, 2, 0, 4, 5);

void setup() {
  Serial.begin(9600);

  //###############
  //## LCD Setup ##
  //###############
  lcd.createChar(0, wifiDisconected);
  lcd.createChar(1, wifiConnected);
  lcd.createChar(2, APIDisconnected);
  lcd.createChar(3, APIConnected);
  lcd.createChar(4, badHTTPReq);
  lcd.createChar(5, goodHTTPReq);
  lcd.begin(16, 2);
  
  //################
  //## WIFI Setup ##
  //################
  WiFi.begin(SSID, KEY);
  lcd.print("Connecting WiFI");
  String str1 = "................";
  for (int i = 0; (WiFi.status() != WL_CONNECTED); i++) {
    lcd.setCursor(0, 1);
    lcd.print(str1.substring(0, i % 17) + "                ");
    delay(500);
  }
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  //###############
  //## API Setup ##
  //###############
  client.setInsecure();
  lcd.clear();
  lcd.print("Connecting");
  lcd.setCursor(0, 1);
  lcd.print("Coindesk API");
  https.begin(client, HOST, PORT, URL);
}

void loop() {
  int httpsCode = https.GET();
  JsonObject& root = jsonBuffer.parseObject(https.getString());
  double currentRate = root["bpi"]["USD"]["rate_float"];

  printLCDHeader(httpsCode);
  printLCDRate(currentRate);
  delay(60000);
}

void printLCDHeader(int httpsCode){
  lcd.clear();
  lcd.print("BTC-USD:");
  lcd.setCursor(13, 0);
  lcd.write(WiFi.status() == WL_CONNECTED); //TODO: Make Symbols
  lcd.write(client.connected() + 2);
  lcd.write((httpsCode == HTTP_CODE_OK) + 4);
}

double lastRate = 0;
void printLCDRate(double rate){
  if (!rate) rate = lastRate;
  lcd.setCursor(0,1);
  String rate_str = String(rate, 4);
  lcd.print("$" + rate_str.substring(0, rate_str.length()-8) + "," + rate_str.substring(rate_str.length()-8));
  lastRate = rate;
}