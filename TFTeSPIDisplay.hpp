#ifndef BTC_TICKER_TFT_ESPI_DISPLAY_HPP
#define BTC_TICKER_TFT_ESPI_DISPLAY_HPP

#include "ITickerDisplay.hpp"
#include "config.h"
#include <TFT_eSPI.h>

class TFTeSPIDisplay : public ITickerDisplay {

  private:
    TFT_eSPI display = TFT_eSPI(OLED_SCREEN_HEIGHT, OLED_SCREEN_WIDTH);

    static uint16_t mapColor(TickerColor color) {
        switch (color) {
        case TickerColor::COL_RED:
            return TFT_RED;
        case TickerColor::COL_WHITE:
            return TFT_WHITE;
        case TickerColor::COL_GREEN:
            return TFT_GREEN;
        case TickerColor::COL_BLACK:
        default:
            return TFT_BLACK;
        }
    }

  public:
    void init() override {
        display.init();
        display.setRotation(1);
        clear();
    }

    void clear() override {
        display.fillScreen(TFT_BLACK);
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

    bool isBlackWhite() override { return false; }

    size_t write(uint8_t uint8) override { return display.write(uint8); }

    void show() override {}
};

#define DISPLAY_PRINTF(f_, ...)                                                          \
    do {                                                                                 \
        SERIAL_PRINTF((f_), ##__VA_ARGS__);                                              \
        display.printf((f_), ##__VA_ARGS__);                                             \
    } while (0);

#endif // BTC_TICKER_TFT_ESPI_DISPLAY_HPP
