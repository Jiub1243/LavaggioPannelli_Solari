/*////////////////////////////////////////////////////////////////////
//  Lavaggio Pannelli Solari 31/01/2025                             //
//                                                                  //
//  TRASLO                                                          //
//  (Arduino nella scatola sopra il traslo)                         //
//  M1 - Motore avanzamento superiore (cremagliera)                 //
//  0-285500 (1mm = 28,5 tic )                                      //
//                                                                  //
//  M2 - Motore avanzamento inferiore (ruote liscie)                //
//  0-11520  (1mm =   Solleva   tic)                                //
//    M2 = M1 / 1,732851985559567                                   //
////////////////////////////////////////////////////////////////////*/

#include <Wire.h> 
#include <LiquidCrystal_I2C.h> // https://github.com/johnrickman/LiquidCrystal_I2C
#include <Keypad.h> // Keypad da Mark Stanley, Alexander Brevig
#include <EEPROM.h>
#include <MobaTools.h>
#include "I2CKeyPad.h"
#include "MyDef.h"

void setup(){

  lcd.init(); 
  lcd.backlight();

  Wire.begin();
  Wire.setClock(400000);
  if (keyPad.begin() == false)
  {
    Serial.println("\nTastiera scollegata\n"); //TODO funziona anche se dobbiamo ancora fare Serial.begin?
  }

  //stepper_M1.attachEnable( EnaPin_M1, 10, LOW );  // Commentare per FRENO
  stepper_M1.attach( StepPin_M1, DirPin_M1, INV_DIR );
  stepper_M1.setSpeed( 800 );             // 30 rev/min (if stepsPerRev is set correctly)
  stepper_M1.setRampLen( 400 ); // Ramp length is 1/2 revolution

  //stepper_M2.attachEnable( EnaPin_M2, 10, LOW );  // Commentare per FRENO
  stepper_M2.attach( StepPin_M2, DirPin_M2, INV_DIR );
  stepper_M2.setSpeed( 400 );             // 30 rev/min (if stepsPerRev is set correctly)
  stepper_M2.setRampLen( 200 ); // Ramp length is 1/2 revolution

  pinMode(Error_Driver_M1, INPUT_PULLUP);
  pinMode(Error_Driver_M2, INPUT_PULLUP);
  
  pinMode(Sensore_Zero_M1, INPUT_PULLUP);
  pinMode(Sensore_Posizione_M1, INPUT_PULLUP);
  pinMode(Sensore_Posizione_M2, INPUT_PULLUP);
  
  digitalWrite(signalPin, HIGH);
  Serial.begin( 115200 );
  while ( !Serial )
  {
    ; // wait for serial port to connect. Needed for native USB
  }

  Serial.println( "*********************************************************" );
  Serial.print( "Arduino - Lavaggio Pannelli Solari - TRASLO -" );
  Serial.print( __DATE__ );
  Serial.print( " " );
  Serial.println( __TIME__ );
  Serial.println( "*********************************************************" );
  Homepage();
  Serial1.begin( 115200 );

  long temp_M1;
  long temp_M2;
  uint8_t temp_maxCol;
  long temp_P0_M1;
  long temp_P0_M2;

  EEPROM.get(EE_ADDR_PASSI_M1, temp_M1);
  EEPROM.get(EE_ADDR_PASSI_M2, temp_M2);
  temp_maxCol = EEPROM.read(EE_ADDR_MAXCOL);
  EEPROM.get(EE_ADDR_PASSI0_M1, temp_P0_M1);
  EEPROM.get(EE_ADDR_PASSI0_M2, temp_P0_M2);

  //assegno nuovi valori letti dalla memoria solo quando superano il sanity check. Altrimenti se la memoria è fleshata o corrotta si spacca tutto
  if (temp_M1 > 0 && temp_M1 < 100000)   passi_Colonna_M1 = temp_M1;
  if (temp_M2 > 0 && temp_M2 < 100000)   passi_Colonna_M2 = temp_M2;
  if (temp_maxCol > 0 && temp_maxCol < 100) maxCol = temp_maxCol;
  if (temp_P0_M1 > 0 && temp_P0_M1 < 100000) passi0_M1 = temp_P0_M1;
  if (temp_P0_M2 > 0 && temp_P0_M2 < 100000) passi0_M2 = temp_P0_M2;

}

void loop(){
  
  //if( ! password()){
    Test_01();
    Serial.println( "Fine Test" );
    Homepage();
  //}

  if (ReadDataFromAntenna()) {
    if ( parseAntenna() ) esaminaMessaggio();  // esamina messaggio seriale
  }
  delay(100);
}

