<!-- slide: class: title-slide -->

# Planche 3bis: WiFi Kit 32 - Connexions I2S

## Microphone ICS-43434 & DAC PCM5102A

---

<!-- slide: class: content-slide -->

## Composants et images

| Péripherique | Description |
|--------------|-------------|
| **WiFi Kit 32** | Carte de developpement ESP32 Heltec |
| **ICS-43434** | Microphone I2S MEMS |
| **PCM5102A** | DAC audio I2S |

![Wifikit32Pinout](images/Wifikit32Pinout.png){width=350px}

---

<!-- slide: class: content-slide -->

## Schema de connexion I2S

```mermaid
flowchart TB
    subgraph "WiFi Kit 32 (ESP32)"
        direction TB
        GPIO22[GPIO 22<br/>I2S_SERIAL_CLOCK]
        GPIO23[GPIO 23<br/>I2S_WS_LEFT_RIGHT_CLOCK]
        GPIO25[GPIO 25<br/>I2S_SERIAL_DATA_OUT]
        GPIO26[GPIO 26<br/>I2S_SERIAL_DATA_IN]
    end

    subgraph "Peripheriques I2S"
        subgraph "DAC PCM5102A"
            BCK[BCK<br/>Bit Clock]
            DIN[DIN<br/>Data In]
            LCK[LCK<br/>Left/Right Clock]
            OUT[OUT L/R<br/>Audio Output]
        end
        
        subgraph "Microphone ICS-43434"
            MCK[MCK<br/>Master Clock]
            WSP[WS<br/>Word Select]
            SD[SD<br/>Serial Data]
            GND[GND]
        end
    end

    GPIO22 --> BCK
    GPIO22 --> MCK
    GPIO23 --> LCK
    GPIO23 --> WSP
    GPIO25 --> DIN
    GPIO26 --> SD

    style GPIO22 fill:#3B82F6,color:white
    style GPIO23 fill:#3B82F6,color:white
    style GPIO25 fill:#3B82F6,color:white
    style GPIO26 fill:#3B82F6,color:white
    style BCK fill:#10B981,color:white
    style DIN fill:#10B981,color:white
    style LCK fill:#10B981,color:white
    style MCK fill:#F59E0B,color:black
    style WSP fill:#F59E0B,color:black
    style SD fill:#F59E0B,color:black
```

---

<!-- slide: class: content-slide -->

## Tableau des connexions detaillees

| Signal | GPIO | Fonction |
|--------|------|----------|
| **I2S_SERIAL_CLOCK** | GPIO_NUM_22 | Horloge serie (BCK/SCK) |
| **I2S_WS_LEFT_RIGHT_CLOCK** | GPIO_NUM_23 | Selection canal G/D (LCK/WS) |
| **I2S_SERIAL_DATA_OUT_PCM5102A** | GPIO_NUM_25 | Donnees vers DAC (DIN) |
| **I2S_SERIAL_DATA_IN_ICS-43434** | GPIO_NUM_26 | Donnees depuis microphone (SD) |

### Specifications techniques

- **Microphone ICS-43434**: MEMS I2S, SNR 65dB, plage -26dBFS a -26dBFS
- **DAC PCM5102A**: SNR 112dB,THD+N 0.003%, sortie ligne 2.1VRMS
- **Format I2S**: Master mode (ESP32 genere les horloges)

---

<!-- slide: class: content-slide -->

## Cablage I2S

<div>
  <img src="images/CarteExperimentationI2S_recto.jpg" width="200">
  <img src="images/CarteExperimentationI2S_verso.jpg" width="200">
</div>

---

<!-- slide: class: content-slide -->

## Module PCM5102A - Cablage

### Connexions ESP32 vers PCM5102A

| PCM5102A | ESP32 GPIO | Description |
|----------|------------|-------------|
| VIN | 3V3 | Alimentation 3.3V/5V |
| GND | GND | Masse |
| LRCK | GPIO 22 | Word Select (WS) |
| BCK | GPIO 23 | Bit Clock (BCK) |
| DIN | GPIO 25 | Data In |
| SCK | GND | (bridger si pas d'horloge externe) |

### Pins de fonction (bridges H1L-H4L)

| Bridge | Fonction | Position | Description |
|--------|----------|----------|-------------|
| **H1L/FLT** | Filter select | LOW (defaut) | Filtre normal latence ~500us |
| **H2L/DEMP** | De-emphasis | LOW | OFF (recommandé pour44.1kHz) |
| **H3L/XSMT** | Soft mute | HIGH | Unmute (recommandé) |
| **H4L/FMT** | Format audio | LOW | I2S/Right-justified |

### Schema de cablage

```mermaid
flowchart LR
    subgraph "ESP32"
        V3[3.3V]
        G[GND]
        GPIO22[GPIO 22<br/>LRCK]
        GPIO23[GPIO 23<br/>BCK]
        GPIO25[GPIO 25<br/>DIN]
    end

    subgraph "PCM5102A"
        VIN[VIN]
        GND[GND]
        LCK[LRCK]
        BCK[BCK]
        DIN[DIN]
        SCK[SCK]
        LOUT[L OUT]
        ROUT[R OUT]
    end

    V3 --> VIN
    G --> GND
    GPIO22 --> LCK
    GPIO23 --> BCK
    GPIO25 --> DIN
    G -.-> SCK

    style V3 fill:#EF4444,color:white
    style GPIO22 fill:#3B82F6,color:white
    style GPIO23 fill:#3B82F6,color:white
    style GPIO25 fill:#3B82F6,color:white
    style VIN fill:#10B981,color:white
    style LCK fill:#10B981,color:white
    style BCK fill:#10B981,color:white
    style DIN fill:#10B981,color:white
```

### Image du module PCM5102A

![Module PCM5102A](https://www.makerguides.com/wp-content/uploads/2026/02/image-12.png){width=400px}

