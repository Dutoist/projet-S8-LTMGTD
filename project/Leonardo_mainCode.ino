#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

// LiquidCrystal_I2C lcd(0x27, 16, 2);

const int pinTempSalon = A4;
double tempSalon;
const int pinTempChambre = A5;
double tempChambre;

// MQ2
const int pinDigitalMQ2 = 8;
const int pinAnalogMQ2 = A3;
int valGaz;

bool isGaz;
bool isAlarm;

float getTemperatureFromSensorKeyestudio(int pinSensor)
{
    double val = analogRead(pinSensor);
    double fenya = (val / 1023) * 5;
    double r = (5 - fenya) / fenya * 4700;
    double temp = (1 / (log(r / 10000) / 3950 + 1 / (25 + 273.15)) - 273.15);
    return temp;
}

int getGasValue(int pin)
{
    int val = analogRead(pin);
    Serial.println(val);

    return val;
}

void array_to_string(byte array[], unsigned int len, char buffer[])
{
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i * 2 + 0] = nib1 < 0xA ? '0' + nib1 : 'A' + nib1 - 0xA;
        buffer[i * 2 + 1] = nib2 < 0xA ? '0' + nib2 : 'A' + nib2 - 0xA;
    }
    buffer[len * 2] = '\0';
}

void setup()
{

    Serial.begin(9600);  // Communication avec le PC via USB
    Serial1.begin(9600); // Communication avec l'ESP8266 via RX1 et TX1

    // MQ2
    pinMode(pinAnalogMQ2, INPUT);
    pinMode(pinDigitalMQ2, INPUT);
    isGaz = false;

    pinMode(pinTempSalon, INPUT);
    pinMode(pinTempChambre, INPUT);

    isAlarm = false;

}

void loop()
{

    // Lecture des données série depuis l'ESP8266
    if (Serial1.available() > 0)
    {
        Serial.println("testLecture");
        char received = Serial1.read();
        Serial.println(received);
        if (received == '1')
        {
            Serial.println("alarm true");
            isAlarm = true;
        }
        else if (received == '0')
        {
            Serial.println("alarm false");
            isAlarm = false;
        }
    }
    if (isAlarm)
    {
        tempSalon = getTemperatureFromSensorKeyestudio(pinTempSalon);
        tempChambre = getTemperatureFromSensorKeyestudio(pinTempChambre);
        valGaz = getGasValue(pinAnalogMQ2);

        // Créer une chaîne de caractères avec les données
        String data = String(tempChambre) + "," + String(tempSalon) + "," + String(valGaz) + "\n";

        // Envoyer les données à l'ESP8266
        Serial1.println(data);
        // Afficher les mêmes données sur le moniteur série USB pour le débogage
        Serial.println(data);
    }

    delay(1000);
}
