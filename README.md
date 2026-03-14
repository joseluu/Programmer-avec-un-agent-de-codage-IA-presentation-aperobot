# prez-aperobot-example

Projet support d'un exposé sur la programmation assistée par une IA de codage.

## Contenu

- **Planches de présentation** — les slides utilisées lors de l'exposé
- **Projet fonctionnel** — le code embarqué réalisé pendant la démonstration (ESP32 / Heltec WiFi Kit 32)
- **Historique des prompts** — les prompts utilisés pour guider l'IA tout au long du développement

## Hardware

- **Carte :** Heltec WiFi Kit 32 (ESP32 + OLED 128×64 intégré)
- **Sortie audio :** DAC PCM5102A via I2S (GPIO 25)
- **Entrée audio :** Microphone ICS-43434 via I2S (GPIO 26)
- **Horloge I2S partagée :** BCK = GPIO 22, WS = GPIO 23

## Projet principal (`heltec_oled_test/`)

Sketch Arduino qui fait tourner simultanément :

- **Sortie audio** : bip 1000 Hz de 20 ms toutes les 500 ms (task FreeRTOS sur core 0)
- **Entrée audio** : lecture micro, calcul RMS et FFT (loop sur core 1)
- **Affichage OLED** : retard émission→réception en ms, niveau en dB, barre de niveau, spectre
- **Indicateur visuel** : carré 5×5 px en haut à droite pendant l'émission du bip

L'audio utilise un seul port I2S en **full duplex** (TX + RX sur I2S_NUM_0) car
le micro et le DAC partagent les mêmes lignes d'horloge.

## Compléments

- `test_audio_output/` — test isolé de la sortie audio I2S (sinus 400 Hz continu), ajouté après la présentation
- `docs/TRAITEMENT_AUDIO.md` — notes techniques sur les pièges de l'audio I2S full duplex sur ESP32
