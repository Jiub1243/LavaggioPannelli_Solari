/*changelog
ho aggiornato l'assegnazione di Messaggio_OK in ReadDataFromAntenna
ho implementato i comandi 06-11
TODO alla fine della spazzolatura, se spazzola_OK allora stato = 1 anziché 0? Però poi devo cambiare tutti i controlli if stato == 0
*/

/* UART 1 = seriale wireless LORA
*
*   ESEMPIO Stringa TX: AT+SEND=1,5,11999
*
*
*  Messaggio:-Risposte:
*         xx          rr         yyy
*         01 = Richiesta STATO  001
*                     01   000 = Idle
*                     01   001 = Idle Terminata spazzolatura colonna
*                     01   002 = Azzeramento Avanzamento (Non viene mai assegnato in Carro.ino, si usa 008 in realtà)
*                     01   003 = Rientro Carro dopo spazzolatura               
*                     01   004 = Spazzolatura
*                     01   005 = Test
*                     01   006 = Allarme driver Avanzamento
*                     01   007 = Zero Sollevamento
*                     01   008 = Zero Avanzamento 
*                     01   009 = Comando non eseguibile
*                     01   010 = Allarme Driver Sollevamento SX
*                     01   011 = Allarme Driver Sollevamento DX
*                     01   012 = Allarme Driver Spazzola
*                     01   013 = Allarme Driver Avanzamento
*                     01   014 = Extra Corsa M4
*                     01   015 = Allarme Lettura Altezza Sollevatore
*                     03   ppp = spazzolatura, percentuale eseguita ppp
*         
*         
*         02 = Azzera Asse      001 = Azzera Sollevamento
*                     01   001 = Azzeramento Sollevatore
*                     01   009 = Comando non eseguibile
*         02 = Azzera Asse      002 = Azzera Avanzamento
*                     01   002 = Azzeramento Carro
*                     01   009 = Comando non eseguibile
*         02 = Azzera comando   003 = Termina qualsiasi comando in esecuzione
*                     01   000 = Idle
*
*         03 = Pulizia          001 = Esegui pulizia della  colonna corrente
*                     03   000 = Comando OK (Percentuale esecuzione comando) 
*                     01   009 = Comando non eseguibile
*         04 = TEST Sollevatore xxx = Percentuale altezza (0/100% = 0/-11520)
*                     04   001 = Comando ok
*                     01   009 = Comando non eseguibile
*         05 = TEST Avanzamento xxx = Percentuale avanzamento (0/100% = 0/285500)
*                     04   001 = Comando ok
*                     01   009 = Comando non eseguibile
*         06 = TEST Spazzola    xxx = Percentuale velocità rotazione (0/100% = 0/800)  
*         07 = RQST Sollevatore 001 = Richiesta Percentuale altezza Sollevatore SX
*                     07   xxx = Percentuale altezza Sollevatore SX (0/100% = 0/-11520)
*         08 = RQST Sollevatore 001 = Richiesta Percentuale altezza Sollevatore DX
*                     08   xxx = Percentuale altezza Sollevatore DX (0/100% = 0/-11520)
*         09 = RQST Avanzamento 001 = Richiesta Percentuale avanzamento
*                     09   xxx = Percentuale avanzamento (0/100% = 0/285500)
*         10 = RQST Spazzola    001 = Richiesta Percentuale velocità rotazione
*                     10   xxx = Percentuale velocità rotazione spazzola (0/100% = 0/3750)
*         11 = Altezza SX       xxx = Setta l'altezza SX della spazzola (1/2 millimetro)
*                               0-255 -> setta l'altezza di spazzolamento SX
*                               999   -> Ritorna il settaggio corrente
*                     11   xxx = Altezza settata
*                     01   009 = Comando non eseguibile
*         12 = Altezza DX       xxx = Setta l'altezza DX della spazzola (1/2 millimetro)
*                               0-255 -> setta l'altezza di spazzolamento DX
*                               999   -> Ritorna il settaggio corrente
*                     11   xxx = Altezza settata
*                     01   009 = Comando non eseguibile
*
*         13 = Set CORSA_M1M2   xxx = Nuovo modulo CORSA_M1M2 / 100 (salvata negativa)
*                     13   xxx = Valore confermato
*                     01   009 = Comando non eseguibile
*
*         14 = Set CORSA_M4     xxx = Nuovo valore CORSA_M4 / 1000
*                     14   xxx = Valore confermato
*                     01   009 = Comando non eseguibile
*
*         15 = RQST CORSA_M1M2  001 = Richiesta valore corrente
*                     15   xxx = |CORSA_M1M2| / 100
*
*         16 = RQST CORSA_M4    001 = Richiesta valore corrente
*                     16   xxx = CORSA_M4 / 1000
*
*
*         
*         
* 
* 
*/

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

    if( rc == MESS_START_MESS )
    {
      myMessIndex = 0;
      RS232_1_buffer_RX[ myMessIndex ] = rc;
      myMessIndex++;

      RecvInProgress = true;
    }
    else if( RecvInProgress == true )
    {
      RS232_1_buffer_RX[ myMessIndex ] = rc;
      myMessIndex++;

      if( rc != MESS_END_MESS )
      {
        if( myMessIndex >= LUN_BUF_232 )
        {
          myMessIndex = LUN_BUF_232 - 1;
        }
      }
      else if( myMessIndex == MESS_LUN_OK )
      {
        if( RS232_1_buffer_RX[ myMessIndex -4 ] == 'O' && RS232_1_buffer_RX[ myMessIndex -3 ] == 'K' ) Messaggio_OK = 1;
        RecvInProgress = false;
        myMessIndex = 0;
        Serial.println( " == MESS_LUN_OK " );
      }
      else if( myMessIndex <= MESS_LUN_ERR )
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

        if( myMessIndex >= MESS_LUN_REC )
        {
          strncpy( verifica, RS232_1_buffer_RX, 9 );
          verifica[9]=0x00;
          if( memcmp( verifica, "+RCV=0,5," ,9 ) == 0 ) {
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
        else
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
//
int parseAntenna()
{
  int    myIndex;
  char   myMittente, myComando[ 6 ], com[3], dat[4];
  
  RS232_1_newData = false;

  //strncpy( fromHost.piu, RS232_1_buffer_RX, 5 );
  myMittente = RS232_1_buffer_RX[ 5 ];
  strncpy( myComando, &RS232_1_buffer_RX[9], 5 );
  myComando[ 5 ] = 0x00;
  strncpy( com, &RS232_1_buffer_RX[9], 2 );
  com[ 2 ] = 0x00;
  strncpy( dat, &RS232_1_buffer_RX[11], 3 );
  dat[ 3 ] = 0x00;
  
    
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
    Stato = stepper_M4.read();
    //Stato = (CORSA_M4 / Stato * 100) + 1000; TODO mi sembra che questa faccia l'opposto di quello che vogliamo
    Stato = (Stato / CORSA_M4 * 100) + 1000;
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
  if(datiHost.comando == 2 && datiHost.dato == 001) {
    if(STATO_CORRENTE == 0 || STATO_CORRENTE == 15) {
      spedisciStringa("01001"); // Zero Sollevatore
      Zero_Sollevatore();
    }
    else spedisciStringa("01009"); // Comando non eseguibile
  }
  if(datiHost.comando == 2 && datiHost.dato == 002) {
    if(STATO_CORRENTE == 0 || STATO_CORRENTE == 15) {
      spedisciStringa("01002"); // Zero Avanzamento
      Zero_Avanzamento();
    }
    else spedisciStringa("01009"); // Comando non eseguibile
  }
  if(datiHost.comando == 3 && datiHost.dato == 001) {
    if(STATO_CORRENTE == 0 || STATO_CORRENTE == 1) {
      spedisciStringa("03000"); // Spazzolatura + percentuale 0%
      Spazzolatura();
    }
    else spedisciStringa("01009"); // Comando non eseguibile
  }
  if(datiHost.comando == 2 && datiHost.dato == 003) {
    if(STATO_CORRENTE != 0) {
      spedisciStringa("02003"); // Interrompi qualsiasi esecuzione
      if( STATO_CORRENTE < 9 ) ARRESTA = 1;
      STATO_CORRENTE = 0;
      Serial.println("Arresta Esecuzione Comando");
    }
    else spedisciStringa("01009"); // Comando non eseguibile
    Serial.print("Stato Corrente = ");
    Serial.println(STATO_CORRENTE);
  }
  if(datiHost.comando == 4 && datiHost.dato < 101 ) { //TEST Sollevatore
    if( (STATO_CORRENTE == 0) && Posizione_M1 && Posizione_M2 ) {
      spedisciStringa("04001"); // Posiziona sollevatore OK
      Posiziona_Sollevatore(datiHost.dato);
    }
    else spedisciStringa("01009"); // Comando non eseguibile
  }
  if(datiHost.comando == 5 && datiHost.dato < 101 ) { // TEST avanzamento
    if( (STATO_CORRENTE == 0) && Posizione_M4 ) {
      spedisciStringa("05001"); // Posiziona sollevatore OK
      Posiziona_Avanzamento(datiHost.dato);
    }
    else spedisciStringa("01009"); // Comando non eseguibile
  }

  if(datiHost.comando == 6 && datiHost.dato < 101 ) { // TEST velocità spazzola
    if( (STATO_CORRENTE == 0) ) {
      spedisciStringa("06001"); // Posiziona sollevatore OK
      Imposta_Velocita(datiHost.dato);
    }
    else spedisciStringa("01009"); // Comando non eseguibile
  }

  if(datiHost.comando == 7 && datiHost.dato == 1) { // RQST Sollevatore SX
    int32_t percentuale = (stepper_M1.read() * 100) / CORSA_M1M2;
    if(percentuale < 0) percentuale = 0;
    if(percentuale > 100) percentuale = 100;
    Stato = (uint32_t)percentuale + 1000;
    DaSpedire = String(Stato);
    DaSpedire = DaSpedire.substring(1,4);
    DaSpedire = "07" + DaSpedire;
    spedisciStringa(DaSpedire);
    Serial.println(DaSpedire);
  }
  if(datiHost.comando == 8 && datiHost.dato == 1) { // RQST Sollevatore DX
    int32_t percentuale = (stepper_M2.read() * 100) / CORSA_M1M2;
    if(percentuale < 0) percentuale = 0;
    if(percentuale > 100) percentuale = 100;
    Stato = (uint32_t)percentuale + 1000;
    DaSpedire = String(Stato);
    DaSpedire = DaSpedire.substring(1,4);
    DaSpedire = "08" + DaSpedire;
    spedisciStringa(DaSpedire);
    Serial.println(DaSpedire);
  }
  if(datiHost.comando == 9 && datiHost.dato == 1) { // RQST Avanzamento
    int32_t percentuale = (stepper_M4.read() * 100) / CORSA_M4;
    if(percentuale < 0) percentuale = 0;
    if(percentuale > 100) percentuale = 100;
    Stato = (uint32_t)percentuale + 1000;
    DaSpedire = String(Stato);
    DaSpedire = DaSpedire.substring(1,4);
    DaSpedire = "09" + DaSpedire;
    spedisciStringa(DaSpedire);
    Serial.println(DaSpedire);
  }
  if(datiHost.comando == 10 && datiHost.dato == 1) { // RQST Velocità Spazzola
    uint32_t percentuale = ((uint32_t)Velocita_Corrente_M3 * 100) / VELOCITA_MAX;
    if(percentuale > 100) percentuale = 100;
    Stato = percentuale + 1000;
    DaSpedire = String(Stato);
    DaSpedire = DaSpedire.substring(1,4);
    DaSpedire = "10" + DaSpedire;
    spedisciStringa(DaSpedire);
    Serial.println(DaSpedire);
  }

  if(datiHost.comando == 11 ) {  // Settaggio Altezza Spazzolatura SX
    if( datiHost.dato <= 255 ) {
      Abbassamento_SX = datiHost.dato;
      EEPROM.write(513, Abbassamento_SX);
    }
    if( datiHost.dato <= 255 || datiHost.dato == 999 ) {
      Stato = Abbassamento_SX + 1000;
      DaSpedire = String( Stato );
      DaSpedire = DaSpedire.substring(1,4);
      DaSpedire = "11"+DaSpedire;
      spedisciStringa(DaSpedire);
    }
    else spedisciStringa("01009"); // Comando non eseguibile
  }
  if(datiHost.comando == 12 ) {  // Settaggio Altezza Spazzolatura DX
    if( datiHost.dato <= 255 ) {
      Abbassamento_DX = datiHost.dato;
      EEPROM.write(514, Abbassamento_DX);
    }
    if( datiHost.dato <= 255 || datiHost.dato == 999 ) {
      Stato = Abbassamento_DX + 1000;
      DaSpedire = String( Stato );
      DaSpedire = DaSpedire.substring(1,4);
      DaSpedire = "12"+DaSpedire;
      spedisciStringa(DaSpedire);
    }
    else spedisciStringa("01009"); // Comando non eseguibile
  }

  if (datiHost.comando == 13) { // Nuovo valore CORSA_M1M2 (modulo/100, salvata negativa)
    if (STATO_CORRENTE == 0 && datiHost.dato > 0) {
      CORSA_M1M2 = -((long)datiHost.dato * 100);
      EEPROM.put(EE_ADDR_CORSA_M1M2, CORSA_M1M2);
      char buf[6];
      snprintf(buf, sizeof(buf), "13%03u", datiHost.dato);
      spedisciStringa(buf); // conferma
    } else spedisciStringa("01009"); // Comando non eseguibile
  }

  if (datiHost.comando == 14) { // Nuovo valore CORSA_M4 (valore/1000)
    if (STATO_CORRENTE == 0 && datiHost.dato > 0) {
      CORSA_M4 = (long)datiHost.dato * 1000;
      EEPROM.put(EE_ADDR_CORSA_M4, CORSA_M4);
      char buf[6];
      snprintf(buf, sizeof(buf), "14%03u", datiHost.dato);
      spedisciStringa(buf); // conferma
    } else spedisciStringa("01009"); // Comando non eseguibile
  }

  if (datiHost.comando == 15) { // Richiesta CORSA_M1M2
    char buf[6];
    uint16_t val = (uint16_t)(-CORSA_M1M2 / 100);
    snprintf(buf, sizeof(buf), "15%03u", val);
    spedisciStringa(buf);
  }

  if (datiHost.comando == 16) { // Richiesta CORSA_M4
    char buf[6];
    uint16_t val = (uint16_t)(CORSA_M4 / 1000);
    snprintf(buf, sizeof(buf), "16%03u", val);
    spedisciStringa(buf);
  }
}
//
// seriale wireless LORA  ----> FINE
//

