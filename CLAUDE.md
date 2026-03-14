# prez-aperobot-example

## Hardware
- **MCU:** ESP32 sur carte **Heltec WiFi Kit 32** (`heltec_wifi_kit_32`)
- **Ecran:** OLED intégré à la carte Heltec

### I2S Audio (micro en entrée, DAC en sortie)
| Signal              | GPIO       |
|---------------------|------------|
| I2S_SERIAL_CLOCK    | GPIO_NUM_22 |
| I2S_WS_LEFT_RIGHT_CLOCK | GPIO_NUM_23 |
| I2S_SERIAL_DATA_OUT_PCM5102A | GPIO_NUM_25 |
| I2S_SERIAL_DATA_IN_ICS-43434  | GPIO_NUM_26 |

## Toolchain
- **Framework:** Arduino
- **Build:** Arduino CLI (`$ARDUINO_CLI_PATH`)
- Penser à exporter `ARDUINO_CLI_PATH` avant compilation
