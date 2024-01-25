/*
  ******************************
  Nico78 - French Forum Arduino
  *********************************************************************************************
  Création d'un service BLE pour ajouter un UART BLE (Equivalent du SPP bluetooth Classic)
  *********************************************************************************************
  Exemple basé sur les informations du site web d'Adafruit, de Neil Kolban et de ThingEngineer:
  https://learn.adafruit.com/introducing-adafruit-ble-bluetooth-low-energy-friend/uart-service
  https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLETests/Arduino/BLE_client/BLE_client.ino
  https://github.com/ThingEngineer/ESP32_BLE_client_uart
  *********************************************************************************************************************************
  Test effectué sur un Arduino nano IOT 33 avec smartphone Android et l'application Serial Bluetooth Terminal (Kai Morich)
  Paramètres de l'application pour le test:
  Terminal Settings: Charset UTF8 (par défaut)
  Display mode: Text (par défaut)
  Receive Settings: CR ou LF (Ne pas mettre les deux), le Moniteur série d'arduino devra être configuré de la même façon
  Send Settings: Character delay -> 0 ms dans ce cas, envoie des données par bloc de 20 caractères au maximum
  ou Send Settings: Character delay -> 1 ms envoie des données au fil de l'eau
  la longueur max de la chaine est définie par la variable tamponCompletedText initialisé a 64 octets que vous pouvez modifier
  *********************************************************************************************************************************
    Infos utiles en FR: https://blog.groupe-sii.com/le-ble-bluetooth-low-energy/
  *********************************************************************************************************************************
*/

#include <ArduinoBLE.h>

// Déclaration du service offert et des caractéristiques associées
// Comprendre que TX et RX sont des caractéristiques offertes pour que le client puisse écrire et lire les données
// donc TX et RX sont vues coté client, par conséquent l'arduino lira dans TX et écrira dans RX
BLEService UARTService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
BLECharacteristic TX("6E400002-B5A3-F393-E0A9-E50E24DCCA9E", BLEWrite, 20);
BLECharacteristic RX("6E400003-B5A3-F393-E0A9-E50E24DCCA9E", BLERead | BLENotify, 20);

//Ne pas changer (tampon max de 20 octets que l'on peut écrire dans une caractéristique, restriction du bluetooth)
const unsigned int tamponBluetooth = 20;
char receiveBluetooth[tamponBluetooth + 1] = {0,};

// Vous pouvez changer la valeur tamponCompletedText ci dessous
// suivant la chaine de longueur max que vous souhaitez pouvoir recevoir
const unsigned int tamponCompletedText = 64;
char completedText[tamponCompletedText + 1] = {0,}; // + 1 pour le '\0' de fin de chaine

BLEDevice central;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }

  // Nom lié au service GENERIC ACCESS
  BLE.setDeviceName("UART Service");

  // Nom qui apparait lors d'un scan
  BLE.setLocalName("UART BLE");

  // Déclaration du service que l'on veut offrir
  //BLE.setAdvertisedServiceUuid(UARTService.uuid());
  BLE.setAdvertisedService(UARTService);

  UARTService.addCharacteristic(TX);
  UARTService.addCharacteristic(RX);

  BLE.addService(UARTService);

  // Démarrer la publication du service
  BLE.advertise();

  Serial.println("UART Service");
}

void loop() {
  int ret = 0;
  static int etat = 0;
  char receptionPortSerie[tamponBluetooth + 1] = {0,};

  // Nous sommes un périphérique esclave (serveur)
  // et nous attendons une connexion centrale maitre (client)

  // En attente de connexion d'un client
  central = BLE.central();

  // etat est utilisé pour éviter la répétition de l'information
  if (etat == 0) {
    etat = 1;
    Serial.println("périphérique esclave (serveur), en attente de connexion d'un client maitre");
    Serial.println("");
  }

  // Test si un appareil s'est connecté
  if (central) {
    Serial.print("Connecté à l'appareil suivant: ");
    Serial.println(central.address());
    Serial.println("");

    while (central.connected()) {
      // ***********************************************************************
      // **** Lecture des données reçues du port série *************************
      if (Serial.available() > 0) {
        memset(receptionPortSerie, 0, tamponBluetooth);
        for (unsigned int i = 0; i < 20; ++i) { // lecture par bloc de 20 max
          receptionPortSerie[i] = Serial.read();
          //Serial.println(receptionPortSerie[i]);
          if (receptionPortSerie[i] == '\r' || receptionPortSerie[i] == '\n' ) {
            break;
          }
        }
        // *********************************************************************
        // **** Envoie des données du port Série sur le module Bluetooth **
        // que le marqueur de fin soit reçu ou pas (pas de limite pour l'envoie)
        if (writeBleUART(receptionPortSerie))
        {
          Serial.print("Données envoyées: ");
          Serial.println(receptionPortSerie);
        } else {
          Serial.print("Une erreur s'est produite pour l'envoie des données");
        }
        // *********************************************************************
      }

      //***********************************************************************
      //**** Lecture des données du module Bluetooth **************************
      ret = readBleUART();
      if (ret == 1) {                     // données reçues
        Serial.print("Données reçues sur TX: ");
        Serial.print(receiveBluetooth);
        Serial.print("   Longueur: ");
        Serial.println(strlen(receiveBluetooth));
        Serial.println("");
      } else if (ret == 2) {              // chaine complète (marqueur de fin reçu)
        printSerial(completedText, ret);
      } else if (ret == 0) {              // erreur rencontrée, chaine incomplète
        printSerial(completedText, ret);
      } else {                            // -1  Pas de données!
        //Serial.println("Pas de données! ");
      }
      //*************************************************
      // ******* Votre code personnel ici ***************
      //*************************************************
    }

    Serial.print("Déconnecté du central: ");
    Serial.println(central.address());
    Serial.println("");
    etat = 0;
  }
}

int writeBleUART(char text[]) {
  if (central.connected()) {
    if (RX.writeValue(text, strlen(text)))
      return 1;
  }
  return 0;
}

int readBleUART(void) {
  int etat = -1;
  unsigned int lenData = 0;
  static unsigned int align = 0;

  if (central.connected()) {
    if (TX.written()) {
      lenData = TX.valueLength();
      memset(receiveBluetooth, 0, tamponBluetooth);
      if (TX.readValue(receiveBluetooth, lenData)) {
        etat = 1;
        if (align == 0) {
          memset(completedText, 0, tamponCompletedText + 1);
        }
        if ((align + lenData) < (tamponCompletedText + 1) ) {
          memcpy(completedText + align, receiveBluetooth, lenData);
          align += lenData;
          if (completedText[align - 1] == '\r' || completedText[align - 1] == '\n')
          {
            align = 0;
            etat += 1;
          }
        } else {
          memcpy(completedText + align, receiveBluetooth, tamponCompletedText - align);
          align = 0;
          etat = 0;
        }
      }
    }
  }
  return etat;
}

void printSerial(char text[], int etat) {
  if (etat) {
    Serial.print("Chaine complète: ");
  } else {
    Serial.print("Erreur longueur max de la chaine atteinte: ");
    Serial.print(tamponCompletedText);
    Serial.println(" octets et marqueur de fin non reçu!");
    Serial.println("Rappel, le dernier caractère en byte doit être 13 pour '\\r' ou 10 pour '\\n'");
    Serial.print("*** Chaine incomplète *** : ");
  }
  Serial.print(text);
  Serial.print("   Longueur: ");
  Serial.print(strlen(text));
  Serial.print("   Dernier caractère (byte): ");
  Serial.println((byte)text[strlen(text) - 1]);
  Serial.println("");
}