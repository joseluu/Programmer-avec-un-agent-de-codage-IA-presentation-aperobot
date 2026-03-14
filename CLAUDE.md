# prez-aperobot-example

## Hardware
- **MCU:** ESP32 sur carte **Heltec WiFi Kit 32** (`heltec_wifi_kit_32`)
- **Ecran:** OLED intégré à la carte Heltec

### I2S Audio (micro en entrée, DAC en sortie)
| Signal              | GPIO       |
|---------------------|------------|
| I2S_SERIAL_CLOCK    | GPIO_NUM_22 |
| I2S_WS_LEFT_RIGHT_CLOCK | GPIO_NUM_23 |
| I2S_SERIAL_DATA_OUT (PCM5102A) | GPIO_NUM_25 |
| I2S_SERIAL_DATA_IN (ICS-43434)  | GPIO_NUM_26 |

- Le micro et le DAC partagent BCK/WS → utiliser **un seul port I2S en full duplex**
- Le micro ICS-43434 émet sur le **canal droit** (samples impairs en interleaved)

## Toolchain
- **Framework:** Arduino
- **Build:** Arduino CLI (`$ARDUINO_CLI_PATH`)
- Penser à exporter `ARDUINO_CLI_PATH` avant compilation
