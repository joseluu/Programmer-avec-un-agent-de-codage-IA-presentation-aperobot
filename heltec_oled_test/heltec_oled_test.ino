#include <Wire.h>
#include <SSD1306Wire.h>
#include <driver/i2s.h>
#include <math.h>

#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16

#define SAMPLE_RATE 32000
#define WINDOW_SIZE 256

#define FREQ 1000
#define ACTIVE_DURATION 20
#define SILENCE_DURATION 500

#define DETECT_THRESHOLD 0.005f  // seuil RMS pour détection du son reçu

SSD1306Wire display(0x3c, OLED_SDA, OLED_SCL, GEOMETRY_128_64);

// Single I2S port in full duplex
const i2s_port_t I2S_PORT = I2S_NUM_0;

float window[WINDOW_SIZE];
float fftReal[WINDOW_SIZE];
float fftImag[WINDOW_SIZE];
float spectrum[WINDOW_SIZE / 2];

volatile bool soundActive = false;
volatile unsigned long txStartMicros = 0;  // timestamp début émission (µs)
volatile bool txNewPulse = false;          // nouvelle impulsion émise, pas encore détectée

// Sine table (32-bit samples for full duplex compatibility)
#define SAMPLES_PER_CYCLE (SAMPLE_RATE / FREQ)
int32_t sineTable[SAMPLES_PER_CYCLE];

// TX audio task: runs on core 0
void audioTxTask(void* param) {
  int32_t buf[SAMPLES_PER_CYCLE * 2];  // stereo
  int32_t silence[SAMPLES_PER_CYCLE * 2];
  memset(silence, 0, sizeof(silence));

  for (int i = 0; i < SAMPLES_PER_CYCLE; i++) {
    buf[i * 2]     = sineTable[i];
    buf[i * 2 + 1] = sineTable[i];
  }

  unsigned long lastToggle = 0;
  bool active = false;

  for (;;) {
    unsigned long now = millis();
    if (active && now - lastToggle >= ACTIVE_DURATION) {
      active = false;
      lastToggle = now;
    } else if (!active && now - lastToggle >= SILENCE_DURATION) {
      active = true;
      lastToggle = now;
      txStartMicros = micros();
      txNewPulse = true;
    }
    soundActive = active;

    size_t bytes_written;
    if (active) {
      i2s_write(I2S_PORT, buf, sizeof(buf), &bytes_written, portMAX_DELAY);
    } else {
      i2s_write(I2S_PORT, silence, sizeof(silence), &bytes_written, portMAX_DELAY);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Test audio I2S full duplex");

  // Pre-compute sine table in 32-bit (upper 16 bits carry the signal)
  for (int i = 0; i < SAMPLES_PER_CYCLE; i++) {
    int16_t s = (int16_t)(sin(2.0 * PI * i / SAMPLES_PER_CYCLE) * 16000);
    sineTable[i] = ((int32_t)s) << 16;
  }

  // Hanning window
  for (int i = 0; i < WINDOW_SIZE; i++) {
    window[i] = 0.5 - 0.5 * cos(2 * PI * i / (WINDOW_SIZE - 1));
  }

  // OLED init
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(50);
  digitalWrite(OLED_RST, HIGH);
  delay(50);

  display.init();
  display.setFont(ArialMT_Plain_10);

  // Single I2S port: master, TX + RX (full duplex)
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
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
    .bck_io_num = 22,
    .ws_io_num = 23,
    .data_out_num = 25,
    .data_in_num = 26
  };

  esp_err_t err;
  err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  Serial.printf("i2s_driver_install: %s\n", esp_err_to_name(err));

  err = i2s_set_pin(I2S_PORT, &pin_config);
  Serial.printf("i2s_set_pin: %s\n", esp_err_to_name(err));

  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "Test audio I2S");
  display.drawString(0, 12, "400Hz + microphone");
  display.display();
  delay(1000);

  // Launch TX task on core 0 (loop runs on core 1)
  xTaskCreatePinnedToCore(audioTxTask, "audioTx", 4096, NULL, 1, NULL, 0);
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
  static int32_t rxBuffer[WINDOW_SIZE * 2];  // WINDOW_SIZE paires stéréo (L+R)
  static size_t read_size = 0;
  static bool wasQuiet = true;
  static int displayDelayMs = -1;

  i2s_read(I2S_PORT, rxBuffer, sizeof(rxBuffer), &read_size, portMAX_DELAY);
  unsigned long rxTimeMicros = micros();

  int samples = read_size / sizeof(int32_t);
  if (samples < WINDOW_SIZE) return;

  for (int i = 0; i < WINDOW_SIZE; i++) {
    int32_t sample = rxBuffer[i * 2 + 1];  // canal droit (ICS-43434)
    int16_t left = (sample >> 16) & 0xFFFF;
    fftReal[i] = (float)left / 32768.0f;
  }

  // Calcul RMS pour affichage niveau
  float sumL = 0;
  for (int i = 0; i < WINDOW_SIZE; i++) {
    sumL += fftReal[i] * fftReal[i];
  }
  float rmsL = sqrt(sumL / WINDOW_SIZE);
  float dbL = rmsToDb(rmsL);

  applyWindow(fftReal, window, WINDOW_SIZE);
  computeFFT(fftReal, fftImag, WINDOW_SIZE);
  computeSpectrum(fftReal, fftImag, spectrum, WINDOW_SIZE);

  // Nouvelle impulsion émise : reset de la détection
  if (txNewPulse) {
    txNewPulse = false;
    wasQuiet = true;
    displayDelayMs = -1;
  }

  // Détection du front montant par RMS
  if (wasQuiet && rmsL > DETECT_THRESHOLD && txStartMicros > 0) {
    unsigned long delayUs = rxTimeMicros - txStartMicros;
    displayDelayMs = delayUs / 1000;
    wasQuiet = false;
    Serial.printf("Delay: %d ms (rms=%.4f)\n", displayDelayMs, rmsL);
  } else if (rmsL < DETECT_THRESHOLD * 0.3f) {
    wasQuiet = true;
  }

  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  // Retard mesuré en haut à gauche
  if (displayDelayMs >= 0) {
    display.drawString(0, 0, String(displayDelayMs) + " ms");
  } else {
    display.drawString(0, 0, "-- ms");
  }

  // Indicateur son actif : carré 5px en haut à droite
  if (soundActive) {
    display.fillRect(123, 0, 5, 5);
  }

  display.drawString(64, 0, "G: " + String(dbL, 0) + " dB");
  drawLevelBar(64, 12, dbL);

  int spectrumBins = 32;
  drawSpectrum(0, 26, 128, 36, spectrum, spectrumBins, -80.0, -20.0);

  display.display();

  delay(10);
}
