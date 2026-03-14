## 0. test affichage
Afficher "Hello" sur l'afficheur oled de l'ESP32
## 1. test sortie audio
Sortir un son d'une frequence de 400Hz sur le periferique I2S audio, 100ms active, 400ms silence et ainsi de suite
Sur l'affichage, lorsque le son est actif afficher "monte le son", lorsque le son est inactive, afficher "silence"
## 2. test entrée audio
Un microphone de type ICS-43434 est connecté à l'entrée I2S, mesurer le niveau d'entrée en dB pour chacun des canaux droite et gauche, toutes les 100ms et afficher les niveaux sur l'afficheur OLED
## 2.1 lisibilité niveau
Afficher aussi le niveau sous chaque canal sous forme d'une barre horizontale proportionelle au niveau entre -80dB et 0dB
## 2.2 spectre
Le micro est branche sur le canal G, ne plus tenir compte du canal D
Dans l'espace sous le graphe de niveau afficher le spectre du signal puissance en dB entre -80dB et -40dB
