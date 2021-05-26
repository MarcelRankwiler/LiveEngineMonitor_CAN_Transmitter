// Liveengine Diplomarbeit 2021 Marcel Rankwiler
// CAN-Sender mit Potentiometer und Wippenschalter-Auswertung
// Version 1.0

#include <CAN.h>

//Hardware Pins
int analogPin = A6;
bool switchPin = 1;

// Zeitverzögerung beim Senden von CAN-Frames in ms
int timeDelay = 20;

// Zwischenspeicher falls Werte sich zwischen Serial.print und CAN.write ändern
int potiSpeicherWert = 0;
bool switchSpeicherWert = 0;

//Display-Anzeige glätten
int potiSmooth = 0;

void setup() {
//Serielle Schnittstelle initialisieren
  Serial.begin(9600);

//Starte CAN Bus mit 500 kbps inkl. Fehlerdiagnose
  if (!CAN.begin(500E3)) {
    Serial.println("CAN init error");
    while (1);
  }
  Serial.println("CAN init ok");
}

void readAnalog(){
  potiSpeicherWert = analogRead(analogPin);
  potiSmooth = 0.8 * potiSmooth + 0.2 * analogRead(analogPin);
  }
void readDigital(){
  switchSpeicherWert = !digitalRead(switchPin); //Switch ist invertiert
  }
void loop() {
  //LED Toggeln
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

  readDigital();
  //Schalter-Status über serielle Schnittstelle ausgeben
  Serial.print("Schalter: ");
  Serial.print(switchSpeicherWert);
  Serial.print("\t");

  //CAN ID1: Schalter-Status senden
  CAN.beginPacket(0x01);
  CAN.write(switchSpeicherWert);
  CAN.write(0x00);
  CAN.write(0x00);
  CAN.write(0x00);
  CAN.write(0x00);
  CAN.write(0x00);
  CAN.write(0x00);
  CAN.write(0x00);
  CAN.endPacket();
  
  //Zeitverzögerung bis zum Senden des Potentiometers
  delay(timeDelay);
  
  //Analogwert geglättet in potiSmooth und ungeglättet in potiSpeicherWert speichern
  readAnalog()();

  //Analogwert ungeglättet über serielle Schnittstelle ausgeben
  Serial.print("Poti Speicherwert: ");
  Serial.print(potiSpeicherWert);
  Serial.print("\t");
  Serial.print("Pori Smooth: ");
  Serial.println(potiSmooth);

  //CAN ID2: Analogwert Potentiometer senden 
  CAN.beginPacket(0x02);
  CAN.write(potiSpeicherWert & 0xff); //oder lowByte / highByte Funktion
  CAN.write((potiSpeicherWert>>8) & 0xff);
  CAN.write(0x00);
  CAN.write(0x00);
  CAN.write(0x00);
  CAN.write(0x00);
  CAN.write(0x00);
  CAN.write(map(potiSmooth, 0, 1023, 0, 255)); //Byte 7: Geglättet für Anzeige
  CAN.endPacket();   
  
  //Zeitverzögerung bis zum Neustart des Loops
  delay(timeDelay);
}
