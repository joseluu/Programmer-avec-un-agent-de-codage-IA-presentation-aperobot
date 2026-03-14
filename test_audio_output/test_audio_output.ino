#include <Wire.h>
#include <SSD1306Wire.h>
#include <driver/i2s.h>
#include <math.h>

// OLED pins (Heltec WiFi Kit 32)
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16

// I2S TX config
#define I2S_BCK    22
#define I2S_WS     23
#define I2S_DOUT   25

#define SAMPLE_RATE 44100
#define FREQ        400
#define AMPLITUDE   16000   // ~50% du max 16-bit

SSD1306Wire display(0x3c, OLED_SDA, OLED_SCL, GEOMETRY_128_64);

const i2s_port_t I2S_TX = I2S_NUM_0;

// Buffer de samples pré-calculés pour un cycle complet
#define SAMPLES_PER_CYCLE (SAMPLE_RATE / FREQ)  // 110 samples à 44100/400
int16_t sineTable[SAMPLES_PER_CYCLE];

void setup() {
  Serial.begin(115200);
  Serial.println("Test sortie audio I2S - 400Hz");

  // Pré-calcul d'un cycle de sinus
  for (int i = 0; i < SAMPLES_PER_CYCLE; i++) {
    sineTable[i] = (int16_t)(sin(2.0 * PI * i / SAMPLES_PER_CYCLE) * AMPLITUDE);
  }

  // Init OLED
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(50);
  digitalWrite(OLED_RST, HIGH);
  delay(50);

  display.init();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.clear();
  display.drawString(64, 10, "Audio OUT");
  display.drawString(64, 30, "400 Hz");
  display.display();

  // Config I2S TX
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 256,
    .use_apll = true,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_DOUT,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  esp_err_t err;
  err = i2s_driver_install(I2S_TX, &i2s_config, 0, NULL);
  Serial.printf("i2s_driver_install: %s\n", esp_err_to_name(err));

  err = i2s_set_pin(I2S_TX, &pin_config);
  Serial.printf("i2s_set_pin: %s\n", esp_err_to_name(err));

  Serial.println("I2S TX ready, playing 400Hz sine wave");
}

void loop() {
  // Envoie un cycle complet de sinus (stéréo = même sample L+R)
  int16_t buf[SAMPLES_PER_CYCLE * 2];  // stereo
  for (int i = 0; i < SAMPLES_PER_CYCLE; i++) {
    buf[i * 2]     = sineTable[i];  // Left
    buf[i * 2 + 1] = sineTable[i];  // Right
  }

  size_t bytes_written;
  i2s_write(I2S_TX, buf, sizeof(buf), &bytes_written, portMAX_DELAY);
}
