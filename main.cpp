#include <ArduinoHttpClient.h>
#define ARDUINOJSON_USE_LONG_LONG 1
#include "TickerDrawer.hpp"
#include "config.h"
#include <ArduinoJson.h>
#include <ArduinoWebsockets.h>
#include <SPI.h>

// =============  PLATFORM  ======================================
#if TICKER_PLATFORM_ESP8266
    #include <ESP8266WiFi.h>
#elif TICKER_PLATFORM_ESP32
    #include <WiFi.h>
const char *ws_ssl_ca_cert PROGMEM =
    #include "ca_cert.txt"
    ;
#else
    #error Unknown platform
#endif
// =============  DISPLAY  =======================================
#ifdef DISPLAY_TFTeSPI
    #include "TFTeSPIDisplay.hpp"
TFTeSPIDisplay display;
#endif
#ifdef DISPLAY_SSD1306
    #include "SSD1306Display.hpp"
SSD1306Display display;
#endif
// ===============================================================

TickerDrawer tickerDrawer(display);

WiFiClientSecure wifiClient;
websockets::WebsocketsClient wsClient;
bool isWebsocketConnected = false;

StaticJsonDocument<JSON_BUFFER_SIZE> jsonDoc;

void onMessageCallback(const websockets::WebsocketsMessage &message) {

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
        display.show();
    }

    jsonDoc.clear();
}

void onEventsCallback(websockets::WebsocketsEvent event, const String &data) {
    if (event == websockets::WebsocketsEvent::ConnectionOpened) {
        SERIAL_PRINTLN("WS: connection opened");
        isWebsocketConnected = true;
    } else if (event == websockets::WebsocketsEvent::ConnectionClosed) {
        SERIAL_PRINTLN("WS: connection closed");
        isWebsocketConnected = false;
    }
}

void setupWifi() {

    display.clear();
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
    delay(2000);
}

int fetchAllCandles() {

    display.clear();
    DISPLAY_PRINTF(
        "Fetching initial data ...\nServer: %s\n Interval: %s\n #candles: %d\n",
        BINANCE_REST_SERVER_NAME, CANDLE_INTERVAL, CANDLES_MAX);

    wifiClient.setInsecure();
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
            .t = (unsigned long)(t_llu >> 10),
            .T = (unsigned long)(T_llu >> 10),
            .l = newCandles[i][3],
            .h = newCandles[i][2],
            .o = newCandles[i][1],
            .c = newCandles[i][4],
            .v = newCandles[i][5],
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
    display.clear();
    DISPLAY_PRINTF("Connecting to websocket\n%s\n ...\n", BINANCE_WEBSOCKET_URL.c_str());
    wsClient.onMessage(onMessageCallback);
    wsClient.onEvent(onEventsCallback);
    return wsClient.connect(BINANCE_WEBSOCKET_URL) ? SUCCESS : ERROR_WEBSOCKET;
}

void printFullTicker() {
    display.clear();
    tickerDrawer.printAllCandles();
    tickerDrawer.printPrices();
    display.show();
}

void setup() {

#ifdef DEBUG_TICKER
    Serial.begin(UART_SPEED);
#endif
    display.init();

    setupWifi();
#ifdef TICKER_PLATFORM_ESP32
    wsClient.setCACert(ws_ssl_ca_cert);
#endif
}

void loop() {

    while (!WiFi.isConnected()) {
        delay(500);
        display.clear();
        DISPLAY_PRINTF("reconnecting WiFi\n");
    }

    if (!isWebsocketConnected && WiFi.isConnected() && fetchAllCandles() == SUCCESS) {
        connectWebsocket();
        printFullTicker();
    }
    wsClient.poll();
}
