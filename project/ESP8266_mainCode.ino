#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

// Paramètres WiFi
const char* ssid = "RIOC-TP-IoT";  // Remplacez par le nom de votre réseau Wi-Fi
const char* password = "GRIT-RIOC_2024";  // Remplacez par le mot de passe de votre Wi-Fi

// Paramètres MQTT
const char* mqtt_server = "10.19.4.44";  // IP du broker MQTT
const char* mqtt_user = "ltmgtd_esp";  // Nom d'utilisateur MQTT
const char* mqtt_password = "ltmgtd";  // Mot de passe MQTT
const char* mqtt_topic = "home/sensors";  // Topic MQTT

WiFiClient espClient;
PubSubClient client(espClient);

//PIN DEFINITION
#define RST_PIN  0  // Configurer le pin RST RFID
#define SS_PIN   15  // Configurer le pin SS RFID
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Créer une instance de la classe MFRC522

#define LED_PIN  16 //  LED
#define soundPin A0 // Sound Sensor

bool ALARME_STATUT = false;  

int RX_PIN = 4;  // D1 sur un NodeMCU par exemple
int TX_PIN = 5;  // D2 sur un NodeMCU par exemple

// Créer une instance de SoftwareSerial
SoftwareSerial mySerial(RX_PIN, TX_PIN); // RX, TX

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.begin(9600); 
  mySerial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  
  Serial.println("Systeme pret. Approchez votre carte RFID.");
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.publish(mqtt_topic, "ESP8266_Connnected");
      publishJson("alarme","OFF");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  if(ALARME_STATUT){

      int soundValue = analogRead(soundPin);
      Serial.print("Valeur Analogique : ");
      Serial.println(soundValue);
      publishJson("sound_bedroom", String(soundValue).c_str());
      
      while (mySerial.available() > 0) {
        char buffer[128] = {0}; // Initialiser le buffer avec des zéros
        size_t len = mySerial.readBytesUntil('\n', buffer, sizeof(buffer) - 1);
        buffer[len] = '\0'; // S'assurer que la chaîne est terminée correctement
        if (len == 0) continue; // Si aucune donnée n'est lue, passer à la prochaine itération
        
        Serial.println(buffer); // Afficher les données reçues pour le débogage

        // Conversion et extraction des valeurs
        String received = String(buffer);
        int firstComma = received.indexOf(',');
        int secondComma = received.indexOf(',', firstComma + 1);

        float temp_office = received.substring(0, firstComma).toFloat();
        float temp_kitchen = received.substring(firstComma + 1, secondComma).toFloat();
        int GazValue = received.substring(secondComma + 1).toInt();

        if(temp_office>0) publishJson("temp_office", String(temp_office).c_str());
        if(temp_kitchen>0) publishJson("temp_kitchen", String(temp_kitchen).c_str());
        if(GazValue>0) publishJson("gaz_kitchen", String(GazValue).c_str());
        
        
    }

    delay(500);

  }
  

  // Vérifier la présence d'un badge RFID
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.print("UID de la carte: ");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println();


    changeAlarmStatus();
    digitalWrite(LED_PIN, ALARME_STATUT ? HIGH : LOW);
  }

  mfrc522.PICC_HaltA();
}

void changeAlarmStatus() {
  envoyerIsAlarm(ALARME_STATUT);
  ALARME_STATUT = !ALARME_STATUT;
  Serial.println("Statut de l'alarme: " + String(ALARME_STATUT ? "ON" : "OFF"));
  const char* state = ALARME_STATUT ? "ON" : "OFF";
  publishJson("alarme",state);      
}

void envoyerIsAlarm(bool etat) {
    if (etat) {
        mySerial.write('0');
    } else {
        mySerial.write('1');
    }
}

void publishJson(const char* element, const char* value) {
  const int capacity = JSON_OBJECT_SIZE(1);
  StaticJsonDocument<capacity> doc;
  doc[element] = value;

  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);
  client.publish(mqtt_topic, jsonBuffer);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message reçu sur le topic [");
  Serial.print(topic);
  Serial.print("] ");

  // Créer une chaîne de caractères à partir du payload pour l'analyse JSON
  char msg[length + 1];
  strncpy(msg, (char*)payload, length);
  msg[length] = '\0';
  Serial.println(msg);

  if (strcmp(msg, "true") == 0) {
      if (!ALARME_STATUT) { // Change le statut uniquement si l'état actuel est différent
        changeAlarmStatus();
        digitalWrite(LED_PIN, HIGH);
      }
    } else if (strcmp(msg, "false") == 0) {
      if (ALARME_STATUT) { // Change le statut uniquement si l'état actuel est différent
        changeAlarmStatus();
        digitalWrite(LED_PIN, LOW);
      }
    }

  // Ajoutez d'autres traitements ici si nécessaire
}
