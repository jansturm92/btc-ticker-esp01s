#ifndef BTC_TICKER_SSD1306_DISPLAY_HPP
#define BTC_TICKER_SSD1306_DISPLAY_HPP

#include "ITickerDisplay.hpp"
#include "config.h"
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>

#define PIN_SDA 2
#define PIN_SCL 0

class SSD1306Display : public ITickerDisplay {

  private:
    Adafruit_SSD1306 display =
        Adafruit_SSD1306(OLED_SCREEN_WIDTH, OLED_SCREEN_HEIGHT, &Wire, -1);

    static uint16_t mapColor(TickerColor color) {
        switch (color) {
        case TickerColor::COL_WHITE:
        case TickerColor::COL_GREEN:
            return SSD1306_WHITE;
        case TickerColor::COL_BLACK:
        case TickerColor::COL_RED:
        default:
            return SSD1306_BLACK;
        }
    }

  public:
    void init() override {
        Wire.begin(PIN_SDA, PIN_SCL);

        if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
            SERIAL_PRINTLN(F("SSD1306 allocation failed"));
            while (true)
                ;
        }
        display.setTextColor(WHITE);
        clear();
    }

    void clear() override {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.setTextSize(1);
    }

    void fillRect(int32_t x, int32_t y, int32_t w, int32_t h,
                  TickerColor color) override {
        display.fillRect(x, y, w, h, mapColor(color));
    }

    void drawVLine(int32_t x, int32_t y, int32_t h, TickerColor color) override {
        display.drawFastVLine(x, y, h, mapColor(color));
    }

    void drawRect(int32_t x, int32_t y, int32_t w, int32_t h,
                  TickerColor color) override {
        display.drawRect(x, y, w, h, mapColor(color));
    }

    void setCursor(int16_t x, int16_t y) override { display.setCursor(x, y); }

    void setTextSize(uint8_t s) override { display.setTextSize(s); }

    bool isBlackWhite() override { return true; }

    size_t write(uint8_t uint8) override { return display.write(uint8); }

    void show() override { display.display(); }
};

#define DISPLAY_PRINTF(f_, ...)                                                          \
    do {                                                                                 \
        SERIAL_PRINTF((f_), ##__VA_ARGS__);                                              \
        display.printf((f_), ##__VA_ARGS__);                                             \
        display.show();                                                                  \
    } while (0);

#endif // BTC_TICKER_SSD1306_DISPLAY_HPP
