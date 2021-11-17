#ifndef BTC_TICKER_CONFIG_H_
#define BTC_TICKER_CONFIG_H_

// =================  USER CONFIG ========================================================
// #define DEBUG_TICKER

#define WIFI_SSID "<SSID>"
#define WIFI_PASSWORD "<PASSWORD>"

// valid intervals: 1m,3m,5m,15m,30m,1h,2h,4h,6h,8h,12h,1d,3d,1w,1M
#define CANDLE_INTERVAL_PERIOD "30"
#define CANDLE_INTERVAL_UNIT "m"

// =================  DO NOT CHANGE  =====================================================
#if defined(DISPLAY_SSD1306)
    #define OLED_SCREEN_WIDTH 128
    #define OLED_SCREEN_HEIGHT 64

    #define PANEL_CANDLE_TOP 0
    #define PANEL_CANDLE_BOTTOM 46
    #define PANEL_PRICES_TOP (PANEL_CANDLE_BOTTOM + 2)
    #define PANEL_PRICES_BOTTOM OLED_SCREEN_HEIGHT
    #define PANEL_PRICES_HIGHLOW_X 70
    #define PANEL_PRICES_INTERVAL_X 115

    #define TEXTSIZE_CURRENT 2
    #define TEXTSIZE_HIGHLOW 1
    #define TEXTSIZE_CANDLE_INTERVAL 1

    #define CANDLE_WIDTH 5
    #define CANDLE_SPACING 2

#elif defined(DISPLAY_TFTeSPI)
    #define OLED_SCREEN_WIDTH 240
    #define OLED_SCREEN_HEIGHT 135

    #define PANEL_CANDLE_TOP 0
    #define PANEL_CANDLE_BOTTOM 76
    #define PANEL_PRICES_TOP (PANEL_CANDLE_BOTTOM + 5)
    #define PANEL_PRICES_BOTTOM OLED_SCREEN_HEIGHT
    #define PANEL_PRICES_HIGHLOW_X 150
    #define PANEL_PRICES_INTERVAL_Y (PANEL_PRICES_BOTTOM - 10)

    #define TEXTSIZE_CURRENT 4
    #define TEXTSIZE_HIGHLOW 2
    #define TEXTSIZE_CANDLE_INTERVAL 1

    #define CANDLE_WIDTH 7
    #define CANDLE_SPACING 2

#else
    #error "Select a Display"
#endif

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define CANDLES_MAX MIN(25, OLED_SCREEN_WIDTH / (CANDLE_WIDTH + CANDLE_SPACING))

#define CANDLE_INTERVAL (CANDLE_INTERVAL_PERIOD CANDLE_INTERVAL_UNIT)

#define BINANCE_REST_SERVER_NAME "api.binance.com"
#define BINANCE_REST_SERVER_PORT 443
#define BINANCE_REST_ENDPOINT                                                            \
    (String("/api/v3/klines?symbol=BTCUSDT&interval=") + String(CANDLE_INTERVAL) +       \
     String("&limit=") + String(CANDLES_MAX))
#define BINANCE_WEBSOCKET_URL                                                            \
    (String("wss://stream.binance.com:9443/ws/btcusdt@kline_") + String(CANDLE_INTERVAL))

#define JSON_BUFFER_SIZE 8192

#define SUCCESS 0
#define ERROR_HTTP -0x01
#define ERROR_JSON_DESERIALIZATION -0x02
#define ERROR_WEBSOCKET -0x03
#define ERROR_CANDLE_MISSING -0x04

#ifdef DEBUG_TICKER
    #define UART_SPEED 115200
    #define SERIAL_PRINTF(f_, ...) Serial.printf((f_), ##__VA_ARGS__)
    #define SERIAL_PRINTLN(x) Serial.println(x)
#else
    #define SERIAL_PRINTF(f_, ...)
    #define SERIAL_PRINTLN(x)
#endif

#endif // BTC_TICKER_CONFIG_H_
