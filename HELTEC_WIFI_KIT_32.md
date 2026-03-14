# Heltec WiFi Kit 32 - Notes de developpement

## OLED SSD1306

### Cablage
| Signal  | GPIO |
|---------|------|
| OLED_SDA | 4    |
| OLED_SCL | 15   |
| OLED_RST | 16   |

### Code fonctionnel

```cpp
#include <Wire.h>
#include <SSD1306Wire.h>

#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16

SSD1306Wire display(0x3c, OLED_SDA, OLED_SCL, GEOMETRY_128_64);

void setup() {
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(50);
  digitalWrite(OLED_RST, HIGH);
  delay(50);
  
  display.init();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "Hello");
  display.display();
}

void loop() {
}
```

### Points importants

- **Bibliotheque:** `ESP8266 and ESP32 OLED driver for SSD1306 displays` (SSD1306Wire)
- **Adresse I2C:** 0x3c
- **Geometrie:** GEOMETRY_128_64 (128x64 pixels)
- **Ne pas utiliser** `heltec.h` avec le core esp32:esp32 - incompatibilite de version LoRaMac
- **Reset OLED:** Necessaire pour initialiser l'afficheur (GPIO 16)

## Compilation

```bash
export ARDUINO_CLI_PATH="/c/Users/josel/AppData/Local/Programs/arduino-cli_1.4.1_Windows_64bit"
$ARDUINO_CLI_PATH/arduino-cli compile -b esp32:esp32:heltec_wifi_kit_32 <sketch>
```

## Upload

```bash
$ARDUINO_CLI_PATH/arduino-cli upload -b esp32:esp32:heltec_wifi_kit_32 -p COM6 <sketch>
```

## Bibliotheques necessaires

- ESP8266 and ESP32 OLED driver for SSD1306 displays
- Adafruit GFX Library
- Adafruit BusIO
