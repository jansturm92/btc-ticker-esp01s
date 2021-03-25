#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoHttpClient.h>
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>
#include <ArduinoWebsockets.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <SPI.h>
#include <Wire.h>

#include "TickerDrawer.hpp"
#include "config.h"

#if !DEBUG_SERIAL
#define SERIAL_PRINTF(f_, ...)
#define SERIAL_PRINTLN(x)
#else
#define SERIAL_PRINTF(f_, ...)                                                           \
    do {                                                                                 \
        Serial.printf((f_), ##__VA_ARGS__);                                              \
    } while (0);
#define SERIAL_PRINTLN(x) Serial.println(x)
#endif
#define DISPLAY_PRINTF(f_, ...)                                                          \
    do {                                                                                 \
        SERIAL_PRINTF((f_), ##__VA_ARGS__);                                              \
        display.printf((f_), ##__VA_ARGS__);                                             \
        display.display();                                                               \
    } while (0);

#define DISPLAY_RESET                                                                    \
    do {                                                                                 \
        display.clearDisplay();                                                          \
        display.setCursor(0, 0);                                                         \
        display.setTextSize(1);                                                          \
    } while (0);

// SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(OLED_SCREEN_WIDTH, OLED_SCREEN_HEIGHT, &Wire, -1);
TickerDrawer tickerDrawer(display);

WiFiClientSecure wifiClient;
websockets::WebsocketsClient wsClient;
bool isWebsocketConnected = false;

StaticJsonDocument<JSON_BUFFER_SIZE> jsonDoc;

void onMessageCallback(websockets::WebsocketsMessage message) {

    SERIAL_PRINTLN(message.data());
    deserializeJson(jsonDoc, message.data());

    unsigned long long t_llu = jsonDoc["k"]["t"];
    unsigned long long T_llu = jsonDoc["k"]["T"];

    int ret = tickerDrawer.update({
        .t = (unsigned long)(t_llu >> 10),
        .T = (unsigned long)(T_llu >> 10),
        .l = jsonDoc["k"]["l"],
        .h = jsonDoc["k"]["h"],
        .o = jsonDoc["k"]["o"],
        .c = jsonDoc["k"]["c"],
        .v = jsonDoc["k"]["v"],
    });
    if (ret != SUCCESS) {
        wsClient.close();
    } else {
        display.display();
    }

    jsonDoc.clear();
}

void onEventsCallback(websockets::WebsocketsEvent event, String data) {
    if (event == websockets::WebsocketsEvent::ConnectionOpened) {
        SERIAL_PRINTLN("WS: connection opened");
        isWebsocketConnected = true;
    } else if (event == websockets::WebsocketsEvent::ConnectionClosed) {
        SERIAL_PRINTLN("WS: connection closed");
        isWebsocketConnected = false;
    }
}

void setupWifi() {

    DISPLAY_RESET;
    DISPLAY_PRINTF("Connecting to WLAN:\n SSID: %s\n PWD: %s\n", WIFI_SSID,
                   WIFI_PASSWORD);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (!WiFi.isConnected()) {
        delay(500);
        DISPLAY_PRINTF(".");
    }
    DISPLAY_PRINTF("\n--> success!\n");
    DISPLAY_PRINTF("IP: %s\n", WiFi.localIP().toString().c_str());
    wifiClient.setInsecure();
    delay(2000);
}

int fetchAllCandles() {

    DISPLAY_RESET;
    DISPLAY_PRINTF(
        "Fetching initial data ...\nServer: %s\n Interval: %s\n #candles: %d\n",
        BINANCE_REST_SERVER_NAME, CANDLE_INTERVAL, CANDLES_MAX);

    HttpClient httpClient =
        HttpClient(wifiClient, BINANCE_REST_SERVER_NAME, BINANCE_REST_SERVER_PORT);
    httpClient.get(BINANCE_REST_ENDPOINT);

    if (httpClient.responseStatusCode() != 200) {
        DISPLAY_PRINTF("ERROR: HTTP request\n");
        return ERROR_HTTP;
    }
    if (deserializeJson(jsonDoc, httpClient.responseBody())) {
        return ERROR_JSON_DESERIALIZATION;
    }

    JsonArray newCandles = jsonDoc.as<JsonArray>();
    tickerDrawer.deleteCandles();

    for (size_t i = 0; i < CANDLES_MAX; i++) {
        // Data format: [[OPENTIME, OPEN, HIGH, LOW, CLOSE, VOL, CLOSETIME, ...], ...]
        unsigned long long t_llu = newCandles[i][0];
        unsigned long long T_llu = newCandles[i][6];
        tickerDrawer.appendCandle({
            t : (unsigned long)(t_llu >> 10),
            T : (unsigned long)(T_llu >> 10),
            l : newCandles[i][3],
            h : newCandles[i][2],
            o : newCandles[i][1],
            c : newCandles[i][4],
            v : newCandles[i][5],
        });
        DISPLAY_PRINTF(".");
    }
    httpClient.stop();
    DISPLAY_PRINTF("\ndone!");
    jsonDoc.clear();
    delay(1000);
    return SUCCESS;
}

int connectWebsocket() {
    DISPLAY_RESET;
    DISPLAY_PRINTF("Connecting to websocket\n%s\n ...\n", BINANCE_WEBSOCKET_URL.c_str());
    wsClient.onMessage(onMessageCallback);
    wsClient.onEvent(onEventsCallback);
    return wsClient.connect(BINANCE_WEBSOCKET_URL) ? SUCCESS : ERROR_WEBSOCKET;
}

void printFullTicker() {
    display.clearDisplay();
    tickerDrawer.printAllCandles();
    tickerDrawer.printPrices();
    display.display();
}

void setup() {

#if DEBUG_SERIAL
    Serial.begin(UART_SPEED);
#endif
    Wire.begin(PIN_SDA, PIN_SCL);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        SERIAL_PRINTLN(F("SSD1306 allocation failed"));
        while (1)
            ;
    }

    display.setTextColor(WHITE);
    display.setTextSize(1);

    setupWifi();
}

void loop() {

    while (!WiFi.isConnected()) {
        delay(500);
        DISPLAY_RESET;
        DISPLAY_PRINTF("reconnecting WiFi\n");
    }

    if (!isWebsocketConnected && WiFi.isConnected() && fetchAllCandles() == SUCCESS) {
        connectWebsocket();
        printFullTicker();
    }
    wsClient.poll();
}
