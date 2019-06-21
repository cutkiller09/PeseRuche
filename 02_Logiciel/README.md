Rucher connecté
---------------------------------------------------------


<br>
Le Rucher connecté permet de controler à distance le poids d'un rucher par l'usage d'une appli mobile sur téléphone portable.

Le Rucher connecté est composé de plusieurs pese-ruche et d'un concentrateur de données

- Implements CAN V2.0B at up to 1 Mb/s
- SPI Interface up to 10 MHz
- Standard (11 bit) and extended (29 bit) data and remote frames
- Two receive buffers with prioritized message storage

## Installation du Rucher connecté

1. Télécharge le fichier ZIP depuis https://github.com/cutkiller09/PeseRuche.git
2. Pour le "Pese Ruche", double cliquez sur PeseRuche.ino et Téleversez le logiciel en sélectionnant le port USBX et "Arduino Nano"
3. Pour le "Concentrateur de données", double cliquez sur Concentrateur.ino et Téleversez le logiciel en sélectionnant le port USBX et "Arduino Nano" 
4. Sur l'Apple Store au le PlayStore, récupérez l'appli mobile "Apeez"



# Usage:



## 1. Initialization

Il est nécessaire d'installer et configurer le rucher connecté avant toute installation de ruche.
Pour cela les étapes suivantes devront être suivies
1. Chaque Pese Ruche doit être positionné sur son emplacement 
2. Le concentrateur de données doit être positionné sur un lieu dégagé permettant une bonne communication sans fil
3. Lorsque les Peses ruches et le concentrateur sont positionnés, ils doivent être connectés à la batterie à l'aide de la connectique fournie.
4. Une fois l'installation du rucher connecté, l'apiculteur lancera l'application mobile "Apeez" et notamment demandera l'initialisation du rucher sur l'application si c'est une premiere installation du rucher. 
5. Chaque ruche sera positionnée à son emplacement définitif en commencant par l'emplacement numéro #1 jusqu'à l'emplacement définitif. 


