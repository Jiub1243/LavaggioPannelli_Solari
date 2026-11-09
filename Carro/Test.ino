
void Posiziona_Sollevatore(uint8_t percentuale){
  bool verso = false;
  uint16_t arresta; // 0/-11520
  int32_t posizione, cur_pos, altezza_SX, altezza_DX;
  STATO_CORRENTE = 5;
  //spedisciStringa("01005");
  posizione = ( CORSA_M1M2 / 100 ) * percentuale;
  Serial.println("Muovo Sollevatore Inizio");
  stepper_M1.write(posizione);
  stepper_M2.write(posizione);
  if( stepper_M1.read() > posizione ) verso = true; // Abbassa
  do{
    arresta = Interrompi();
    altezza_SX = Leggi_Altezza_SX();
    if( altezza_SX > 10  && verso ) {
      stepper_M1.stop();
      Serial.print("Toccato sensore SX     verso = ");
      Serial.print(verso);
      Serial.print("   ---   Altezza SX = ");
      Serial.print(altezza_SX);
    }
    altezza_DX = Leggi_Altezza_DX();
    if( altezza_DX > 10  && verso ) {
      stepper_M2.stop();
      Serial.print("Toccato sensore DX     verso = ");
      Serial.print(verso);
      Serial.print("   ---   Altezza DX = ");
      Serial.println(altezza_DX);
    }
    
  } while ( ( stepper_M1.moving() || stepper_M2.moving() ) && arresta );
  stepper_M1.rotate(0);
  stepper_M2.rotate(0);
  Serial.println("Muovo Sollevatore Fine");
  if ( STATO_CORRENTE == 5 ){
    STATO_CORRENTE = 0;
    //spedisciStringa("01000");
  }
  
}

void Posiziona_Avanzamento(uint8_t percentuale){
  bool verso = false;
  uint16_t arresta; // 0/-11520
  int32_t posizione, cur_pos;
  STATO_CORRENTE = 5;
  //spedisciStringa("01005");
  posizione = ( CORSA_M4 / 100 ) * percentuale;
  Serial.println("Muovo Avanzamento Inizio");
  stepper_M4.write(posizione);
  if( stepper_M4.read() < posizione ) verso = true; // Avanti
  Serial.print(" Verso = ");
  Serial.println(verso);
  do{
    arresta = Interrompi();
    
  } while ( stepper_M4.moving() && arresta );
  stepper_M4.rotate(0);
  Serial.println("Muovo Avanzamento Fine");
  if ( STATO_CORRENTE == 5 ){
    STATO_CORRENTE = 0;
    //spedisciStringa("01000");
  }
}

// Fa girare la spazzola a 'percentuale' % della velocità di default (Velocita_Spazzola), solo per la durata del test.
// Rotazione_Spazzola() ripristina la velocità di default richiamando stepper_M3.setSpeed(Velocita_Spazzola),
// quindi qui NON bisogna toccare Velocita_Spazzola stessa, altrimenti il "default" si riduce permanentemente
// ad ogni test fatto a meno del 100% (bug precedente).
void Imposta_Velocita(uint8_t percentuale){
  STATO_CORRENTE = 5;
  //spedisciStringa("01005");
  Serial.println("Test Velocita Spazzola Inizio");

  if(percentuale == 0) {
    stepper_M3.rotate(0);
    while(stepper_M3.moving()) Interrompi(); // continua ad ascoltare antenna/tasti mentre la spazzola rallenta
    digitalWrite(Accendi_Driver_M3, LOW);
    Spazzola = false;
    Velocita_Corrente_M3 = 0;
    Serial.println("Spazzola Fermata");
  }
  else {
    uint16_t velocitaTest = ((uint32_t)Velocita_Spazzola * percentuale) / 100; // casto a int32 per non andare in overflow quando percentuale è 100
    if(!Spazzola) {
      digitalWrite(Accendi_Driver_M3, HIGH);
      delay(1000); // delay cieco VOLUTO: durante l'accensione del driver il pin di errore non è affidabile e Interrompi() non deve campionarlo
    }
    stepper_M3.setSpeed(velocitaTest);
    stepper_M3.rotate(1);
    while(!stepper_M3.moving()) Interrompi(); // continua ad ascoltare antenna/tasti mentre la spazzola accelera
    attesaAttivaTA(500);
    Velocita_Corrente_M3 = velocitaTest;
    Spazzola = true;
    Serial.print("Velocita Spazzola (test) = ");
    Serial.println(velocitaTest);
  }

  Serial.println("Test Velocita Spazzola Fine");
  if ( STATO_CORRENTE == 5 ){
    STATO_CORRENTE = 0;
    //spedisciStringa("01000");
  }
}