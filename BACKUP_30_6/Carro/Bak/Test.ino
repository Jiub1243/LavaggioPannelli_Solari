
void Posiziona_Sollevatore(uint8_t percentuale){
  bool verso = false;
  uint16_t arresta; // 0/-11520
  int32_t posizione, cur_pos, altezza_SX, altezza_DX;
  STATO_CORRENTE = 5;
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
  if ( STATO_CORRENTE == 5 ) STATO_CORRENTE = 0;
}

void Posiziona_Avanzamento(uint8_t percentuale){
  bool verso = false;
  uint16_t arresta; // 0/-11520
  int32_t posizione, cur_pos;
  STATO_CORRENTE = 5;
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
  if ( STATO_CORRENTE == 5 ) STATO_CORRENTE = 0;
}