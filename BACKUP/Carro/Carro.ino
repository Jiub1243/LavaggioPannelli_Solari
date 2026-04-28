/*////////////////////////////////////////////////////////////////////
//  Lavaggio Pannelli Solari 31/01/2025                             //
//                                                                  //
//  CARRO                                                           //
//  (Arduino sopra la Spazzola)                                     //
//                                                                  //
//                                                                  //
////////////////////////////////////////////////////////////////////*/

#include <Wire.h> 
#include <LiquidCrystal_I2C.h> // https://github.com/johnrickman/LiquidCrystal_I2C
#include <Keypad.h> // Keypad da Mark Stanley, Alexander Brevig

#include <MobaTools.h>
#include "I2CKeyPad.h"
#include <EEPROM.h>
#include "MyDef.h"

LiquidCrystal_I2C lcd(LCD_ADR, 20, 4); // Definizione Display I2C 20 X 4
I2CKeyPad keyPad(KEYPAD_ADDRESS); // Definizione Tastiera I2C

void setup(){

  lcd.init(); 
  lcd.backlight();

  Wire.begin();
  Wire.setClock(400000);
  if (keyPad.begin() == false)
  {
    Serial.println("\nTastiera scollegata\n");
  }

  //  M1 - Motore sollevamento Spazola SX --- 0-11520  (1mm =   Solleva 45 tic)
  //stepper_M1.attachEnable( EnaPin_M1, 10, LOW );
  stepper_M1.attach( StepPin_M1, DirPin_M1 );
  stepper_M1.setSpeed( 2000 );             //2000 - 30 rev/min (if stepsPerRev is set correctly)
  stepper_M1.setRampLen( stepsPerRev / 8 ); // Ramp length is 1/8 revolution

  //  M2 - Motore sollevamento Spazola DX --- 0-11520  (1mm =   Solleva 45 tic)
  //stepper_M2.attachEnable( EnaPin_M2, 10, LOW );
  stepper_M2.attach( StepPin_M2, DirPin_M2 );
  stepper_M2.setSpeed( 2000 );             //2000 - 30 rev/min (if stepsPerRev is set correctly)
  stepper_M2.setRampLen( stepsPerRev / 8 ); // Ramp length is 1/8 revolution

  //  M3 - Motore Rotazione Spazzola
  stepper_M3.attachEnable( EnaPin_M3, 10, LOW );
  stepper_M3.attach( StepPin_M3, DirPin_M3 );
  stepper_M3.setSpeed( Velocita_Spazzola );             // 3750
  stepper_M3.setRampLen( stepsPerRev / 2 ); // Ramp length is 1/2 revolution

  //  M4 - Motore Avanzamento Carro  --- 0-285500 (1mm = 28,5 tic )
  //stepper_M4.attachEnable( EnaPin_M4, 10, LOW ); // Commentare per FRENO
  stepper_M4.attach( StepPin_M4, DirPin_M4 );
  stepper_M4.setSpeed( 1500 );             // 3750-1875 - 30 rev/min (if stepsPerRev is set correctly)
  stepper_M4.setRampLen( stepsPerRev ); // Ramp length is 4 revolution

  pinMode(Accendi_Driver_M3, OUTPUT);
  pinMode(Accendi_Driver_M4, OUTPUT);
  pinMode(Collega_M4, OUTPUT);
  pinMode(Ventola, OUTPUT);
  digitalWrite(Ventola, LOW ); // Spegni Ventola Quadro

  pinMode(Error_Driver_M1, INPUT_PULLUP);
  pinMode(Error_Driver_M2, INPUT_PULLUP);
  pinMode(Error_Driver_M3, INPUT_PULLUP);
  pinMode(Error_Driver_M4, INPUT_PULLUP);
  pinMode(Sens_Zero_M1, INPUT_PULLUP);
  pinMode(Sens_Zero_M2, INPUT_PULLUP);
  pinMode(Sens_Zero_M4, INPUT_PULLUP);
  pinMode(OltreCorsa_M4, INPUT_PULLUP);
  digitalWrite(signalPin, HIGH);
  Serial.begin( 115200 );
  while ( !Serial )
  {
    ; // wait for serial port to connect. Needed for native USB
  }

  Serial.println( "*********************************************************" );
  Serial.print( "Arduino - Lavaggio Pannelli Solari - " );
  Serial.print( __DATE__ );
  Serial.print( " " );
  Serial.println( __TIME__ );
  Serial.println( "*********************************************************" );
  Homepage();
  Serial1.begin( 115200 );

  Serial2.begin( 115200 );
  Serial3.begin( 115200 );
  delay(500);
  spedisciStringa("HELLO");
  Abbassamento_SX = EEPROM.read(513);
  Abbassamento_DX = EEPROM.read(514);
}

void loop(){
  if( ! password()){
    Test_01();
    Serial.println( "Fine Test" );
    Homepage();
  }

  // Ricevuto messaggio wireless ?
  if (ReadDataFromAntenna()) {
    if ( parseAntenna() ) esaminaMessaggio();  // esamina messaggio seriale
  }
  delay(100);
}

void Zero_Sollevatore(void){
  int sensore_SX, sensore_DX, sensore_UP;
  uint8_t arresto;
  STATO_CORRENTE = 7;
  Serial.println("Zero Sollevamento");
  Posizione_M1 = 0;
  Posizione_M2 = 0;
  sensore_SX = digitalRead(Sens_Zero_M1);
  sensore_DX = digitalRead(Sens_Zero_M2);
  if( sensore_SX ) stepper_M1.rotate(1);
  if( sensore_DX ) stepper_M2.rotate(1);
  do{
    sensore_SX = digitalRead(Sens_Zero_M1);
    sensore_DX = digitalRead(Sens_Zero_M2);
    if(! sensore_SX) {
      stepper_M1.stop();
      Posizione_M1 = 1;
      stepper_M1.setZero();
      //Serial2.print("R"); // Azzera il sensore altezza SX
    }
    if(! sensore_DX) {
      stepper_M2.stop();
      Posizione_M2 = 1;
      stepper_M2.setZero();
      //Serial3.print("R"); // Azzera il sensore altezza DX
    }
    sensore_UP = digitalRead(Sens_Zero_M4);
     if(! sensore_UP) {
      stepper_M4.rotate(0);
    }
    arresto = Interrompi();
  } while ( ( stepper_M1.moving() || stepper_M2.moving() ) && arresto );
  stepper_M1.rotate(0);
  stepper_M2.rotate(0);
  //delay(1200);
  if( (Posizione_M1 == 0) || (Posizione_M2 == 0) ) Serial.println("Zero Sollevamento KO");
  else {
    Serial2.print("R"); // Azzera il sensore altezza SX
    Serial3.print("R"); // Azzera il sensore altezza DX
    delay(500);
  }
  STATO_CORRENTE = 0;
  Serial.println("Zero Sollevamento fatto");
}

void Zero_Avanzamento(void){
  uint8_t sensore_SX, sensore_DX, sensore_UP, arresto, Fault_M4;
  STATO_CORRENTE = 8;
  Posizione_M4 = 0;
  sensore_SX = digitalRead(Sens_Zero_M1);
  sensore_DX = digitalRead(Sens_Zero_M2);
  if( sensore_SX || sensore_DX) {
    Zero_Sollevatore();
    STATO_CORRENTE = 8;
  }
  Serial.println("Zero Avanzamento");
  stepper_M4.setSpeed( VELOCITA_M4_ZERO );
  Accendi_M4();
  sensore_UP = digitalRead(Sens_Zero_M4);
  if( sensore_UP ) stepper_M4.rotate(-1);
  do {
    sensore_UP = digitalRead(Sens_Zero_M4);
     if(! sensore_UP) {
      stepper_M4.stop();
      stepper_M4.setSpeed( VELOCITA_M4_RITORNO );  // Aumenta velocità dopo lo 0
    }
    arresto = Interrompi();
  } while ( stepper_M4.moving() && arresto );
  stepper_M4.rotate(0);
  delay(100);
  if(arresto){
    Posizione_M4 = 1; // Posizione definita OK
    stepper_M4.setZero();
    Serial.println("Zero Avanzamento fatto");
  }
  //stepper_M4.setRampLen( stepsPerRev * 2); // Ramp length is 1/2 revolution
  STATO_CORRENTE = 0;
}

void Accendi_M4(void){
  uint8_t releAttivo;
  releAttivo = digitalRead(Accendi_Driver_M4);
  if(! releAttivo){
    digitalWrite(Accendi_Driver_M4, HIGH);
    delay(1500);
  }
  releAttivo = digitalRead(Collega_M4);
  if(! releAttivo){
    digitalWrite(Collega_M4, HIGH);
    delay(100);
  }
}


void Rotazione_Spazzola(){
  if(! Spazzola) {
    digitalWrite(Accendi_Driver_M3, HIGH);
    delay(1000);
    stepper_M3.rotate(1);
    while (! stepper_M3.moving() );
  }
  else {
    stepper_M3.rotate(0);
    while ( stepper_M3.moving() );
    //digitalWrite(Accendi_Driver_M3, LOW);

  }
  delay(500);
  Spazzola = ! Spazzola;
}

/*
Errore_SP :
1 = Altezza_1_KO per 3 volte
2 = Altezza_2_KO per 3 volte
4 = Allarme Driver M1
8 = Allarme Driver M2
16= Allarme Driver M3
32= Allarme Driver M4

*/
uint8_t Spazzolatura(void)
{
  uint8_t arresta, spazzola_OK, testAltezza;
  lcd.clear();
  digitalWrite(Accendi_Driver_M3, HIGH);  // Accendi Driver Spazzola
  digitalWrite(Ventola, HIGH ); // Accendi Ventola Quadro
  delay(1000);

  //if(! (Posizione_M1 && Posizione_M2) ) {
  //  Serial.println("Zero Sollevamento");
  //  Zero_Sollevatore();
  //}
  Zero_Sollevatore();
  if(! Posizione_M4 ) Zero_Avanzamento();
  STATO_CORRENTE = 4;
  if( Posizione_M1 && Posizione_M2 && Posizione_M4 ) {  //stepper_M4.setSpeed( 875 );
    Accendi_M4();
    testAltezza = Test_Altezza(10000); // Comincia da 10000 e torna indietro per non collidere le ruote "misura altezza"
    if ( ! testAltezza ){
      STATO_CORRENTE = 15;
      return(0);
    }
    
    spazzola_OK = Spazza( 2000 );
    Serial.print("     Spazza=");
    Serial.println(spazzola_OK);

    if( spazzola_OK ) {

      testAltezza = Test_Altezza(6000); // Comincia da 6000 e torna indietro per non collidere le ruote "misura altezza"
      if ( ! testAltezza ){
        STATO_CORRENTE = 15;
        return(0);
      }

      do{
        LCD_Spazzolatura();
        arresta = Interrompi();
      } while ( ( stepper_M1.moving() || stepper_M2.moving() || stepper_M4.moving() ) && arresta );
      stepper_M1.rotate(0);
      stepper_M2.rotate(0);
      stepper_M4.rotate(0);
      if( arresta ) spazzola_OK = Spazza( CORSA_M4 ); //  285500
    }
  }
  digitalWrite(Accendi_Driver_M3, LOW); // Spegni Driver Spazzola
  digitalWrite(Ventola, LOW); // Spegni Ventola Quadro
  STATO_CORRENTE = 0;
  if ( spazzola_OK ) return(1);
  return ( 0 );

}

uint8_t Test_Altezza(uint32_t Run_TO)
{
  uint8_t contaLetture_SX, contaLetture_DX, ritorno = 0;
  long altezza_SX, altezza_DX;
  stepper_M4.write(Run_TO);
  do{ } while ( stepper_M4.moving() && Interrompi() ); // attendi posizionamento M4
  delay(150);
  Serial.print("Test Altezza ... Altezza SX = ");
  do{ 
    altezza_SX = Leggi_Altezza_SX();
    contaLetture_SX++;
    Serial.print(altezza_SX);
    Serial.print(" ... ");
    delay(1000);
  } while ( altezza_SX > 100  && contaLetture_SX < 6 );
  Serial.println(" ");
  Serial.print("Test Altezza ... Altezza DX = ");
  do{ 
    altezza_DX = Leggi_Altezza_DX();
    contaLetture_DX++;
    Serial.print(altezza_DX);
    Serial.print(" ... ");
    delay(1000);
  } while ( altezza_DX > 100  && contaLetture_DX < 6 );
  Serial.println(" ");
  delay(100);
  stepper_M1.write(ALTEZZA_M1); // -2500
  stepper_M2.write(ALTEZZA_M2); // -2500
  do{ } while ( stepper_M1.moving() && stepper_M2.moving() && Interrompi() ); // attendi posizionamento M1 e M2
  
  contaLetture_SX = 0;
  Serial.print("Altezza Appoggiato ... Altezza SX = ");
  do{
    altezza_SX = Leggi_Altezza_SX();
    contaLetture_SX++;
    Serial.print(altezza_SX);
    Serial.print(" ... ");
    delay(1000);
  } while ( ( altezza_SX < 800 || altezza_SX > 1100 ) && ( contaLetture_SX < 6 ) );

  contaLetture_DX = 0;
  Serial.println(" ");
  Serial.print("Altezza Appoggiato ... Altezza DX = ");
  do{
    altezza_DX = Leggi_Altezza_DX();
    contaLetture_DX++;
    Serial.print(altezza_DX);
    Serial.print(" ... ");
    delay(1000);
  } while ( ( altezza_DX < 800 || altezza_DX > 1100 ) && ( contaLetture_DX < 6 ) );
  Serial.println(" ");
  if( contaLetture_SX < 5 &&  contaLetture_DX < 5 ) ritorno = 1;

  return(ritorno);
}

uint8_t Spazza(uint32_t Run_TO)
{
  long old_altezza_1, old_altezza_2, P_M4, Cor_M1, Cor_M2, altezza_1, altezza_2, step1, step2;
  uint8_t arresta, Sens_M4;
  static uint8_t altezza_1_KO, altezza_2_KO, Errore_SP;
  int SX_OK=0, DX_OK=0;
  bool avanti = 0, raggiunto = 0;
  lcd.clear();

  //Serial.print("Inizio Spazzolatura ... verso -> ");
  //Serial.println(Run_TO);
  stepper_M3.rotate(1); // Azione motore Spazzola
  //P_M1 = -3000;
  //P_M2 = -3000;
  P_M4 = stepper_M4.read();
  if ( Run_TO > P_M4 ) avanti = 1;
  CntDis_1 = 0;
  CalcDis_1 = 0;
  CntDis_2 = 0;
  CalcDis_2 = 0;
  altezza_1 = 0;
  altezza_2 = 0;
  do{
    LCD_Spazzolatura ();
    Sens_M4= digitalRead(OltreCorsa_M4);
    if( ! Sens_M4 ) stepper_M4.stop( );
    altezza_1_KO = 0;
    altezza_2_KO = 0;
    if( ! stepper_M1.moving() ){
      altezza_1 = Leggi_Altezza_SX();
      Cor_M1 = 0;
      if(SX_OK > 0 && abs(altezza_1 - old_altezza_1) > 500 ) altezza_1 = 9999;
      else old_altezza_1 = altezza_1;
      if( altezza_1 < 9999 ) {
        Cor_M1 = (altezza_1 / 50) - Abbassamento_SX; // 40 millimetri (80 Maezzi millimetri)   ***************************************************************************************************
        //Cor_M1 = (altezza_1 / 100) - 42; // 42 millimetri
        if((Cor_M1 == 0) && !SX_OK) SX_OK = 1; // Fai partire Carro
        altezza_1_KO = 0;
      }
      else {
        altezza_1_KO ++;
        if( altezza_1_KO > 3 ) Errore_SP &= 1;
      }
      step1 = 0;
      if( Cor_M1 ) {
        step1 = Cor_M1 * 22;
        //if( ! stepper_M1.moving() ) stepper_M1.doSteps( step1 );
        stepper_M1.doSteps( step1 );
      }
    }
    if( ! stepper_M2.moving() ){
      altezza_2 = Leggi_Altezza_DX();
      Cor_M2 = 0;
      if(DX_OK > 0 && abs(altezza_2 - old_altezza_2) > 500 ) altezza_2 = 9999;
      else old_altezza_2 = altezza_2;
      if( altezza_2 < 9999 ) {
        Cor_M2 = (altezza_2 / 50) - Abbassamento_DX; // 47 millimetri (94 mezzi millimetri)   *******************************************************************************************************************
        if((Cor_M2 == 0) && !DX_OK) DX_OK = 1; // Fai partire Carro
        altezza_2_KO = 0;
      }
      else {
        altezza_2_KO ++;
        if( altezza_2_KO > 3 ) Errore_SP &= 2;
      }
      step2 = 0;
      if( Cor_M2 ) {
        step2 =Cor_M2 * 22;
        //if( ! stepper_M2.moving() ) stepper_M2.doSteps( step2 );
        stepper_M2.doSteps( step2 );
      }
    }    
  
    if( SX_OK == 1 && DX_OK == 1) {
      stepper_M4.setSpeed( VELOCITA_M4_SPAZZOLATURA );
      stepper_M4.write(Run_TO);
      SX_OK = 2;
      DX_OK = 2;
    }
    P_M4 = stepper_M4.read();

    if ( Cor_M1 || Cor_M2 ) {
    Serial.print("   ---   Sinistra --- Misura = ");
    Serial.print(altezza_1);
    Serial.print("    Correzione = ");
    Serial.print(Cor_M1);
    Serial.print("    Step -M1 = ");
    Serial.print(step1);
    Serial.print("           ");
      
    Serial.print("Destra --- Misura = ");
    Serial.print(altezza_2);
    Serial.print("    Correzione = ");
    Serial.print(Cor_M2);
    Serial.print("    Step M2 = ");
    Serial.println(step2);  
    }
    if (avanti) {
      if(  P_M4 >= (Run_TO-2) ) raggiunto = 1;
    }
    else {
      if(  P_M4 <= (Run_TO+2) ) raggiunto = 1;
    }
    //if ( (! errore_M1) || (! errore_M2) || (! errore_M3) || (! errore_M4) ) customKey = 'E'; 
    if (  Errore_SP || STATO_CORRENTE > 9 ) {
      Serial.print("STATO_CORRENTE = ");
      Serial.print(STATO_CORRENTE);
      Serial.print("     Errore_SP=");
      Serial.println(Errore_SP);
      ARRESTA = 1;
    }
  arresta = Interrompi();
  } while ( ( ! raggiunto ) && arresta && Errore_SP == 0 ); //  285500 while ( stepper_M4.moving() && ( customKey == ' ' ) ); //while ( customKey == ' ');

  Serial.print("Fine Spazzolatura.     raggiunto=");
  Serial.print(raggiunto);
  Serial.print("   --- STATO_CORRENTE = ");
  Serial.println(STATO_CORRENTE);
  stepper_M1.rotate(0);
  stepper_M2.rotate(0);
  stepper_M3.rotate(0);
  stepper_M4.rotate(0);
  if(! arresta ) return(0);
  stepper_M4.setSpeed( VELOCITA_M4_RITORNO );
  STATO_CORRENTE = 3;
  //stepper_M1.write(0);
  //stepper_M2.write(0);
  Zero_Sollevatore();
  stepper_M4.write(2000); // Ritorno Carro
  do{
    LCD_Spazzolatura ();
    arresta = Interrompi();
    Sens_M4 = digitalRead(Sens_Zero_M4);
    if(! Sens_M4) stepper_M4.stop( );
  } while ( ( stepper_M4.moving() ) && arresta );
  stepper_M1.write(20);
  stepper_M2.write(20);
  do { 
    Sens_M4= digitalRead(Sens_Zero_M4);
    arresta = Interrompi();
    if(!Sens_M4) arresta = 0;
  } while ( ( stepper_M1.moving() || stepper_M2.moving() || stepper_M4.moving() ) && arresta );
  stepper_M1.rotate(0);
  stepper_M2.rotate(0);
  stepper_M4.rotate(0);
  if( ! arresta ) return(0);
  else return(1);
}

uint8_t Interrompi(void) {
  uint8_t customKey, rele_spazzola, rele_avanzamento;
  uint8_t errore_M1, errore_M2, errore_M3, errore_M4, finecorsa;
  bool ritorno=1;
  customKey = GetKey();
  if(customKey != ' ' ) ritorno = 0;
  finecorsa= digitalRead(OltreCorsa_M4);
  if( stepper_M4.moving() && (! finecorsa) ) {
    stepper_M4.stop( );
    Serial.println("Extracorsa M4");
    STATO_CORRENTE = 14; // Extra corsa M4
    ritorno = 0;
  }
  
  errore_M1 = digitalRead( Error_Driver_M1 );
  if (errore_M1) STATO_CORRENTE = 10;
  errore_M2 = digitalRead( Error_Driver_M2 );
  if (errore_M2) STATO_CORRENTE = 11;
  rele_spazzola = digitalRead(Accendi_Driver_M3);
  if(rele_spazzola) {
    errore_M3 = digitalRead( Error_Driver_M3 );
    if (! errore_M3) STATO_CORRENTE = 12;
  }
  rele_avanzamento = digitalRead(Accendi_Driver_M4);
  if(rele_avanzamento) {
    errore_M4 = digitalRead( Error_Driver_M4 );
    if (! errore_M4) STATO_CORRENTE = 13;
  }
  if(STATO_CORRENTE > 9) {
    ritorno = 0;
    Serial.print("STATO_CORRENTE = ");
    Serial.println(STATO_CORRENTE);
  }
  if (ReadDataFromAntenna()) {
    if ( parseAntenna() ) esaminaMessaggio();  // esamina messaggio seriale
    RS232_1_newData = false;
  }
  if(ARRESTA) {
    ritorno = 0;
    ARRESTA = 0;
    Serial.println(" SETTATO ARRESTA ");
  }
  return(ritorno);
}