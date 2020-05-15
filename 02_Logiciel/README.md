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


## 2. Configuration

## 2. How to calibrate your load cell

    Call set_scale() with no parameter.
    Call tare() with no parameter.
    Place a known weight on the scale and call get_units(10).
    Divide the result in step 3 to your known weight. You should get about the parameter you need to pass to set_scale().
    Adjust the parameter in step 4 until you get an accurate reading.


            Poids reel      Hx711 (cal)             a                               b
rien        0 kg            offset = -38 
            7kg             118.6           44.7 = 7000/ (118.6 +38)  1698.6 = 7000 - 44.7*118.6
Baptiste : 19.4 kg          -830000
Nonore : 13.5 kg            


            7 kg            -510            539 000




    Mesure du décalage :
        Relever la valeur à vide (rien sur le plateau)
        Tourner le potentiomètre jusqu'à obtenir la valeur la plus faible possible
        Cette valeur s'appelera offset : offset=sensorValue pour m = 0g
    Calibrage :
        Placer sur le plateau une masse connue, par exemple 5 kG.
        Relever la valeur en charge
        Cette valeur s'appellera cal : cal=sensorValue pour m = 5000g
    Calcul de la fonction de transfert m(g) = f(sensorValue)
        Cette fonction de transfert est une droite : m(g) = a.sensorValue + b
        a = 5000 / (cal - offset)
        b = 5000 - a.cal


         y = mx + b
         y = mx -  510

         m = (7000 + 510) / 539000
         m = 0,01393321



         get units: 		-553.0
masse: 		-0.0
get units: 		-553.0
masse: 		-0.0
get units: 		-559.0
masse: 		-0.1
get units: 		-639.0
masse: 		-1.2
get units: 		959.0
masse: 		19.6
get units: 		31639.0
masse: 		418.0
get units: 		159110.0
masse: 		2073.5
get units: 		269240.0
masse: 		3503.8
get units: 		269449.0
masse: 		3506.5
get units: 		269421.0
masse: 		3506.1
get units: 		269459.0
masse: 		3506.6
get units: 		269399.0
masse: 		3505.8
get units: 		321004.0
masse: 		4176.0
get units: 		528700.0
masse: 		6873.4
get units: 		538771.0
masse: 		7004.2
get units: 		538880.0
masse: 		7005.6
get units: 		538864.0
masse: 		7005.4
get units: 		538847.0
masse: 		7005.2
get units: 		538833.0
masse: 		7005.0
get units: 		538873.0
masse: 		7005.5
get units: 		538863.0
masse: 		7005.4
get units: 		538820.0
masse: 		7004.8
get units: 		538832.0
masse: 		7005.0
get units: 		538847.0
masse: 		7005.2
get units: 		539205.0
masse: 		7009.8
get units: 		539615.0
masse: 		7015.1
get units: 		538826.0
masse: 		7004.9
get units: 		538875.0
masse: 		7005.5
get units: 		538837.0
masse: 		7005.0
get units: 		538837.0
masse: 		7005.0
get units: 		538830.0
masse: 		7004.9
get units: 		538842.0
masse: 		7005.1
get units: 		538822.0
masse: 		7004.8
get units: 		538835.0
masse: 		7005.0
get units: 		538806.0
masse: 		7004.6
get units: 		538788.0
masse: 		7004.4
get units: 		538807.0
masse: 		7004.6
get units: 		538788.0
masse: 		7004.4
get units: 		538807.0
masse: 		7004.6
get units: 		538787.0
masse: 		7004.4
get units: 		538845.0
masse: 		7005.1
get units: 		538808.0
masse: 		7004.6
get units: 		538804.0
masse: 		7004.6
get units: 		538816.0
masse: 		7004.8
get units: 		538805.0
masse: 		7004.6
get units: 		538806.0
masse: 		7004.6
get units: 		538791.0
masse: 		7004.4
get units: 		538790.0
masse: 		7004.4
get units: 		538792.0
masse: 		7004.4
get units: 		538805.0
masse: 		7004.6
get units: 		538772.0
masse: 		7004.2
get units: 		538792.0
masse: 		7004.4
get units: 		538800.0
masse: 		7004.5
get units: 		538816.0
masse: 		7004.8
get units: 		538784.0
masse: 		7004.3
get units: 		538791.0
masse: 		7004.4
get units: 		538806.0
masse: 		7004.6
get units: 		538810.0
masse: 		7004.7
get units: 		538797.0
masse: 		7004.5
get units: 		538799.0
masse: 		7004.5
get units: 		538779.0
masse: 		7004.3
get units: 		538765.0
masse: 		7004.1
get units: 		538773.0
masse: 		7004.2
get units: 		538788.0
masse: 		7004.4
get units: 		538793.0
masse: 		7004.5
get units: 		538788.0
masse: 		7004.4
get units: 		538767.0
masse: 		7004.1
get units: 		538780.0
masse: 		7004.3
get units: 		538749.0
masse: 		7003.9
get units: 		538742.0
masse: 		7003.8
get units: 		538759.0
masse: 		7004.0
get units: 		538737.0
masse: 		7003.7
get units: 		538752.0
masse: 		7003.9
get units: 		538731.0
masse: 		7003.6
get units: 		538751.0
masse: 		7003.9
get units: 		538780.0
masse: 		7004.3
get units: 		538785.0
masse: 		7004.4
get units: 		538757.0
masse: 		7004.0
get units: 		538766.0
masse: 		7004.1
get units: 		538786.0
masse: 		7004.4
get units: 		538772.0
masse: 		7004.2
get units: 		538746.0
masse: 		7003.8
get units: 		538791.0
masse: 		7004.4
get units: 		538737.0
masse: 		7003.7
get units: 		538734.0
masse: 		7003.7
get units: 		538742.0
masse: 		7003.8
get units: 		538743.0
masse: 		7003.8
get units: 		538800.0
masse: 		7004.5
get units: 		538782.0
masse: 		7004.3
get units: 		538774.0
masse: 		7004.2
get units: 		538757.0
masse: 		7004.0
get units: 		538748.0
masse: 		7003.9
get units: 		538754.0
masse: 		7003.9
get units: 		538740.0
masse: 		7003.8
get units: 		538730.0
masse: 		7003.6
get units: 		538766.0
masse: 		7004.1
get units: 		538746.0
masse: 		7003.8
get units: 		538757.0
masse: 		7004.0
get units: 		538763.0
masse: 		7004.1
get units: 		538768.0
masse: 		7004.1
get units: 		538797.0
masse: 		7004.5
get units: 		538756.0
masse: 		7004.0
get units: 		538782.0
masse: 		7004.3
get units: 		538757.0
masse: 		7004.0
get units: 		538772.0
masse: 		7004.2
get units: 		538778.0
masse: 		7004.3
get units: 		538732.0
masse: 		7003.7
get units: 		538755.0
masse: 		7004.0
get units: 		505310.0
masse: 		6569.6
get units: 		269270.0
masse: 		3504.2
get units: 		199713.0
masse: 		2600.8
get units: 		-190.0
masse: 		4.7
get units: 		-587.0
masse: 		-0.5
get units: 		-1334.0
masse: 		-10.2
get units: 		55651.0
masse: 		729.9
get units: 		180074.0
masse: 		2345.8
get units: 		272744.0
masse: 		3549.3
get units: 		543231.0
masse: 		7062.1
get units: 		605553.0
masse: 		7871.5
get units: 		742851.0
masse: 		9654.6
get units: 		834352.0
masse: 		10842.9
get units: 		847193.0
masse: 		11009.6
get units: 		842278.0
masse: 		10945.8
get units: 		845395.0
masse: 		10986.3
get units: 		833140.0
masse: 		10827.1
get units: 		848080.0
masse: 		11021.2
get units: 		846786.0
masse: 		11004.4
get units: 		849715.0
masse: 		11042.4
get units: 		850608.0
masse: 		11054.0
get units: 		849624.0
masse: 		11041.2
get units: 		845054.0
masse: 		10981.9
get units: 		838557.0
masse: 		10897.5
get units: 		832423.0
masse: 		10817.8
get units: 		837599.0
masse: 		10885.1
get units: 		836024.0
masse: 		10864.6
get units: 		842666.0
masse: 		10950.9
get units: 		843393.0
masse: 		10960.3
get units: 		829953.0
masse: 		10785.8
get units: 		846050.0
masse: 		10994.8
get units: 		839474.0
masse: 		10909.4
get units: 		821433.0
masse: 		10675.1
get units: 		306500.0
masse: 		3987.7
get units: 		88439.0
masse: 		1155.7
get units: 		-452.0
masse: 		1.3
get units: 		20840.0
masse: 		277.8
get units: 		256717.0
masse: 		3341.1
get units: 		711701.0
masse: 		9250.0
get units: 		806282.0
masse: 		10478.3
get units: 		721793.0
masse: 		9381.1
get units: 		787726.0
masse: 		10237.4
get units: 		783709.0
masse: 		10185.2
get units: 		377779.0
masse: 		4913.4
get units: 		14044.0
masse: 		189.5
get units: 		-234.0
masse: 		4.1
get units: 		-224.0
masse: 		4.2
get units: 		-236.0
masse: 		4.1
get units: 		-205.0
masse: 		4.5
get units: 		-245.0
masse: 		4.0
get units: 		-227.0
masse: 		4.2
get units: 		-251.0
masse: 		3.9
get units: 		-230.0
masse: 		4.2
get units: 		-247.0
masse: 		3.9
get units: 		-258.0
masse: 		3.8
get units: 		-233.0
masse: 		4.1
get units: 		-247.0
masse: 		3.9
get units: 		-260.0
masse: 		3.8
get units: 		-253.0
masse: 		3.9
get units: 		-241.0
masse: 		4.0
get units: 		-213.0
masse: 		4.4
get units: 		-216.0
masse: 		4.3
get units: 		-243.0
masse: 		4.0
get units: 		-246.0
masse: 		3.9
get units: 		-229.0
masse: 		4.2
get units: 		-254.0
masse: 		3.8
get units: 		-252.0
masse: 		3.9
get units: 		-217.0
masse: 		4.3
get units: 		-226.0
masse: 		4.2
get units: 		-229.0
masse: 		4.2
get units: 		-222.0
masse: 		4.3
get units: 		-230.0
masse: 		4.2
get units: 		-227.0
masse: 		4.2
get units: 		-236.0
masse: 		4.1
get units: 		-227.0
masse: 		4.2
get units: 		-206.0
masse: 		4.5
get units: 		-214.0
masse: 		4.4
get units: 		-217.0
masse: 		4.3
get units: 		-241.0
masse: 		4.0
get units: 		-212.0
masse: 		4.4
get units: 		-254.0
masse: 		3.8
get units: 		
