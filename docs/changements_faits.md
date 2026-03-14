# Changements effectues

## 0. test affichage

**Tache:** Afficher "Hello" sur l'afficheur OLED de l'ESP32

**Actions:**
- Cree le sketch `heltec_oled_test/heltec_oled_test.ino`
- Premiere version avec bibliotheque `heltec.h` - echec (incompatible avec le core esp32:esp32)
-Deuxieme tentative avec bibliotheque SSD1306Wire - echec (probleme de compilation)
- Version fonctionnelle avec SSD1306Wire en utilisant les broches OLED specifique Heltec WiFi Kit 32:
  - OLED_SDA: GPIO 4
  - OLED_SCL: GPIO 15
  - OLED_RST: GPIO 16
- Upload compile et fonctionne

**Resultat:** OK - "Hello" s'affiche sur l'OLED

---

## 1. test sortie audio

**Tache:** Sortir un son de 400Hz sur le peripherique I2S audio, 100ms actif, 400ms silence + affichage

**Actions:**
- Modifie le sketch pour generer un signal sinus 400Hz
- Premiere tentative: DAC interne ESP32 - pas de son confirme
- Deuxieme tentative: I2S externe avec broches:
  - BCK: GPIO 22
  - WS: GPIO 23
  - DIN: GPIO 25
- Configuration I2S en mode MASTER | TX avec driver i2s.h
- Ajout de l'affichage "monte le son" / "silence" en fonction de l'etat

**Resultat:** Pas de son sur le peripherique I2S (PCM5102A) despite toutes les configurations essayees. L'affichage fonctionne correctement.

---

## 2. test entree audio

**Tache:** Mesurer le niveau d'entree en dB pour les canaux G et D toutes les 100ms

**Actions:**
- Reconfigure I2S en mode RX (entree)
- Configure les broches:
  - BCK: GPIO 22
  - WS: GPIO 23
  - DIN: GPIO 26
- Implementation de la lecture I2S avec buffer circulaire
- Calcul RMS et conversion en dB pour chaque canal
- Affichage toutes les 100ms

**Resultat:** Les niveaux s'affichent sur l'OLED

---

## 2.1 lisibilite niveau

**Tache:** Afficher le niveau sous forme de barre horizontale entre -80dB et 0dB

**Actions:**
- Ajout de la fonction `drawLevelBar()` pour afficher une barre horizontale
- Barres pour les canaux G et D
- Echelle: -80dB a 0dB

**Resultat:** Les barres de niveau s'affichent

---

## 2.2 spectre

**Tache:** Afficher le spectre du signal uniquement sur le canal G, entre -80dB et -40dB

**Actions:**
- Implementation d'un FFT (Radix-2) sur 256 echantillons
- Generation de fenetre de Hanning pour reduire le leakage spectral
- Affichage du spectre sur 32 bins
- Resolution frequentielle: ~172 Hz/bin (44100/256)
- Affichage uniquement canal G (canal D ignore)
- Ajout d'un gain x10 pour visibilite
- Echelle ajustee: -80dB a -20dB

**Resultat:** Le spectre s'affiche sous forme de barres verticales

---

## Notes techniques

### Configuration I2S Heltec WiFi Kit 32

| Signal    | GPIO  |
|-----------|-------|
| I2S_SCK   | 22    |
| I2S_WS    | 23    |
| I2S_DOUT  | 25    |
| I2S_DIN   | 26    |

### Bibliotheques utilisees

- ESP8266 and ESP32 OLED driver for SSD1306 displays
- driver/i2s (inclus ESP32 Arduino core)

### Problemes rencontres

- Incompatibilite entre heltec.h et esp32:esp32 core
- Pas de son sur sortie I2S externe (PCM5102A) - cause non identifiee
