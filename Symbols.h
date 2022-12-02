// ESP8266 ==> | A0 | D0 | D1 | D2 | D3 | D4 | D5 | D6 | D7 | D8 | RX | TX | SD2 | SD3 |
// Arduino ==> | A0 | 16 |  5 |  4 |  0 |  2 | 14 | 12 | 13 | 15 |  3 |  1 |  9  |  10 |

// LCD     ==> | VSS | VDD | VEE | RS | RW | E | D0 | D1 | D2 | D3 | D4 | D5 | D6 | D7 | LED_A | LED_K |
// Pin #   ==> |  1  |  2  |  3  |  4 |  5 | 6 |  7 |  8 |  9 | 10 | 11 | 12 | 13 | 14 |   15  |   16  |

// LCD->ESP8->IDE
// D7 -> D1 -> 5
// D6 -> D2 -> 4
// D5 -> D3 -> 0
// D4 -> D4 -> 2
// RS -> D5 -> 14
// E  -> D6 -> 12

//MAC Address: 50:02:91:78:DA:F2
#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <Arduino.h>

//Symbol Creator:   https://maxpromer.github.io/LCD-Character-Creator/
//ESP8266Wifi Docs: https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html#bearssl-client-secure-and-server-secure

extern byte wifiConnected[] = {
  B11111,
  B00001,
  B11101,
  B00101,
  B10101,
  B00000,
  B00000,
  B00000
};

extern byte wifiDisconected[] = {
  B10001,
  B01010,
  B00100,
  B01010,
  B10001,
  B00000,
  B00000,
  B00000
};

extern byte APIDisconnected[]{
  B10101,
  B10101,
  B10101,
  B00000,
  B10101,
  B00000,
  B00000,
  B00000
};

extern byte APIConnected[] = {
  B01010,
  B01010,
  B00000,
  B10001,
  B01110,
  B00000,
  B00000,
  B00000
};

extern byte badHTTPReq[]{
  B01010,
  B01010,
  B00000,
  B01110,
  B10001,
  B00000,
  B00000,
  B00000
};

extern byte goodHTTPReq[] = {
  B01010,
  B01010,
  B00000,
  B10001,
  B01110,
  B00000,
  B00000,
  B00000
};

#endif
