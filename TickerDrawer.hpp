#ifndef BTC_TICKER_TICKERDRAWER_HPP
#define BTC_TICKER_TICKERDRAWER_HPP

#include "ITickerDisplay.hpp"
#include "config.h"
#include <cstddef>
#include <vector>

typedef struct {
    unsigned long t; // Kline start time
    unsigned long T; // Kline close time
    int l;           // Low price
    int h;           // High price
    int o;           // Open price
    int c;           // Close price
    float v;         // Base asset volume
} Candle;

class TickerDrawer {

  private:
    ITickerDisplay &display;
    std::vector<Candle> candles;
    int maxPrice, minPrice;

    void printCandle(size_t offset, const Candle &candle);
    void calculateMinMax();

  public:
    explicit TickerDrawer(ITickerDisplay &display);

    void deleteCandles();
    int update(Candle candle);
    void appendCandle(Candle candle);
    void printAllCandles();
    void printPrices();
};

#endif // BTC_TICKER_TICKERDRAWER_HPP