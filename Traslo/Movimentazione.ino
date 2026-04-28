/*changelog
in Vai_Alla_Colonna ho aggiunto un update di Colonna
Aggiunta l'assegnazione a 0 di Traslo_OK, così non resta 1 per sempre
*/

// Ritorna all posizione 0
void Zero_Traslo(void){
  uint8_t sensore_zero, sensore_sotto, arresto;
  bool Posizione_M1, Posizione_M2;
  Posizione_M1 = 0;
  Posizione_M2 = 0;
  STATO_CORRENTE =  1;
  Traslo_OK = 0;
  Serial.println("Zero Traslo");
  stepper_M1.setSpeed( VELOCITA_M1_ZERO );
  stepper_M2.setSpeed( VELOCITA_M2_ZERO );
  sensore_zero = digitalRead(Sensore_Zero_M1);
  sensore_sotto = digitalRead(Sensore_Posizione_M2);
  if( sensore_zero ) stepper_M1.rotate(1);
  if( sensore_sotto ) stepper_M2.rotate(1);
  do{
    sensore_zero = digitalRead(Sensore_Zero_M1);
    if(! sensore_zero) {
      stepper_M1.stop();
      stepper_M2.stop();
    }
    arresto = Interrompi();
  } while ( ( stepper_M1.moving() || stepper_M2.moving() ) && arresto );
  Serial.println("Toccato sensore Zero M1...");
  // Allontanati dai sensori induttivi
  stepper_M1.doSteps( -500 );
  stepper_M2.doSteps( -289 );
  do{ 
    arresto = Interrompi(); 
  } while ( ( stepper_M1.moving() || stepper_M2.moving() ) && arresto );
  Serial.println("Allontanato dai sensori ... Ritorno ...");
  if ( arresto ) {
    stepper_M1.rotate(1);
    stepper_M2.rotate(1);
    do{
      sensore_zero = digitalRead(Sensore_Zero_M1);
      if(! sensore_zero) {
        stepper_M1.stop();
        Posizione_M1 = 1;
        stepper_M1.setZero();
      }
      sensore_sotto = digitalRead(Sensore_Posizione_M2);
      if(! sensore_sotto) {
        stepper_M2.stop();
        Posizione_M2 = 1;
        stepper_M2.setZero();
      }
      arresto = Interrompi(); 
    } while ( ( stepper_M1.moving() || stepper_M2.moving() ) && arresto );
  }
  stepper_M1.rotate(0);
  stepper_M2.rotate(0);
  do{ 
    arresto = Interrompi(); 
  } while ( ( stepper_M1.moving() || stepper_M2.moving() ) && arresto );
  Serial.println("FINE ZERO TRASLO");
  if( ! Posizione_M1 ) STATO_CORRENTE = 12; // Allarme Zero M1
  else if( ! Posizione_M2 ) STATO_CORRENTE = 13; // Allarme Zero M2
  else { 
    Traslo_OK = 1;
    STATO_CORRENTE = 0;
  }
  stepper_M1.setSpeed( VELOCITA_M1_AVANZAMENTO );
  stepper_M2.setSpeed( VELOCITA_M2_AVANZAMENTO );
}

// Se viene rilevato un errore dei drivers  (e se viene premuto un qualsiasi pulsante?)
// ritorna 0, printa lo stato e in caso arresta il sistema
// altrimenti ritorna 1
uint8_t Interrompi(void) {
  uint8_t customKey;
  uint8_t errore_M1, errore_M2;
  bool ritorno=1;
  customKey = GetKey();
  if(customKey != ' ' ) ritorno = 0; // Così basta premere un qualsiasi tasto per interrompere
  errore_M1 = digitalRead( Error_Driver_M1 ); // Allarme Driver Superiore
  if (! errore_M1) STATO_CORRENTE = 10;
  errore_M2 = digitalRead( Error_Driver_M2 ); // Allarme Driver Inferiore
  if (! errore_M2) STATO_CORRENTE = 11;
  if(STATO_CORRENTE > 9) {
    ritorno = 0;
    Serial.print("STATO_CORRENTE = ");
    Serial.println(STATO_CORRENTE);
  }
  if (ReadDataFromAntenna()) {
    if ( parseAntenna() ) esaminaMessaggio();  // esamina messaggio seriale
    RS232_1_newData = false;
  }
  if(ARRESTA) { // Se ho ricevuto una richiesta di arresto mentre 0 < STATO_CORRENTE < 9
    ritorno = 0;
    ARRESTA = 0;
    Serial.println(" SETTATO ARRESTA ");
  }
  return(ritorno);
}

// Trasla fino alla colonna desiderata
// poi aggiorna il valore di 'Colonna'
// ritorna 0 se non ci sono problemi
// altrimenti ritorna 1
uint8_t Vai_Alla_Colonna( uint8_t alla_Colonna )
{
  uint8_t arresto, ritorno = 0;
  int32_t passi_M1, passi_M2; //passi necessari per arivare alla colonna
  if( alla_Colonna > 0 ) {
    STATO_CORRENTE = 3; //Movimentazione Traslo
    passi_M1 = ( ( alla_Colonna - Colonna ) * passi_Colonna_M1 );
    passi_M2 = ( ( alla_Colonna - Colonna ) * passi_Colonna_M2 );
    stepper_M1.doSteps( passi_M1 );
    stepper_M2.doSteps( passi_M2 );
    do{ 
      arresto = Interrompi(); // arresto è 0 se Interrompi() ha riscontrato un errore
    } while ( ( stepper_M1.moving() || stepper_M2.moving() ) && arresto );
    if ( arresto ) { //il carrello si è arrestato alla posizione desiderata senza errori
      STATO_CORRENTE = 0;
      ritorno = 1;
      Colonna = alla_Colonna;
    }
  }
  else {
    STATO_CORRENTE = 2; //Movimentazione Traslo verso Home
    stepper_M1.write( 0 );
    stepper_M2.write( 0 );
     do{ 
      arresto = Interrompi(); 
    } while ( ( stepper_M1.moving() || stepper_M2.moving() ) && arresto );
    if ( arresto ) { //il carrello si è arrestato alla posizione desiderata senza errori
      STATO_CORRENTE = 0;
      Colonna = 0;
      ritorno = 1;
    }
  } 
  return(ritorno);
}
