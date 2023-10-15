#include <WS2812FX.h>

namespace LED {

#define LED_COUNT 12
#define LED_PIN 16
#define DISTANCE_VERY_CLOSE 0
#define DISTANCE_CLOSE 1
#define DISTANCE_AWAY 2
#define RSSI_THRESHOLD_CLOSE -73
#define RSSI_THRESHOLD_VERY_CLOSE -55

  WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
  int closestDeviceDistance = DISTANCE_AWAY;

  void setup() {
      ws2812fx.init();
      ws2812fx.setBrightness(100);
      ws2812fx.setMode(FX_MODE_RAINBOW_CYCLE);
      ws2812fx.setSpeed(10000);
      ws2812fx.start();
  }

  void loop(bool deviceIsClose, int rssi) {
      if (rssi > RSSI_THRESHOLD_VERY_CLOSE) {
          if (closestDeviceDistance != DISTANCE_VERY_CLOSE){
              Serial.printf("Device became very close ");
              closestDeviceDistance = DISTANCE_VERY_CLOSE;
              ws2812fx.init();
              ws2812fx.setBrightness(100);
              ws2812fx.setMode(FX_MODE_BLINK);
              ws2812fx.setSpeed(100);
              ws2812fx.setColor(255, 0, 255);
              ws2812fx.start();
          }
      }
      else if (rssi > RSSI_THRESHOLD_CLOSE) {
          if (closestDeviceDistance != DISTANCE_CLOSE){
              Serial.printf("Device became close ");
              closestDeviceDistance = DISTANCE_CLOSE;
              ws2812fx.init();
              ws2812fx.setBrightness(100);
              ws2812fx.setMode(FX_MODE_RAINBOW_CYCLE);
              ws2812fx.setSpeed(7000);
              ws2812fx.start();
          }
      }
      else {
          if (closestDeviceDistance != DISTANCE_AWAY){
              Serial.printf("Device became far away ");
              closestDeviceDistance = DISTANCE_AWAY;
              ws2812fx.init();
              ws2812fx.setBrightness(100);
              ws2812fx.setMode(FX_MODE_STATIC);
              ws2812fx.setColor(255, 0, 0);
              ws2812fx.setSpeed(100);
              ws2812fx.start();
          }
      }

      LED::ws2812fx.service();
  }
}
