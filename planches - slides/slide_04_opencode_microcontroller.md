<!-- slide: class: title-slide -->

# Développement Microcontrôleur avec OpenCode

## Bonnes pratiques et configuration


| Tool | Global file | Project file |
|-----|-------------|-------------|
| OpenCode | `~/.config/opencode/AGENTS.md` | `AGENTS.md` |
| Claude Code | `~/.claude/CLAUDE.md` | `CLAUDE.md` |
| Copilot (CLI / GitHub) | — | `.github/copilot-instructions.md` |
| Cursor | `~/.cursor/rules/` | `.cursorrules` or `.cursor/rules/*` |
| Gemini CLI | `~/.gemini/GEMINI.md` | `GEMINI.md` |
| Codex CLI | — | `AGENTS.md` |
| Aider | `~/.aider.conf.yml` | `CONVENTIONS.md` |

---

<!-- slide: class: content-slide -->

## Configuration OpenCode pour l'embarqué

### Fichier ~/.claude/CLAUDE.md  general

```
# Environment & Preferences

## System
- **OS:** Windows 11, x86-64
- **Shell:** Git Bash (`C:\Program Files\Git\bin`) — always use Unix shell syntax (forward slashes, /dev/null, etc.)

## Python
- 3 versions installed, latest is **Python 3.12**
- Location: `C:\Users\josel\AppData\Local\Programs\Python\` (sous-repertoires par version)

## Node.js
- Disponible via **nvm**, à initialiser d'abord : `source ~/setup-nvm.sh`

## Microcontrollers (Arduino / PlatformIO)
- **Arduino CLI:** set `ARDUINO_CLI_PATH="/c/Users/josel/AppData/Local/Programs/arduino-cli_1.4.1_Windows_64bit"` when needed
- **PlatformIO:** `~/.platformio/penv/bin/pio`

## GitHub
- **gh** CLI disponible et authentifié

## Conventions
- Prefer Git Bash commands and Unix-style paths throughout
- Documentation en Markdown avec diagrammes Mermaid
```

### Fichier CLAUDE.md par projet

```markdown
# Projet Firmware Microcontrôleur

## Hardware
- **MCU:** ESP32 sur carte **Heltec WiFi Kit 32** (`heltec_wifi_kit_32`)
- **Ecran:** OLED intégré à la carte Heltec

### I2S Audio (micro en entrée, DAC en sortie)
| Signal              | GPIO       |
|---------------------|------------|
| I2S_SERIAL_CLOCK    | GPIO_NUM_22 |
| I2S_WS_LEFT_RIGHT_CLOCK | GPIO_NUM_23 |
| I2S_SERIAL_DATA_OUT | GPIO_NUM_25 |
| I2S_SERIAL_DATA_IN  | GPIO_NUM_26 |

## Toolchain
- **Framework:** Arduino
- **Build:** Arduino CLI (`$ARDUINO_CLI_PATH`)
- Penser à exporter `ARDUINO_CLI_PATH` avant compilation
```

---

<!-- slide: class: content-slide -->

## Bonnes pratiques microcontroller

## organisation du projet

- repertoire docs contenant
  - a_faire.md  liste des actions a enrichier petit a petit, c'est la memoire des "intentions"
  - changements_faits.md   comptes rendus faits par l'IA, c'est la memoire des "actions"
  - sous repertoire technique -> mettre les documentations des composants et les plans des circuits

Pour lire la documentation: Visual studio et extension "Markdown Preview Enhanced" et CTRL-SHIFT-V



## Conseils pratiques

### ✅ Faire 
#### Contexte du projet
- Spécifier la famille de MCU (STM32F401, ESP32-S3, RP2040...)
  - Spécifier le MCU exact et la carte utilisée
- Installer et indiquer le framework (HAL, ESP-IDF, Arduino, FreeRTOS)
  - Les actions (compilation, upload) doivent pouvoir être réalisées depuis la ligne de commande (arduino-cli, pio...)
- Ne pas hesiter à donner des exemples ou pointer sur du code qui fonctionne (localement ou sur internet)

#### prompts
- initialiser git pour le projet et faire un commit à chaque étape
- se limiter à des changements incrementaux
- pour les changements importants demander un plan et demander de l'approuver avant de commencer
- Demander des explications sur le code généré
- Demander de générer une documentation:
-  Motivation du projet
-  Utilisation
-  description technique

---
<!-- slide: class: content-slide -->


#### Pour les projets complexes que l'on veut faire en une seule fois
Possibilité d'utiliser plusieurs IA afin de limiter les **débordements de contexte**

1. Premiere IA: supervise le developpement
Préparer une description en francais trés précise de tout ce qu'il y a a faire, donc texte long.
- (facultatif) On peut préciser qu'il peut poser des questions des qu'il y a une incertitude
  -  Inconvenient: toute la spécification ne sera pas dans la description du projet
- Lui demander un plan d'action pour une IA de codage -> PLAN.md
- Lui demander un plan de test et de realisation des tests -> TEST.md
- Relire et alterer les plans. 
  - Modifications de détails à la main
  - Modifications importantes: changer le texte de la description et regénérer le plan
- Lui faire générer de la documentation:
  - La motivation du projet
  - Le manuel utilisateur
  - L'architecture générale
- Relire la documentation, si problème modifier la description

2. Deuxième IA (le developpeur)
- Lui faire developper selon le plan
- Lui preciser qu'il faut passer les tests deja préparés (les IA de developpement font aussi des tests par eux même)
- Lui faire documenter chaque étape -> docs/changements_faits.md
- Lui faire faire un git commit à chaque étape
- Attention, ca va vite ... les bétises aussi



### Exemples de prompts: les phrases "magiques"

```
creer un fichier global pour cet utilisateur  ~/.claude/CLAUDE.md decrivant la maniere dont est installé ce systeme et ma manière
  preferée de travailler: C'est une machine x86-64 sous Windows 11, je travaille sous git-bash c'est une installaiton de git
  standard, avec git bach ici: C:\Program Files\Git\bin , il y a 3 versions de python dont la plus recente est la 3.12 installées
  dans des sous-repertoires: C:\Users\josel\AppData\Local\Programs\Python pour les developpements sur microcontrolleurs je mets
  lorsque necessaire la variable d'environement
  ARDUINO_CLI_PATH="/c/Users/josel/AppData/Local/Programs/arduino-cli_1.4.1_Windows_64bit", pour les developpement sur
  microcontrollers j'utilise aussi PlatformIO, l'executable pio est dans ~/.platformio/penv/bin/pio. 
  Nodejs est disponible via la commande nvm qui elle meme doit etre initialisée en faisant source  ~/setup-nvm.sh
  gh est disponible et deja parametré pour agir sur les dépots github de l'utilisateur
  la documentation doit etre generée en markdown + mermaid
```

```
Pour ce projet, creer un fichier CLAUDE.md decrivant le microcontrolleur utilisé: ESP32 sur une carte Heltec Wifikit
  (heltec_wifi_kit_32) possedant un afficheur oled 2 peripheriques I2S audio sont connectés: I2S_SERIAL_CLOCK GPIO_NUM_22;
  I2S_WS_LEFT_RIGHT_CLOCK GPIO_NUM_23; I2S_SERIAL_DATA_OUT        GPIO_NUM_25; I2S_SERIAL_DATA_IN GPIO_NUM_26, l'entrée est un
  microphone et la sortie une carte DAC, programmer en mode arduino et compiler avec arduino cli
```

```
sauve les informations qui si tu les avaient eu auraient evité les essais et erreurs, sauver dans HELTEC_WIFI_KIT_32.md
```

```
pour chaque action demandée et effectuée a partir des paragraphes du fichier docs/a_faire.md ajouter le compte
rendu de l'action dans le fichier docs/changements_faits.md
```




