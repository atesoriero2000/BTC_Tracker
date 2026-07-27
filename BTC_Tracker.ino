#include <LiquidCrystal.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "Symbols.h"

//#define SSID  "xxxxxxxxxxxxxxxxxxxx"
//#define KEY   "xxxxxxxxxxxxxxxxxxxx"
#define SSID "36Albione"
#define KEY "LigmaChops24"
//#define SSID "Tesfamily"
//#define KEY "Tes8628125601"
//#define SSID  "29BrackettBoys"
//#define KEY   "Frick35Jewett"
//#define SSID  "WPI Sailbot"
//#define KEY   "YJKFMP6B8D"

#define HOST  "api.coinbase.com"
#define PORT  443

#define SCREEN_DURATION   5000   // ms to display each asset
#define REFRESH_INTERVAL  60000  // ms between price refreshes

WiFiClientSecure client;
HTTPClient https;
DynamicJsonDocument doc(1024);
LiquidCrystal lcd(14, 12, 2, 0, 4, 5);

double btcPrice   = 0;
double goldPrice  = 0;
double silverPrice = 0;
int    lastHttpsCode = 0;

unsigned long lastRefreshTime = 0;
unsigned long lastScreenSwitch = 0;
int currentScreen = 0; // 0=BTC, 1=Gold, 2=Silver

// Format a dollar amount with comma-thousands and 2 decimal places.
// Handles up to $999,999.99 (covers BTC, gold, and silver ranges).
String formatPrice(double price) {
  long cents   = (long)(price * 100.0 + 0.5);
  long dollars = cents / 100;
  int  frac    = (int)(cents % 100);

  String dollarStr;
  if (dollars >= 1000) {
    dollarStr = String(dollars / 1000) + "," +
                String(dollars % 1000 / 100) +
                String(dollars % 100  / 10)  +
                String(dollars % 10);
  } else {
    dollarStr = String(dollars);
  }

  String fracStr = (frac < 10 ? "0" : "") + String(frac);
  return "$" + dollarStr + "." + fracStr;
}

double fetchPrice(const char* url) {
  https.begin(client, HOST, PORT, url);
  int code = https.GET();
  lastHttpsCode = code;
  double result = 0;
  if (code == HTTP_CODE_OK) {
    deserializeJson(doc, https.getString());
    // Coinbase returns amount as a JSON string, so use atof for reliable conversion
    const char* amountStr = doc["data"]["amount"];
    if (amountStr) result = atof(amountStr);
  }
  https.end();
  return result;
}

void refreshPrices() {
  double val;
  val = fetchPrice("/v2/prices/BTC-USD/spot");
  if (val) btcPrice    = val;
  val = fetchPrice("/v2/prices/XAU-USD/spot");
  if (val) goldPrice   = val;
  val = fetchPrice("/v2/prices/XAG-USD/spot");
  if (val) silverPrice = val;
}

void printLCDHeader(const char* label) {
  lcd.clear();
  lcd.print(label);
  lcd.setCursor(13, 0);
  lcd.write(WiFi.status() == WL_CONNECTED ? 1 : 0);
  lcd.write(lastHttpsCode > 0            ? 3 : 2);
  lcd.write(lastHttpsCode == HTTP_CODE_OK ? 5 : 4);
}

void displayScreen(int screen) {
  switch (screen) {
    case 0:
      printLCDHeader("BTC-USD:");
      lcd.setCursor(0, 1);
      lcd.print(btcPrice    ? formatPrice(btcPrice)               : "--");
      break;
    case 1:
      printLCDHeader("GOLD-USD:");
      lcd.setCursor(0, 1);
      lcd.print(goldPrice   ? formatPrice(goldPrice)   + "/oz"    : "--");
      break;
    case 2:
      printLCDHeader("SILVER-USD:");
      lcd.setCursor(0, 1);
      lcd.print(silverPrice ? formatPrice(silverPrice) + "/oz"    : "--");
      break;
  }
}

void setup() {
  Serial.begin(115200);

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
  lcd.clear();
  lcd.print("Connecting");
  lcd.setCursor(0, 1);
  lcd.print("Coinbase API");
  client.setInsecure();

  refreshPrices();
  lastRefreshTime  = millis();
  lastScreenSwitch = millis();
  displayScreen(currentScreen);
}

void loop() {
  unsigned long now = millis();

  // Refresh all three prices every REFRESH_INTERVAL ms
  if (now - lastRefreshTime >= REFRESH_INTERVAL) {
    refreshPrices();
    lastRefreshTime = now;
    displayScreen(currentScreen);
  }

  // Rotate to the next screen every SCREEN_DURATION ms
  if (now - lastScreenSwitch >= SCREEN_DURATION) {
    currentScreen    = (currentScreen + 1) % 3;
    lastScreenSwitch = now;
    displayScreen(currentScreen);
  }
}
