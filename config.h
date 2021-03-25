#define DEBUG_SERIAL 0

#if DEBUG_SERIAL
#define UART_SPEED 115200
#endif
#define PIN_SDA 2
#define PIN_SCL 0
#define OLED_SCREEN_WIDTH 128
#define OLED_SCREEN_HEIGHT 64

#define WIFI_SSID "<SSID>"
#define WIFI_PASSWORD "<PASSWORD>"

#define PANEL_CANDLE_TOP 0
#define PANEL_CANDLE_BOTTOM 46
#define PANEL_PRICES_TOP (PANEL_CANDLE_BOTTOM + 2)
#define PANEL_PRICES_BOTTOM OLED_SCREEN_HEIGHT
#define PANEL_PRICES_HIGHLOW_X 70
#define PANEL_PRICES_INTERVAL_X 115

#define CANDLE_INTERVAL "1m"
#define CANDLE_WIDTH 5
#define CANDLE_SPACING 2
#define CANDLES_MAX (OLED_SCREEN_WIDTH / (CANDLE_WIDTH + CANDLE_SPACING))

#define BINANCE_REST_SERVER_NAME "api.binance.com"
#define BINANCE_REST_SERVER_PORT 443
#define BINANCE_REST_ENDPOINT                                                            \
    (String("/api/v3/klines?symbol=BTCUSDT") + String("&interval=") +                    \
     String(CANDLE_INTERVAL) + String("&limit=") + String(CANDLES_MAX))
#define BINANCE_WEBSOCKET_URL                                                            \
    (String("wss://stream.binance.com:9443/ws/btcusdt@kline_") + String(CANDLE_INTERVAL))

#define JSON_BUFFER_SIZE 8192

#define SUCCESS 0
#define ERROR_HTTP -0x01
#define ERROR_JSON_DESERIALIZATION -0x02
#define ERROR_WEBSOCKET -0x03
#define ERROR_CANDLE_MISSING -0x04
