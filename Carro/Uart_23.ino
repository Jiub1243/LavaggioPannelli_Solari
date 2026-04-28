// UART2 = seriale lettore altezza spazzola sinistro
// UART3 = seriale lettore altezza spazzola destro
//
// Comando lettura : "H" = restituisci Risposta; "R" = resetta encoder.
// Risposta: "<xxxxx>" xxxxx Altezza mm/100 
//
int32_t Leggi_Altezza_SX()
{
  static uint32_t millis_timeout;
  uint8_t i, tentativi_TX, timeout=0, RX1_OK=0, RX2_OK=0, data1[20];
  int8_t segno;
  int32_t  misura_1 = 9999;

  tentativi_TX = 0;
  while ( Serial2.available() > 0 ) data1[0] = Serial2.read();
  do {
    Serial2.print("H");
    tentativi_TX++;
    //if (debug) Serial.print("   Tentativo seriale 2 = ");
    //if (debug) Serial.println(tentativi_TX);
    for ( i=0; i<10; i++) { 
      data1[i] = 0;
    }
    i = 0;
    millis_timeout = millis() + TIMEOUT_COM1;
    timeout = 0;
    do{
      if( Serial2.available() > 0 ) {
        data1[i]=Serial2.read();
        //if (debug) Serial.print(data1[i]);
        //if (debug) Serial.print("-");
        if( i < 19 ) i++;
      }
      if ( millis() > millis_timeout ) timeout = 1;
    } while( (data1[i-1] !='>') && !timeout );
    //if (debug) Serial.println(" ");
    if( !timeout && data1[0] == '<' && data1[7] == '>' && i==8 ){
      data1[0] = '0';
      segno = 1;
      if(data1[1] == '-') segno = -1;
      data1[1] = '0';
      data1[7] = 0x00;
      RX1_OK = 1;
      misura_1 =  atoi(data1);
      misura_1 = misura_1 * segno;
      //if (debug) Serial.print(" ************************************ Ricevuto messaggio OK = ");
      //if (debug) Serial.println(misura_1);
    }
    if(timeout && debug) Serial.println(" Seriale 2 Timeout");
  } while( (RX1_OK == 0) && (tentativi_TX < 3) );

  return(misura_1);
}

int32_t Leggi_Altezza_DX()
{
  uint32_t millis_timeout;
  uint8_t i, tentativi_TX, timeout=0, RX2_OK=0, data2[20];
  int8_t segno;
  int32_t  misura_2 = 9999;

  tentativi_TX = 0;
  while ( Serial3.available() > 0 ) data2[0] = Serial3.read();
  do {
    Serial3.print("H");
    tentativi_TX++;
    //Serial.print("   Tentativo seriale 3 = ");
    //Serial.println(tentativi_TX);
    for ( i=0; i<10; i++) { 
      data2[i] = 0;
    }
    i = 0;
    millis_timeout = millis() + TIMEOUT_COM1;
    timeout = 0;
    do{
      if( Serial3.available() ) {
        data2[i]=Serial3.read();
        //Serial.print(data2[i]);
        //Serial.print("-");
        if( i < 19 ) i++;
      }
      if (millis() > millis_timeout ) timeout = 1;
    } while( (data2[i-1] !='>') && !timeout );

    if( !timeout && data2[0] == '<' && data2[7] == '>' && i==8 ) {
      data2[0] = '0';
      segno = 1;
      if(data2[1] == '-') segno = -1;
      data2[1] = '0';
      data2[7] = 0x00;
      RX2_OK = 1;
      misura_2 = atoi( data2 );
      misura_2 = misura_2 * segno;
      /*Serial.print("   Dato Seriale 3 OK = ");
      Serial.print(misura_2);
      Serial.print("  segno = ");
      Serial.print(segno);
      Serial.print("  ");
      Serial.println(data2[1]);*/
    }
  } while( (RX2_OK == 0) && (tentativi_TX < 3) );

  return(misura_2);
}

//
// Leggo dalla seriale dei sensori di Altezza Spazzola  ----> FINE
//

