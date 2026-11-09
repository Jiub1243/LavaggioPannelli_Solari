/*////////////////////////////////////////////////////////////////////
//  Lavaggio Pannelli Solari 31/01/2025                             //
//                                                                  //
//  Quadro Comandi (con tastierino e display 20x4)                  //
//  (ESP32 WROOM1)                                                  //
//                                                                  //
//                                                                  //
//  Comandi inviati al Traslo:                                      //
//    01001  → Richiesta stato                                      //
//    02001  → Azzeramento assi (Zero_Traslo)                       //
//    02003  → Stop immediato (ARRESTA)                             //
//    03NNN  → Vai alla colonna NNN  (000 = posizione Home)         //
//    03999  → Richiesta colonna corrente (nessun movimento)        //
//                                                                  //
//  Risposte ricevute dal Traslo:                                   //
//    01NNN  → Stato (NNN = STATO_CORRENTE)                         //
//    03NNN  → In movimento, NNN = colonna corrente                 //
//    01009  → Comando non eseguibile                               //
////////////////////////////////////////////////////////////////////*/

// La prima volta che si carica il codice nella ESP32 va abilitata la porta USB CDC: Tools -> USB CDC on Boot -> Enabled

#include <WiFi.h>
#include <WiFiMulti.h> // permette di registrare molteplici access points e di connettersi in automatico a quello col segnale migliore

#include <HTTPClient.h>
#include <Update.h>

#include <PubSubClient.h> // Installata dal library manager, MQTT

#include <Wire.h> 
#include <LiquidCrystal_I2C.h> // https://github.com/johnrickman/LiquidCrystal_I2C
#include <Keypad.h> // Keypad da Mark Stanley, Alexander Brevig
#include <EEPROM.h>
#include <MobaTools.h>
#include "I2CKeyPad.h"
#include "MyDef.h"

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; } // waiting for serial to stabilize
  Serial1.begin(115200);
  while (!Serial1) { ; } // waiting for serial to stabilize
  //pinMode(LED_BUILTIN, OUTPUT);
  
  lcd.init(); 
  lcd.backlight();

  Wire.begin();
  Wire.setClock(400000);
  if (keyPad.begin() == false)
  {
    Serial.println("\nTastiera scollegata\n");
  }

  
  WiFi.mode(WIFI_STA); // STATION: si connette ad un network esistente
  WiFi.setAutoReconnect(true); // per riconnetteresi in caso la connessione crolli
  WiFiMulti.addAP(ssid_AP2, password_AP2); // per il wifi di casa

  lcd.setCursor(0, 0);
  lcd.print("Connessione WiFi...");

  // Provo a connettermi al WiFi per 30sec al massimo
  uint32_t startWiFi = millis();
  while (WiFiMulti.run() != WL_CONNECTED && millis() - startWiFi < 30000) {
    Serial.print(".");
    delay(1000);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi connesso! IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi non disponibile, proseguo senza. Riproverò nel loop.");
  }

  // MQTT setup
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  // La connessione effettiva avviene nel loop() tramite gestisciMQTT()

  Homepage();
}

// TODO
// Non mi piace il fatto che questo loop in realtà non è quello che ripetiamo all'infinito ma è quello in Test_01. Sarebbe meglio spostare i controlli di routine per LoRa e OTA updates qui
// Però Test_01 ricarica lo schermo ogni volta che viene chiamato ora come ora, se lo invocassi ogni iterazione flickerebbe troppo
void loop(){
  // Controlli da fare anche se non abbiamo messo la PW
  // LoRa
  if (ReadDataFromAntenna()) {
    if ( parseAntenna() ) esaminaMessaggio();  // esamina messaggio seriale
  }

  //solo per test, triggera l'update all'input da serial monitor
  if (Serial.available() && Serial.read() == 'U') FW_UPDATE = 1;

  // MQTT (riconnessione non bloccante + processing messaggi in arrivo)
  gestisciMQTT();

  // Controllo OTA triggerato da MQTT, eseguito quando gli slave sono fermi
  if (shouldUpdateFirmware()) {
    updateFirmware();
  }
  
  if( ! password()){ // pw corretta
    Test_01();
    Serial.println( "Fine Test" );
    Homepage();
  }  
  delay(100);
  
}


