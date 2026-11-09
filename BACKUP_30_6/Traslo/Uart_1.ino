/*changelog
ho aggiornato l'assegnazione di Messaggio_OK in ReadDataFromAntenna
*/

/* UART 1 = seriale wireless LORA
*
*
*  Messaggio:-Risposte:
*         xx         rr         yyy
*         01 = Richiesta STATO  001
*                     01   000 = Idle
*                     01   001 = Azzeramento Traslo
*                     01   002 = Rientro Carro dopo spazzolatura
*                     01   003 = Spostamento 
*                     01   005 = Test                 
*                     01   010 = Allarme driver Superiore
*                     01   011 = Allarme driver Inferiore
*                     01   012 = Allarme Zero M1
*                     01   013 = Allarme Zero M2 
*                     01   014 = Extra Corsa Traslo
*                     03   ppp = spostamento, verso colonna ppp
*
*         02 = Azzera Asse      001 = Azzera Traslo
*                     01   001 = Azzeramento Traslo
*                     01   009 = Comando non eseguibile
*         02 = Azzera comando   003 = Termina qualsiasi comando in esecuzione
*                     01   000 = Idle
*
*         03 = Spostamento      xxx = Raggiungi colonna xxx
*                               000 = Ritorno alla posizione Home
*                               999 = Colonna Corrente
*                     03   xxx = Colonna Corrente
*                     01   009 = Comando non eseguibile
*         
*         04 = Set passi_Colonna_M1    xxx = Nuovo valore passi_Colonna_M1 / 10
*                     04   xxx = Valore confermato
*                     01   009 = Comando non eseguibile
*
*         05 = Set passi_Colonna_M2    xxx = Nuovo valore passi_Colonna_M2 / 10
*                     05   xxx = Valore confermato
*                     01   009 = Comando non eseguibile 
*
*         06 = Set Set maxCol          xxx = Nuovo valore maxCol
*                     06   xxx = Valore confermato
*                     01   009 = Comando non eseguibile
*
*         13 = RQST passi_Colonna_M1   001 = Richiesta valore corrente
*                     13   xxx = passi_Colonna_M1 / 10
*
*         14 = RQST passi_Colonna_M2   001 = Richiesta valore corrente
*                     14   xxx = passi_Colonna_M2 / 10
*
*         15 = RQST maxCol             001 = Richiesta valore corrente
*                     15   xxx = maxCol
* 
*/

// Aspetta di ricevere il carattere 'MESS_START_MESS' prima di iniziare a registrare un messaggio
// ritorna 1 se la ricezione ha successo, 0 altrimenti
uint8_t ReadDataFromAntenna()
{
  static bool RecvInProgress = false;
  static byte myMessIndex = 0;
  char rc;
  char verifica[25];
  uint8_t ritorno = 0;

  while( Serial1.available() > 0 )
  {
    rc = Serial1.read();

    if( rc == MESS_START_MESS ) // inizio messaggio
    {
      myMessIndex = 0;
      RS232_1_buffer_RX[ myMessIndex ] = rc;
      myMessIndex++;

      RecvInProgress = true;
    }
    else if( RecvInProgress == true ) // se avevamo già iniziato il messaggio
    {
      RS232_1_buffer_RX[ myMessIndex ] = rc;
      myMessIndex++;

      if( rc != MESS_END_MESS ) // se non abbiamo finito di trasmettere
      {
        if( myMessIndex >= LUN_BUF_232 ) // non aumentare la lunghezza del messaggio se abbiamo superato il limite, riscrivi l'ultimo carattere
        {
          myMessIndex = LUN_BUF_232 - 1;
        }
      }
      else if( myMessIndex == MESS_LUN_OK ) // se abbiamo mandato un messaggio di 5 byte con 'OK' interrompi la ricezione
      {
        if( RS232_1_buffer_RX[ myMessIndex -4 ] == 'O' && RS232_1_buffer_RX[ myMessIndex -3 ] == 'K' ) Messaggio_OK = 1;
        RecvInProgress = false;
        myMessIndex = 0;
        Serial.println( " == MESS_LUN_OK " );
      }
      else if( myMessIndex <= MESS_LUN_ERR ) // se abbiamo mandato meno di 10 byte segnala un errore
      {
        //if( RS232_2_buffer_RX[ 2 ] = 'E' && RS232_2_buffer_RX[ 3 ] = 'R' ) Messaggio_OK = 0;
        Messaggio_OK = 0;
        RecvInProgress = false;
        myMessIndex = 0;
        Serial.print( " <= MESS_LUN_ERR - MyMessIndex = " );
        Serial.println(myMessIndex);
      }
      else
      {
        RS232_1_buffer_RX[ myMessIndex ] = '\0'; // terminate the string

        if( myMessIndex >= MESS_LUN_REC ) // se abbiamo mandato più di 15 byte
        {
          strncpy( verifica, RS232_1_buffer_RX, 9 );
          verifica[9]=0x00;
          if( memcmp( verifica, "+RCV=0,5," ,9 ) == 0 ) { // se abbiamo ricevuto 5 byte dal nodo 0, ricezione avvenuta con successo
            RS232_1_newData = true;
            ritorno = true;
          }
          /*
          Serial.print( "  Verifica: " );
          Serial.print( verifica );
          Serial.print( "   Antenna: RX Messaggio : " );
          Serial.print( myMessIndex );
          Serial.print( " --- " );
          Serial.println( RS232_1_buffer_RX );*/
        }
        else // qualcosa non va
        {
          //Debug
          Serial.print( "Antenna: lunghezza dati errata: " );
          Serial.print( myMessIndex );
          Serial.print( " " );
          Serial.println( RS232_1_buffer_RX );
        }

        RecvInProgress = false;
        myMessIndex = 0;
      }
    }
  }
  return(ritorno);
}
//
// Leggo dalla seriale ----> FINE
//


//
// Analizzo il contenuto del messaggio ricevuto.
// Buffer:  + R C V = M , 5 , C C D D D \n    M deve essere 0 
// Index:   0 1 2 3 4 5 6 7 8 9 10 11 12 13
// ritorna 1 in caso di successo, altrimenti 0
int parseAntenna()
{
  int    myIndex;
  char   myMittente, myComando[ 6 ], com[3], dat[4];
  
  RS232_1_newData = false;

  //strncpy( fromHost.piu, RS232_1_buffer_RX, 5 );
  myMittente = RS232_1_buffer_RX[ 5 ];
  strncpy( myComando, &RS232_1_buffer_RX[9], 5 );
  myComando[ 5 ] = 0x00; // C C D D D
  strncpy( com, &RS232_1_buffer_RX[9], 2 );
  com[ 2 ] = 0x00; // C C
  strncpy( dat, &RS232_1_buffer_RX[11], 3 );
  dat[ 3 ] = 0x00; // D D D
  
    
  if( myMittente != '0' )// Host
  //&& (memcmp(myComando, "RCV", 3 ) ) )
  {
    Serial.print( "Mittente: " );
    Serial.print( myMittente );
    Serial.print( "   -   Tipo Comando: " );
    Serial.print( myComando );
    Serial.print( "    " );
    Serial.println( "Non e' per me" );
    return ( 0 );
  }
 
  
  //
  // Prima di uscire salvo mittente(telecomando) e comando in una struttura di appoggio.
  //
  datiHost.mittente = myMittente;
  strcpy( datiHost.messaggio, myComando );
  datiHost.comando = atoi(com);
  datiHost.dato = atoi(dat);
  return( 1 );
}


void spedisciStringa( String stringaDaSpedire )
{
  Serial1.print( "AT+SEND=0,5," );
  Serial1.println( stringaDaSpedire );
}

void esaminaMessaggio( void )
{
  String DaSpedire;
  uint32_t Stato;
  Serial.print( "Ricevuto messaggio: " );
  Serial.print( datiHost.messaggio );
  Serial.print( "   Comando = " );
  Serial.print( datiHost.comando );
  Serial.print( "   dato = " );
  Serial.println( datiHost.dato );

  if(datiHost.comando == 1 && datiHost.dato == 001) { // Richiesta Stato
  if(STATO_CORRENTE == 3){ // Se spazzolatura in corso ...
    Stato = Colonna+1000;
    DaSpedire = String( Stato );
    DaSpedire = DaSpedire.substring(1,4);
    DaSpedire = "03"+DaSpedire;
  }
  else{
    Stato = STATO_CORRENTE + 1000;
    DaSpedire = String( Stato );
    DaSpedire = DaSpedire.substring(1,4);
    DaSpedire = "01"+DaSpedire;
  }
  spedisciStringa(DaSpedire);
  Serial.println(DaSpedire);
  }
  if(datiHost.comando == 2 && datiHost.dato == 001) { // Zero traslo
    if(STATO_CORRENTE == 0) {
      spedisciStringa("01001"); // Azzeramento Traslo
      Zero_Traslo();
    }
    else spedisciStringa("01009"); // Comando non eseguibile
  }

  if(datiHost.comando == 2 && datiHost.dato == 003) { //Termina qualsiasi comando in esecuzione
    if(STATO_CORRENTE != 0) {
      spedisciStringa("01000"); // Idle
      if( STATO_CORRENTE < 9 ) ARRESTA = 1;
      STATO_CORRENTE = 0;
    }
    else spedisciStringa("01009"); // Comando non eseguibile
    Serial.print("Stato Corrente = ");
    Serial.println(STATO_CORRENTE);
  }
  
  if(datiHost.comando == 3 ) { // aggiungi colonna xxx
    if( datiHost.dato <= maxCol || datiHost.dato == 999 ) {
        if( datiHost.dato <= maxCol && Traslo_OK && !STATO_CORRENTE ) { // Colonne pannelli
        if( ! Vai_Alla_Colonna(datiHost.dato) ) ;
      }
      Stato = Colonna +1000;
      DaSpedire = String( Stato );
      DaSpedire = DaSpedire.substring(1,4);
      DaSpedire = "03"+DaSpedire;
      spedisciStringa(DaSpedire); // Settaggio Altezza Spazzolatura SX
    }
    else spedisciStringa("01009"); // Comando non eseguibile
  }

  if (datiHost.comando == 4) { // Nuovo valore passi_Colonna_M1
    if (STATO_CORRENTE == 0 && datiHost.dato > 0) {
      passi_Colonna_M1 = (long)datiHost.dato * 10;
      EEPROM.put(EE_ADDR_PASSI_M1, passi_Colonna_M1);
      char buf[6];
      snprintf(buf, sizeof(buf), "04%03u", datiHost.dato);
      spedisciStringa(buf); // conferma
    } else spedisciStringa("01009");
  }

  if (datiHost.comando == 5) { // Nuovo valore passi_Colonna_M2
    if (STATO_CORRENTE == 0 && datiHost.dato > 0) {
      passi_Colonna_M2 = (long)datiHost.dato * 10;
      EEPROM.put(EE_ADDR_PASSI_M2, passi_Colonna_M2);
      char buf[6];
      snprintf(buf, sizeof(buf), "05%03u", datiHost.dato);
      spedisciStringa(buf);
    } else spedisciStringa("01009");
  }

  if (datiHost.comando == 6) { // Nuovo valore maxCol
    if (STATO_CORRENTE == 0 && datiHost.dato > 0) {
      maxCol = datiHost.dato;
      EEPROM.write(EE_ADDR_MAXCOL, maxCol);
      char buf[6];
      snprintf(buf, sizeof(buf), "06%03u", datiHost.dato);
      spedisciStringa(buf);
    } else spedisciStringa("01009");
  }

  if (datiHost.comando == 13) { // Richiesta passi_Colonna_M1
    char buf[6];
    uint16_t val = (uint16_t)(passi_Colonna_M1 / 10);
    snprintf(buf, sizeof(buf), "13%03u", val);
    spedisciStringa(buf);
  }

  if (datiHost.comando == 14) { // Richiesta passi_Colonna_M2
    char buf[6];
    uint16_t val = (uint16_t)(passi_Colonna_M2 / 10);
    snprintf(buf, sizeof(buf), "14%03u", val);
    spedisciStringa(buf);
  }

  if (datiHost.comando == 15) { // Richiesta maxCol
    char buf[6];
    snprintf(buf, sizeof(buf), "15%03u", maxCol);
    spedisciStringa(buf);
  }
}
//
// seriale wireless LORA  ----> FINE
//

