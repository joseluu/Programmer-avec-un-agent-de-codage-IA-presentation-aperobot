#include <Wire.h>
#include <SSD1306Wire.h>
#include <driver/i2s.h>
#include <math.h>

#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16

#define SAMPLE_RATE 44100
#define WINDOW_SIZE 256

#define FREQ 400
#define ACTIVE_DURATION 100
#define SILENCE_DURATION 400

SSD1306Wire display(0x3c, OLED_SDA, OLED_SCL, GEOMETRY_128_64);

const i2s_port_t I2S_RX = I2S_NUM_1;
const i2s_port_t I2S_TX = I2S_NUM_0;

float window[WINDOW_SIZE];
float fftReal[WINDOW_SIZE];
float fftImag[WINDOW_SIZE];
float spectrum[WINDOW_SIZE / 2];

bool soundActive = false;
unsigned long lastToggle = 0;
int32_t audioPhase = 0;

void setup() {
  Serial.begin(115200);
  
  for (int i = 0; i < WINDOW_SIZE; i++) {
    window[i] = 0.5 - 0.5 * cos(2 * PI * i / (WINDOW_SIZE - 1));
  }
  
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(50);
  digitalWrite(OLED_RST, HIGH);
  delay(50);
  
  display.init();
  display.setFont(ArialMT_Plain_10);
  
  i2s_config_t i2s_rx_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };
  
  i2s_pin_config_t pin_rx_config = {
    .bck_io_num = 22,
    .ws_io_num = 23,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = 26
  };
  
  i2s_driver_install(I2S_RX, &i2s_rx_config, 0, NULL);
  i2s_set_pin(I2S_RX, &pin_rx_config);
  
  i2s_config_t i2s_tx_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 2,
    .dma_buf_len = 256,
    .use_apll = true,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };
  
  i2s_pin_config_t pin_tx_config = {
    .bck_io_num = 22,
    .ws_io_num = 23,
    .data_out_num = 25,
    .data_in_num = I2S_PIN_NO_CHANGE
  };
  
  i2s_driver_install(I2S_TX, &i2s_tx_config, 0, NULL);
  i2s_set_pin(I2S_TX, &pin_tx_config);
  
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "Test audio I2S");
  display.drawString(0, 12, "400Hz + microphone");
  display.display();
  delay(1000);
}

float rmsToDb(float rms) {
  if (rms <= 0) return -100;
  return 20.0 * log10(rms);
}

void applyWindow(float* data, const float* win, int size) {
  for (int i = 0; i < size; i++) {
    data[i] *= win[i];
  }
}

void computeFFT(float* real, float* imag, int size) {
  int n = size;
  int m = log2(n);
  
  for (int i = 0; i < n; i++) {
    imag[i] = 0;
  }
  
  for (int i = 0; i < n; i++) {
    int j = 0;
    int k = i;
    for (int l = 0; l < m; l++) {
      j = (j << 1) | (k & 1);
      k >>= 1;
    }
    if (j > i) {
      float temp = real[i];
      real[i] = real[j];
      real[j] = temp;
    }
  }
  
  for (int s = 1; s <= m; s++) {
    int m2 = 1 << s;
    int m1 = m2 >> 1;
    float theta = -PI / m1;
    float wr = 1, wi = 0;
    float wmr = cos(theta), wmi = sin(theta);
    
    for (int j = 0; j < m1; j++) {
      for (int k = j; k < n; k += m2) {
        int k1 = k + m1;
        float tr = wr * real[k1] - wi * imag[k1];
        float ti = wr * imag[k1] + wi * real[k1];
        real[k1] = real[k] - tr;
        imag[k1] = imag[k] - ti;
        real[k] += tr;
        imag[k] += ti;
      }
      float wnr = wr;
      wr = wr * wmr - wi * wmi;
      wi = wnr * wmi + wi * wmr;
    }
  }
}

void computeSpectrum(const float* real, const float* imag, float* spec, int size) {
  for (int i = 0; i < size / 2; i++) {
    float magnitude = sqrt(real[i] * real[i] + imag[i] * imag[i]);
    spec[i] = magnitude / size;
  }
}

void drawLevelBar(int x, int y, float db) {
  float minDb = -80.0;
  float maxDb = 0.0;
  
  if (db < minDb) db = minDb;
  if (db > maxDb) db = maxDb;
  
  int barWidth = map(db * 100, minDb * 100, maxDb * 100, 0, 100);
  
  display.drawRect(x, y, 100, 8);
  display.fillRect(x + 1, y + 1, barWidth, 6);
}

void drawSpectrum(int x, int y, int width, int height, const float* spec, int bins, float minDb, float maxDb) {
  int binWidth = width / bins;
  
  display.drawRect(x, y, width, height);
  
  for (int i = 0; i < bins; i++) {
    float magnitude = spec[i] * 10.0;
    float db = magnitude > 0 ? 20 * log10(magnitude) : minDb;
    if (db < minDb) db = minDb;
    if (db > maxDb) db = maxDb;
    
    int barHeight = map(db * 100, minDb * 100, maxDb * 100, 0, height - 2);
    
    display.fillRect(x + i * binWidth + 1, y + height - 1 - barHeight, binWidth - 2, barHeight);
  }
}

void loop() {
  unsigned long now = millis();
  
  if (soundActive) {
    if (now - lastToggle >= ACTIVE_DURATION) {
      soundActive = false;
      lastToggle = now;
    }
    
    float angle = 2.0 * PI * FREQ * audioPhase / SAMPLE_RATE;
    int16_t sample = (int16_t)(sin(angle) * 32760);
    audioPhase++;
    if (audioPhase >= SAMPLE_RATE / FREQ) {
      audioPhase = 0;
    }
    
    uint8_t buf[4];
    buf[0] = (sample >> 8) & 0xFF;
    buf[1] = sample & 0xFF;
    buf[2] = (sample >> 8) & 0xFF;
    buf[3] = sample & 0xFF;
    size_t bytes_written;
    i2s_write(I2S_TX, buf, 4, &bytes_written, portMAX_DELAY);
  } else {
    if (now - lastToggle >= SILENCE_DURATION) {
      soundActive = true;
      lastToggle = now;
    }
    uint8_t silence[4] = {0x80, 0x80, 0x80, 0x80};
    size_t bytes_written;
    i2s_write(I2S_TX, silence, 4, &bytes_written, portMAX_DELAY);
  }
  
  static int32_t rxBuffer[WINDOW_SIZE * 2];
  static size_t read_size = 0;
  
  i2s_read(I2S_RX, rxBuffer, sizeof(rxBuffer), &read_size, portMAX_DELAY);
  
  int samples = read_size / sizeof(int32_t);
  if (samples < WINDOW_SIZE) return;
  
  for (int i = 0; i < WINDOW_SIZE; i++) {
    int32_t sample = rxBuffer[i];
    int16_t left = (sample >> 16) & 0xFFFF;
    fftReal[i] = (float)left / 32768.0f;
  }
  
  applyWindow(fftReal, window, WINDOW_SIZE);
  
  computeFFT(fftReal, fftImag, WINDOW_SIZE);
  computeSpectrum(fftReal, fftImag, spectrum, WINDOW_SIZE);
  
  float sumL = 0;
  for (int i = 0; i < WINDOW_SIZE; i++) {
    sumL += fftReal[i] * fftReal[i];
  }
  float rmsL = sqrt(sumL / WINDOW_SIZE);
  float dbL = rmsToDb(rmsL);
  
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  
  if (soundActive) {
    display.drawString(0, 0, "monte le son");
  } else {
    display.drawString(0, 0, "silence");
  }
  
  display.drawString(64, 0, "G: " + String(dbL, 0) + " dB");
  drawLevelBar(64, 12, dbL);
  
  int spectrumBins = 32;
  drawSpectrum(0, 26, 128, 36, spectrum, spectrumBins, -80.0, -20.0);
  
  display.display();
  
  delay(10);
}
