
# Maison Connectée - Domotique Miniature

## Description du projet
Ce projet vise à créer une maquette de maison connectée en utilisant des capteurs et des actionneurs pour simuler des systèmes de domotique. L'objectif est de démontrer comment les technologies modernes peuvent être utilisées pour améliorer le confort et l'efficacité énergétique d'une maison.

## Objectifs
- Surveiller et ajuster les conditions climatiques intérieures à l'aide de capteurs de température et d'humidité.
- Ajuster l'éclairage en fonction du niveau de lumière ambiante.
- Automatiser l'éclairage et d'autres appareils en détectant la présence humaine.
- Contrôler des appareils domestiques à distance via des modules de communication sans fil.

## Fonctionnalités
- **Capteurs de température et d'humidité :** Utilisation de DHT11 ou DHT22 pour surveiller les conditions climatiques.
- **Microcontrôleurs :** Utilisation d'Arduino pour la programmation et le contrôle des capteurs et des actionneurs.
- **Affichage d'informations :** Utilisation de NodeRed Dashboard pour afficher les données captées.
- **Capteur de qualité de l'air :** Utilisation du MQ-135 pour surveiller les polluants et la qualité de l'air.
- **Modules de communication :** Utilisation de modules Wi-Fi ou Bluetooth pour le contrôle à distance.

## Technologies Utilisées
- **Capteurs de température et d'humidité :** DHT11, DHT22
- **Capteur de lumière :** BH1750
- **Capteurs de mouvement :** PIR (Passive Infrared Sensor)
- **Actionneurs :** Relais, Transistors
- **Microcontrôleurs :** Arduino, Raspberry Pi
- **Écrans :** LCD, OLED
- **Capteur de qualité de l'air :** MQ-135
- **Modules de communication :** Wi-Fi, RX-TX

## Équipe de Projet
- **Lucas TARDIF:** Rôle : 3D Builder | Developper
- **Maxence GELIG :** Rôle : Main Developper
- **Timothée DUTOIS :** Rôle : Project Manager

## Livrables
- **Maquette fonctionnelle de la maison connectée**
   <img alt="image" src="project/model.png">
   Maison comportant 4 pièces (cuisine/salon/chambre/bureau), trous pour le passage des cables réalisé après impression et sur des emplacements libre de choix.
- **Rapport technique détaillant les composants et le code utilisé**
  - Code ESP32 -
       - Composant principal constituant la passerelle entre la machine virtuelle ainsi que son traitement, et l'ensemble des équipements physiques ainsi que leur état.
    
  - Code Arduino Leonardo -
      - Composant secondaire, nécessaire pour fournir des ports analogiques supplémentaires. Les deux cartes communiquent entre elles via une connexion RX-TX.

   Selon la réalisation de votre maison, seul l'ESP32 sera nécessaire, cependant celui-ci ne fournissant pas une multitude de port Analogique, nous avons eu recourt un l'utilisation d'une deuxième carte.

- **Machine Virtuelle**
  - Installation Docker :
       - Portainer : interface graphique d'administration des dockers/images.
       - NodeRed :  Serveur d'application, visant à recueillir les informations.
       - Fiware : Création et gestion de l'ensemble des pièces de la maison et des capteurs.
       - Orion - MongoDB : Historisation des données pour une future analyse.
    
    ⚠️ Gestion des volumes depuis Portainer à préconiser pour éviter une perte totales de vos données ⚠️

  - Exemple entité Fiware :
   
         "id": "Kitchen",
         "type": "Room",
         "Temperature": {
            "type": "float",
            "value": 19
         },
         "Gaz": {
            "type": "int",
            "value": 80
         },
         "Status": {
            "type": "string",
            "value": "OK"
         }
      
  - Via Fiware :  Création au besoin d'abonnement (Subscription)
       - Mise à disposition d'alerte en fonction d'un seuil critique fourni.

  - Node red :
       Machine Virtuelle indisponible donc je ne peux pas fournir le flux node-red ni un aperçu.  

         
- **Présentation finale du projet**
     - Une fois l'ensemble de vos équipements branchés voici comment s'articule le projet.
          - Le scan d'un badge RFID (sans disctinction) actionne ou désactive le système d'alarme.
          - Système d'alarme OFF :
               - Led éteinte, aucune données
          - Système d'alarme ON :
               - Envoie d'un signal à la deuxième carte pour lui signaler le changement d'état.
               - réception des données de l'autre carte via RX/TX
               - Envoie de l'ensemble des données sur le réseau MQTT
               - Réception des données sur le serveur NodeRed via le topic choisi.
               - Mise à jours des entitées Fiware ainsi que l'Affichage du Dashboard Node-Red.

- ## Évolutions possibles
- **Capteurs de lumière :** Utilisation du BH1750 pour ajuster l'éclairage en fonction de la luminosité ambiante.
- **Capteurs de mouvement :** Utilisation de capteurs PIR pour détecter la présence humaine et automatiser les appareils.
- **Ajout des données en base - Historisation :** Système d'historique complet pouvant servir par la suite à une analyse. (pertes de chaleur de la maison par exemple)

## Références
- [DHT11/DHT22 Documentation](https://www.adafruit.com/product/386)
- [BH1750 Capteur de lumière](https://www.sparkfun.com/products/retired/12055)
- [Capteur PIR](https://www.adafruit.com/product/189)
- [MQ-135 Capteur de qualité de l'air](https://www.adafruit.com/product/3199)
- [Arduino Documentation](https://www.arduino.cc/en/Guide)
- [Raspberry Pi Documentation](https://www.raspberrypi.org/documentation/)
- [Modèle de la maison](https://cad.onshape.com/documents/de840b971062f1b6a3d2750f/w/ea7fc6207572d0be96b95fa8/e/23b2df6f54c7adfef7c06e8f)

