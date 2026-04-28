/* UART 1 = seriale wireless LORA
*
*
*  Messaggio:-Risposte:
*         xx         rr         yyy
*         01 = Richiesta STATO  001
*                     01   000 = Idle
*                     01   001 = Azzeramento Traslo
*                     01   002 = Rientro Carro dopo spazzolatura               
*                     01   003 = Allarme driver Superiore
*                     01   004 = Allarme driver Inferiore
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
*  STATO_CORRENTE:
*         000 = Ilde
*         001 = Zero Traslo
*         002 = Ritorno
*         003 = Spostamento
*         005 = Test
*         010 = Allarme Driver Superiore
*         011 = Allarme Driver Inferiore
*         012 = Allarme Zero M1
*         013 = Allarme Zero M2
*         014 = Extra Corsa Traslo
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
        if( RS232_1_buffer_RX[ myMessIndex -4 ] == 'O' && RS232_1_buffer_RX[ myMessIndex -4 ] == 'K' ) Messaggio_OK = 1;
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
  if(datiHost.comando == 2 && datiHost.dato == 001) {
    if(STATO_CORRENTE == 0) {
      spedisciStringa("01001"); // Zero Sollevatore
      Zero_Traslo();
    }
    else spedisciStringa("01009"); // Comando non eseguibile
  }

  if(datiHost.comando == 2 && datiHost.dato == 003) {
    if(STATO_CORRENTE != 0) {
      spedisciStringa("02003"); // Interrompi qualsiasi esecuzione
      if( STATO_CORRENTE < 9 ) ARRESTA = 1;
      STATO_CORRENTE = 0;
    }
    else spedisciStringa("01009"); // Comando non eseguibile
    Serial.print("Stato Corrente = ");
    Serial.println(STATO_CORRENTE);
  }
  
  if(datiHost.comando == 3 ) {
    if( datiHost.dato < 40 || datiHost.dato == 999 ) {
        if( datiHost.dato < 40 && Traslo_OK && !STATO_CORRENTE ) { // Colonne pannelli
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

}
//
// seriale wireless LORA  ----> FINE
//

