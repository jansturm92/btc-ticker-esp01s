#include "config.h"
#include <Adafruit_SSD1306.h>
#include <array>
#include <stddef.h>

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
    Adafruit_SSD1306 &display;
    std::vector<Candle> candles;
    int maxPrice, minPrice;
    String interval_amount;
    String interval;

    void printCandle(const size_t offset, const Candle &candle);
    void calculateMinMax();

  public:
    TickerDrawer(Adafruit_SSD1306 &display);

    void deleteCandles();
    int update(const Candle candle);
    void appendCandle(const Candle candle);
    void printAllCandles();
    void printPrices();
};