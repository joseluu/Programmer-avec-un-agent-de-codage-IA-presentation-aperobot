# Traitement audio I2S sur ESP32 — Notes techniques

Retour d'expérience sur les pièges rencontrés lors de l'implémentation de l'audio I2S
full duplex (entrée micro + sortie DAC) sur ESP32 Heltec WiFi Kit 32.

## 1. Full duplex : un seul port I2S

**Problème :** L'ESP32 possède deux périphériques I2S (I2S_NUM_0 et I2S_NUM_1).
Si le micro (RX) et le DAC (TX) partagent les mêmes lignes BCK et WS (GPIO 22/23),
on ne peut **pas** utiliser deux ports I2S séparés, même en configurant l'un en master
et l'autre en slave. La matrice GPIO ne permet qu'un seul périphérique par broche de sortie.

**Solution :** Utiliser **un seul port I2S en mode full duplex** :

```c
.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX)
```

Configurer les 4 broches (BCK, WS, DOUT, DIN) sur ce même port.

**Conséquence :** TX et RX doivent utiliser le **même format de sample**
(même `bits_per_sample`). Le micro ICS-43434 fournit du 32-bit et le DAC PCM5102A
accepte du 16, 24 ou 32-bit → utiliser **32-bit pour les deux** et placer le signal
audio dans les 16 bits de poids fort :

```c
sineTable[i] = ((int32_t)sample_16bit) << 16;
```

## 2. Génération audio dans un task FreeRTOS séparé

**Problème :** `i2s_write()` et `i2s_read()` avec `portMAX_DELAY` sont bloquants.
Les appeler séquentiellement dans `loop()` crée des coupures audio et ralentit
la lecture du micro.

**Solution :** Dédier un **task FreeRTOS sur le core 0** à la génération audio TX.
Le `loop()` Arduino (core 1) se consacre à la lecture RX, au traitement et à l'affichage.

```c
xTaskCreatePinnedToCore(audioTxTask, "audioTx", 4096, NULL, 1, NULL, 0);
```

## 3. Génération de sinus par table pré-calculée

Écrire un sample à la fois via `i2s_write()` est beaucoup trop lent.

**Solution :** Pré-calculer un cycle complet de sinus dans un tableau, puis écrire
le tableau entier (stéréo, L+R identiques) en un seul appel `i2s_write()`.

```c
#define SAMPLES_PER_CYCLE (SAMPLE_RATE / FREQ)
int32_t sineTable[SAMPLES_PER_CYCLE];
// Remplir un buffer stéréo et l'envoyer d'un bloc
i2s_write(I2S_PORT, buf, sizeof(buf), &bytes_written, portMAX_DELAY);
```

## 4. DMA buffers

Pour la sortie audio, des buffers DMA trop petits causent des coupures.
Configuration qui fonctionne :

```c
.dma_buf_count = 8,
.dma_buf_len = 256,
```

Activer `use_apll = true` pour une horloge plus précise et
`tx_desc_auto_clear = true` pour injecter du silence automatiquement
si le buffer TX n'est pas rempli à temps.

## 5. Canal du microphone ICS-43434

Le buffer I2S est interleaved stéréo : L, R, L, R…
Le microphone ICS-43434 émet sur le **canal droit** (index impairs).
Pour extraire uniquement le canal du micro :

```c
int32_t sample = rxBuffer[i * 2 + 1];  // canal droit
```

**Piège :** lire `rxBuffer[i]` séquentiellement mélange L et R, ce qui donne
un signal dilué (RMS divisé par ~√2) et un spectre FFT inexploitable sur un bin
précis. Toujours séparer les canaux avant traitement.

## 6. Mesure du retard émission → réception

### Latence observée : ~120 ms

Sources de latence cumulées :
- **DMA TX** : 8 buffers × 256 samples × 2 canaux = ~32 ms à 32 kHz
- **DMA RX** : 8 buffers × 256 samples × 2 canaux = ~32 ms
- **Lecture par blocs** : `i2s_read()` lit WINDOW_SIZE×2 int32 (256 paires stéréo)
  = 256/32000 = 8 ms par bloc
- **Propagation acoustique** : négligeable à courte distance (~0.3 ms/10 cm)
- **Traitement** (FFT, affichage OLED) : quelques ms supplémentaires

### Méthode de détection

La détection par **RMS broadband** est plus fiable que la détection par bin FFT,
car les données lues depuis le buffer I2S sont interleaved stéréo (L, R, L, R…)
et le code extrait les samples sans séparer proprement les canaux,
ce qui dilue l'énergie d'un bin FFT spécifique.

Le seuil RMS de **0.005** fonctionne bien avec un bruit de fond typique de 0.0002–0.0003.

## 7. Choix du sample rate

Le sample rate de **32000 Hz** (au lieu de 44100) a été choisi pour que
1000 Hz tombe exactement sur le bin 8 de la FFT (résolution = 125 Hz/bin).
Cela n'a finalement pas été exploité pour la détection (RMS utilisé à la place),
mais le spectre affiché est plus lisible avec des bins alignés.
