#ifndef BTC_TICKER_ITICKER_DISPLAY_HPP
#define BTC_TICKER_ITICKER_DISPLAY_HPP

#include <Print.h>

enum class TickerColor { COL_BLACK, COL_WHITE, COL_RED, COL_GREEN };

class ITickerDisplay : public Print {
  public:
    virtual void init() = 0;
    virtual void clear() = 0;
    virtual void show() = 0;
    virtual void fillRect(int32_t x, int32_t y, int32_t w, int32_t h,
                          TickerColor color) = 0;
    virtual void drawVLine(int32_t x, int32_t y, int32_t h, TickerColor color) = 0;
    virtual void drawRect(int32_t x, int32_t y, int32_t w, int32_t h,
                          TickerColor color) = 0;
    virtual void setCursor(int16_t x, int16_t y) = 0;
    virtual void setTextSize(uint8_t s) = 0;
    virtual bool isBlackWhite() = 0;

    size_t write(uint8_t uint8) override = 0;
};

#endif // BTC_TICKER_ITICKER_DISPLAY_HPP