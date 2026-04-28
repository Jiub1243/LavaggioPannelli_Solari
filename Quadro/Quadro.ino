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

  /*
  WiFi.mode(WIFI_STA); // STATION: si connette ad un network esistente
  WiFi.setAutoReconnect(true); // per riconnetteresi in caso la connessione crolli
  WiFiMulti.addAP(ssid_AP2, password_AP2); // per il wifi di casa

  lcd.setCursor(0, 0);
  lcd.print("Connessione WiFi...");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("WiFi connesso! IP: ");
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IP:");
  lcd.print(WiFi.localIP());
  */

  Homepage();
}


void loop(){
  
  if( ! password()){ // pw corretta
    if (ReadDataFromAntenna()) {
      if ( parseAntenna() ) esaminaMessaggio();  // esamina messaggio seriale
    }
    Test_01();
    Serial.println( "Fine Test" );
    Homepage();
  }  
  
  delay(100);
  
}


