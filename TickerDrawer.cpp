#include "TickerDrawer.hpp"
#include <limits>

TickerDrawer::TickerDrawer(Adafruit_SSD1306 &display) : display(display) {

    String candleInterval = String(CANDLE_INTERVAL);
    unsigned int stringLength = candleInterval.length();
    interval_amount = candleInterval.substring(0, stringLength - 1);
    interval = candleInterval[stringLength - 1];
}

void TickerDrawer::printPrices() {

    // clear whole price panel
    display.fillRect(0, PANEL_PRICES_TOP, OLED_SCREEN_WIDTH,
                     PANEL_PRICES_BOTTOM - PANEL_PRICES_TOP, BLACK);

    // print current price
    display.setTextSize(2);
    display.setCursor(0, PANEL_PRICES_TOP);
    display.printf("%d", candles.back().c);

    // print high low prices
    display.setTextSize(1);
    display.setCursor(PANEL_PRICES_HIGHLOW_X, PANEL_PRICES_TOP);
    display.printf("H %d", maxPrice);
    display.setCursor(PANEL_PRICES_HIGHLOW_X,
                      ((PANEL_PRICES_TOP + PANEL_PRICES_BOTTOM) >> 1) + 1);
    display.printf("L %d", minPrice);

    // print candle interval
    display.setCursor(PANEL_PRICES_INTERVAL_X, PANEL_PRICES_TOP);
    display.printf("%2s", interval_amount.c_str());
    display.setCursor(PANEL_PRICES_INTERVAL_X,
                      ((PANEL_PRICES_TOP + PANEL_PRICES_BOTTOM) >> 1) + 1);
    display.printf("%2s", interval.c_str());
}

void TickerDrawer::deleteCandles() {
    candles.clear();
    maxPrice = std::numeric_limits<int>::min();
    minPrice = std::numeric_limits<int>::max();
}

int TickerDrawer::update(const Candle newCandle) {

    unsigned long t_last = candles.back().t;
    unsigned long T_last = candles.back().T;

    if (t_last < newCandle.t) { // last candle closed
        // new candle is actually successor (scaled timestamps!)
        if (T_last == newCandle.t) {
            candles.erase(candles.begin());
            candles.push_back(newCandle);
            calculateMinMax();
            printAllCandles();
        } else {
            return ERROR_CANDLE_MISSING;
        }
    } else if (t_last == newCandle.t) { // last candle not closed
        candles.back() = newCandle;

        int oldMaxPrice = maxPrice;
        int oldMinPrice = minPrice;
        maxPrice = max(maxPrice, newCandle.h);
        minPrice = min(minPrice, newCandle.l);
        if (oldMaxPrice < maxPrice || oldMinPrice > minPrice) {
            printAllCandles();
        } else {
            printCandle(CANDLES_MAX - 1, newCandle);
        }
    }

    printPrices();
    return SUCCESS;
}

void TickerDrawer::appendCandle(const Candle candle) {

    maxPrice = max(maxPrice, candle.h);
    minPrice = min(minPrice, candle.l);
    candles.push_back(candle);
}

void TickerDrawer::printAllCandles() {
    display.fillRect(0, PANEL_CANDLE_TOP, OLED_SCREEN_WIDTH, PANEL_CANDLE_BOTTOM, BLACK);

    for (size_t i = 0; i < candles.size(); ++i) {
        printCandle(i, candles[i]);
    }
}

// Remap dollar amounts to pixels
static int getY(int val, int minVal, int maxVal) {
    return map(val, minVal, maxVal, PANEL_CANDLE_BOTTOM, PANEL_CANDLE_TOP - 1);
}

void TickerDrawer::printCandle(const size_t candleNumber, const Candle &candle) {

    int oy = getY(candle.o, minPrice, maxPrice);
    int hy = getY(candle.h, minPrice, maxPrice);
    int ly = getY(candle.l, minPrice, maxPrice);
    int cy = getY(candle.c, minPrice, maxPrice);
    int x = candleNumber * (CANDLE_WIDTH + CANDLE_SPACING);

    // clear the candle
    display.fillRect(x, PANEL_CANDLE_TOP, CANDLE_WIDTH, PANEL_CANDLE_BOTTOM, BLACK);
    // draw upper and lower shadow
    display.drawFastVLine(x + (CANDLE_WIDTH >> 1), hy, ly - hy, WHITE);
    // draw body
    display.drawRect(x, min(oy, cy), CANDLE_WIDTH, abs(cy - oy), WHITE);
    // positive or negative candle
    display.fillRect(x + 1, min(oy, cy) + 1, CANDLE_WIDTH - 2, abs(cy - oy) - 2,
                     cy > oy ? BLACK : WHITE);
}

void TickerDrawer::calculateMinMax() {

    maxPrice =
        std::max_element(candles.begin(), candles.end(),
                         [](const Candle &a, const Candle &b) { return a.h < b.h; })
            ->h;

    minPrice =
        std::min_element(candles.begin(), candles.end(),
                         [](const Candle &a, const Candle &b) { return a.l < b.l; })
            ->l;
}
