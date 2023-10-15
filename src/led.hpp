#include <WS2812FX.h>

namespace LED {
#define LED_COUNT 12
#define LED_PIN 16

    WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

    void setup() {
        ws2812fx.init();
        ws2812fx.setBrightness(100);
        ws2812fx.setMode(FX_MODE_RAINBOW_CYCLE);
        ws2812fx.setSpeed(10000);
        ws2812fx.start();
    }

    void loop(bool deviceIsClose, int rssi) {
        if (deviceIsClose) {
            LED::ws2812fx.service();
        }
    }
}
