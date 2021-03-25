# Realtime Bitcoin Ticker


![](demo.gif)
(1,75x speed, flickering from camera recording)

## Features
- Displays current price and candle chart in **realtime** using websockets
- Tiny form factor with an **ESP8266 ESP-01S WiFi Module** and a **SSD1306 OLED** display
- Based on PlatformIO for dependency management, compilation and flashing of binaries
- Arduino framework


## Hardware requirements
- ESP8266 ESP-01S WiFi Module
- OLED Display SSD1306 128×64
- Programmer for the ESP8266 (I used a USB to Serial Converter)

<img src="dev_setup.jpg" width="500" height="300">

## Usage

### Pin connections
ESP8266 programming mode
<img src="https://content.instructables.com/ORIG/F35/436R/JQMOG3VK/F35436RJQMOG3VK.png" width="800" height="300">

ESP <--> OLED connections
<img src="https://2.bp.blogspot.com/-Vn_AjEvGujc/VyVjNw87AoI/AAAAAAAAAuU/1TB-xEz7KXsOKrQFRbpJBVZURAmVcBy-QCKgB/s1600/Conexion%2BESP8266%2B%2BOled%2Bdisplay.jpg" width="500" height="300">


image sources:
 - https://www.instructables.com/ESP-01-Module-Programming-Board/
 - https://pdacontrolen.com/esp8266-display-oled-i2c-client-irc/
### Compilation and Flashing

Make sure PlatformIO is installed (see also the [PlatformIO documentation](https://docs.platformio.org/en/latest/core/installation.html)):
```bash
$ pip install -U platformio
```
Set the WiFi parameters in `config.h`, then compile the project and create the firmware:
```bash
$ pio run
```
Put the ESP8266 in programming mode and flash the device with
```bash
$ pio run -t upload
```

## License
<p xmlns:dct="http://purl.org/dc/terms/" xmlns:vcard="http://www.w3.org/2001/vcard-rdf/3.0#">
  <a rel="license"
     href="http://creativecommons.org/publicdomain/zero/1.0/">
    <img src="https://licensebuttons.net/p/zero/1.0/88x31.png" style="border-style: none;" alt="CC0" />
  </a>
  <br />
  To the extent possible under law,
  <span resource="[_:publisher]" rel="dct:publisher">
    <span property="dct:title">Jan Sturm</span></span>
  has waived all copyright and related or neighboring rights to
  <span property="dct:title">BTC ticker</span>.
This work is published from:
<span property="vcard:Country" datatype="dct:ISO3166"
      content="DE" about="[_:publisher]">
  Germany</span>.
</p>
