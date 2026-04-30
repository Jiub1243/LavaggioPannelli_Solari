/*changelog
ho aggiornato l'assegnazione di Messaggio_OK in ReadDataFromAntenna
*/

/* TODO:
* aggiungere parametro settabile n colonne e passi tra le colonne, TEST passi Traslo
* chiamare 03 = Pulizia          001 = Esegui pulizia della  colonna corrente per tutte le colonne (e prima fare ZT) dal quadro
* gestire le funzioni per la comunicazione come una FreeRTOS task e impostare dei mutex per proteggere le variabili?
*/

/* UART 1 = seriale wireless LORA
*
*
*  Messaggio:-Risposte Traslo:
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
*  Messaggio:-Risposte Carro:
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
*         11 = Altezza_SX       xxx = Setta l'altezza SX della spazzola (1/2 millimetro)
*                               0-255 -> setta l'altezza di spazzolamento SX
*                               999   -> Ritorna il settaggio corrente
*                     11   xxx = Altezza settata
*                     01   009 = Comando non eseguibile
*         12 = Altezza_DX       xxx = Setta l'altezza DX della spazzola (1/2 millimetro)
*                               0-255 -> setta l'altezza di spazzolamento DX
*                               999   -> Ritorna il settaggio corrente
*                     11   xxx = Altezza settata
*                     01   009 = Comando non eseguibile
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

  //Serial.println( "Siamo dentro ReadDataFromAntenna");

  while( Serial1.available() > 0 )
  {
    rc = Serial1.read();

    if( rc == MESS_START_MESS ) // inizio messaggio
    {
      myMessIndex = 0;
      RS232_1_buffer_RX[ myMessIndex ] = rc;
      myMessIndex++;

      RecvInProgress = true;
      //Serial.println("RecvInProgress true");
    }
    else if( RecvInProgress == true ) // se avevamo già iniziato il messaggio
    {
      RS232_1_buffer_RX[ myMessIndex ] = rc;
      myMessIndex++;

      if( rc != MESS_END_MESS ) // se non abbiamo finito di ricevere
      {
        if( myMessIndex >= LUN_BUF_232 ) // non aumentare la lunghezza del messaggio se abbiamo superato il limite, riscrivi l'ultimo carattere
        {
          myMessIndex = LUN_BUF_232 - 1;
        }
      }
      else if( myMessIndex == MESS_LUN_OK ) // se abbiamo ricevuto un messaggio di 5 byte con 'OK' interrompi la ricezione
      {
        if( RS232_1_buffer_RX[ myMessIndex -4 ] == 'O' && RS232_1_buffer_RX[ myMessIndex -3 ] == 'K' ) Messaggio_OK = 1;
        RecvInProgress = false;
        myMessIndex = 0;
        Serial.println( " == MESS_LUN_OK " );
        Serial.println( "Messaggio ricevuto: " );
        Serial.println( RS232_1_buffer_RX ); 
      }
      else if( myMessIndex <= MESS_LUN_ERR ) // se abbiamo ricevuto meno di 10 byte segnala un errore
      {
        if( RS232_1_buffer_RX[ 1 ] == 'E' && RS232_1_buffer_RX[ 3 ] == 'R' ) {}// abbiamo probabilmente ricevuto '+ERR=5'
        else // abbiamo ricevuto qualcos'altro
        {
          Serial.print("messaggio : ");
          Serial.print( RS232_1_buffer_RX );
          Serial.print( " <= MESS_LUN_ERR - MyMessIndex = " );
          Serial.println(myMessIndex);
        }
        
        Messaggio_OK = 0;
        RecvInProgress = false;
        myMessIndex = 0;
      }
      else
      {
        RS232_1_buffer_RX[ myMessIndex ] = '\0';

        if( myMessIndex >= MESS_LUN_REC ) // se abbiamo ricevuto più di 15 byte
        {
          strncpy( verifica, RS232_1_buffer_RX, 9 );
          verifica[9]=0x00;
          if( memcmp( verifica, "+RCV=1,5," ,9 ) == 0 || memcmp( verifica, "+RCV=2,5," ,9 ) == 0) { // se abbiamo ricevuto 5 byte dal nodo 1 o 2, ricezione avvenuta con successo
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
  
  /*  
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
  */
  
  //
  // Prima di uscire salvo mittente(telecomando) e comando in una struttura di appoggio
  //
  datiClient.mittente = myMittente;
  strcpy( datiClient.messaggio, myComando );
  datiClient.comando = atoi(com);
  datiClient.dato = atoi(dat);
  return( 1 );
}

// Invia un messaggio e attende una risposta. Se non arriva, ritrasmette
void inviaConRisposta(String messaggio, char dest) {
  Messaggio_Buffered = messaggio;
  Destinatario_Atteso = dest;
  Tentativi_Invio = 1;
  Attesa_Risposta = true;
  Tempo_Invio = millis();

  if (dest == CARRO_ADD) { 
    strncpy(retryErroreCarro.comando, messaggio.c_str(), 5);
    retryErroreCarro.comando[5] = '\0';
  }else if (dest == TRASLO_ADD) {
    strncpy(retryErroreTraslo.comando, messaggio.c_str(), 5); 
    retryErroreTraslo.comando[5] = '\0';
  }

  spedisciStringa(messaggio, dest);
}

// Da chiamare nel loop(). Controlla se è scaduto il timeout e ritrasmette
void controllaRisposta() {
  if (!Attesa_Risposta) return;

  if (millis() - Tempo_Invio >= TIMEOUT_RISPOSTA) {
    if (Tentativi_Invio < MAX_TENTATIVI) {
      Tentativi_Invio++;
      Tempo_Invio = millis();
      spedisciStringa(Messaggio_Buffered, Destinatario_Atteso);
      Serial.print("Reinvio (tentativo ");
      Serial.print(Tentativi_Invio);
      Serial.print("): ");
      Serial.println(Messaggio_Buffered);
    } else {
      Attesa_Risposta = false;
      Tentativi_Invio = 0;
      commsFailFlag = true;
      Serial.print("ERRORE: Nessuna risposta per ");
      Serial.println(Messaggio_Buffered);
    }
  }
}

// Spedisce la stringa al destinatario aggiungendo il prefisso necessario
void spedisciStringa( String stringaDaSpedire, char dest )
{
  char header[16];
  snprintf( header, sizeof(header), "AT+SEND=%c,5,", dest );
  Serial1.print( header );
  Serial1.println( stringaDaSpedire );
  if ( stringaDaSpedire != "01001" ) // Non stampo le richieste di stato per non floodare il serial monitor
  {
    Serial.println( "Messaggio spedito: ");
    Serial.print( header );
    Serial.println( stringaDaSpedire );
  }
}

// Interroga lo stato di carro e traslo ogni 5 secondi, sfalsati tra di loro di 2.5s
// Faccio il polling solo se non sto già aspettando la risposposta ad un comando
void richiediStato() {
  uint32_t ora = millis();

  if (ora - Ultimo_Poll_Traslo >= INTERVALLO_POLL) {
    if (!(Attesa_Risposta && Destinatario_Atteso == TRASLO_ADD)) {
      Ultimo_Poll_Traslo = ora;
      spedisciStringa("01001", TRASLO_ADD);
    }
  }

  if (ora - Ultimo_Poll_Carro >= INTERVALLO_POLL) {
    if (ora - Ultimo_Poll_Traslo >= INTERVALLO_POLL / 2) {
      if (!(Attesa_Risposta && Destinatario_Atteso == CARRO_ADD)) {
        Ultimo_Poll_Carro = ora;
        spedisciStringa("01001", CARRO_ADD);
      }
    }
  }
}

// Controlla che gli slaves abbiano risposto entro un certo intervallo, altrimenti attiva una flag.
void controllaConnessione() {
  uint32_t ora = millis();

  if (Ultimo_Msg_Traslo != 0 && ora - Ultimo_Msg_Traslo >= TIMEOUT_OFFLINE) {
    if (!offlineTrasloFlag) { // Se la flag è già attiva non la attivo di nuovo ad ogni iterazione
      offlineTrasloFlag = true;
      Serial.println("ALLARME: Traslo non risponde");
    }
  } else {
    offlineTrasloFlag = false;
  }

  if (Ultimo_Msg_Carro != 0 && ora - Ultimo_Msg_Carro >= TIMEOUT_OFFLINE) {
    if (!offlineCarroFlag) { // Se la flag è già attiva non la attivo di nuovo ad ogni iterazione
      offlineCarroFlag = true;
      Serial.println("ALLARME: Carro non risponde");
    }
  } else {
    offlineCarroFlag = false;
  }
}

// Funzioni helper per gestire gli allarmi
// ritenta un comando quando viene riceuto un errore
void tentaRetry(RetryInfo &r) {
  r.contatore++;
  if (r.contatore < r.maxTentativi) {
    inviaConRisposta(r.comando, r.destinatario);
    Serial.print("Retry tentativo ");
    Serial.println(r.contatore);
  } else {
    r.fallito = true;
  }
}
// Resetta un oggetto retryinfo quando viene superato l'errore o viene superato il counter
void resetRetry(RetryInfo &r) {
  r.contatore = 0;
  r.fallito = false;
}

void esaminaMessaggio( void )
{
  String DaSpedire;
  Serial.print( "Ricevuto messaggio: " );
  Serial.print( datiClient.messaggio );
  Serial.print( "   Comando = " );
  Serial.print( datiClient.comando );
  Serial.print( "   dato = " );
  Serial.print( datiClient.dato );
  Serial.print( "   da " );
  Serial.println( datiClient.mittente );

  //// Retry management
  // Se stavamo aspettando una risposta da questo mittente, annulla il retry
  if (Attesa_Risposta && datiClient.mittente == Destinatario_Atteso) {
    Attesa_Risposta = false;
    Tentativi_Invio = 0;
  }

  // Offline detection
  if (datiClient.mittente == CARRO_ADD) Ultimo_Msg_Carro = millis();
  if (datiClient.mittente == TRASLO_ADD) Ultimo_Msg_Traslo = millis();
 
  if(datiClient.mittente == CARRO_ADD) { // ricevuto messaggio dal carro
    /*Comandi possibili:
    01xxx - Cambio Stato
      01010, 01011 - Errore Driver Sollevamento DX / SX
      01012 - Errore Driver Spazzola
      01013 - Errore Driver Avanzamento
      01014 - Errore Extra Corsa M4 (Mai Assegnato)
      01015 - Errore Lettura Altezza Sollevatore
    03xxx - Spazzolatura completata al xxx % 
    04001, 05001, 06001 - Comando TEST Sollevatore o TEST Avanzamento o TEST velocità spazzola OK
    07xxx - Altezza Sollevatore SX xxx %
    08xxx - Altezza Sollevatore DX xxx %
    09xxx - Avanzamento completato al xxx %
    10xxx - Velocità rotazione spazzola xxx %
    11xxx - Altezza settata (DX o SX)
    01009 - Comando non eseguibile
    */
    if(datiClient.comando == 1) { // Aggiornamento Stato
      
      if ( datiClient.dato == 9 ) {// Comando non eseguibile
        //TODO gestire caso comando non eseguibile
        cneFlag = 1;
      }else{
        STATO_CORRENTE_CARRO = datiClient.dato;
        Serial.print(" Nuovo stato del Carro:   ");
        Serial.println(STATO_CORRENTE_CARRO);
        pubblicaStatoCarroSeCambiato();
      }

      // Gestione errori
      if (datiClient.dato >= 10 && datiClient.dato <= 13) {
        tentaRetry(retryErroreCarro);
      } else {
        resetRetry(retryErroreCarro);
      }

    }else if(datiClient.comando == 3) { // Spazzolatura colonna xxx
      STATO_CORRENTE_CARRO = 4;
      Colonna = (uint8_t)datiClient.dato;
    } 

  }else if(datiClient.mittente == TRASLO_ADD) { // ricevuto messaggio dal traslo
    /*Comandi possibili:
    01xxx - Cambio Stato
      01012, 01013 - Errore M1 / M2
      01010, 01011 - Errore Driver Sup / Inf
      01014 - errore extracorsa, mai assegnato
    03xxx - Stato spostamento verso colonna xxx
    01009 - Comando non eseguibile
    */
    if(datiClient.comando == 1) { // Aggiornamento Stato
      if ( datiClient.dato == 9 ) {// Comando non eseguibile
        cneFlag = 1;
      }else{
        STATO_CORRENTE_TRASLO = datiClient.dato;
        Serial.print(" Nuovo stato del traslo:   ");
        Serial.println(STATO_CORRENTE_TRASLO);
        pubblicaStatoTrasloSeCambiato();

        if (datiClient.dato == 12 ||datiClient.dato == 13 ) {// Allarme Zero M1 o M2
          tentaRetry(retryErroreTraslo);
        }else  {
          resetRetry(retryErroreTraslo);
        }

        if (datiClient.dato == 10 ||datiClient.dato == 11 ) { // Allarme Driver Sup o Inf
          //TODO gestire errori Drivers
          // un errore dei drivers in Movimentazione.ino è controllato solo dentro Interrompi. devo mandare una nuova richiesta di arresto se lo rilevo?
        }
      }
    }else if(datiClient.comando == 3) { // Spostamento verso colonna xxx
      STATO_CORRENTE_TRASLO = 3;
      Colonna = (uint8_t)datiClient.dato;
      Serial.print(" Nuovo stato del traslo:   ");
      Serial.println(STATO_CORRENTE_TRASLO);
      pubblicaStatoTrasloSeCambiato();
    }else if (datiClient.comando == 4) { // Conferma passi_Colonna_M1
      long confermato = (long)datiClient.dato * 10;
      if (confermato != passi_Colonna_M1_da_confermare) {
        Serial.print("ATTENZIONE: passi_Colonna_M1 non confermato. Atteso: ");
        Serial.print(passi_Colonna_M1);
        Serial.print(" Ricevuto: ");
        Serial.println(confermato);
        // TODO: segnalare errore all'HMI o ritentare
      } else {
        passi_Colonna_M1 = confermato;
        Serial.println("passi_Colonna_M1 confermato dal Traslo");
      }
    }else if (datiClient.comando == 5) { // Conferma passi_Colonna_M2
      long confermato = (long)datiClient.dato * 10;
      if (confermato != passi_Colonna_M2_da_confermare) {
        Serial.print("ATTENZIONE: passi_Colonna_M2 non confermato. Atteso: ");
        Serial.print(passi_Colonna_M2);
        Serial.print(" Ricevuto: ");
        Serial.println(confermato);
      } else {
        passi_Colonna_M2 = confermato;
        Serial.println("passi_Colonna_M2 confermato dal Traslo");
      }
    }else if (datiClient.comando == 6) { // Conferma maxCol
      uint8_t confermato = (uint8_t)datiClient.dato;
      if (confermato != maxCol_da_confermare) {
        Serial.print("ATTENZIONE: maxCol non confermato. Atteso: ");
        Serial.print(maxCol_da_confermare);
        Serial.print(" Ricevuto: ");
        Serial.println(confermato);
      } else {
        maxCol = confermato;
        Serial.println("maxCol confermato dal Traslo");
      }
    }else if (datiClient.comando == 13) {
      passi_Colonna_M1 = (long)datiClient.dato * 10;
      Serial.print("Sync passi_Colonna_M1 dal Traslo: ");
      Serial.println(passi_Colonna_M1);
    } else if (datiClient.comando == 14) {
      passi_Colonna_M2 = (long)datiClient.dato * 10;
      Serial.print("Sync passi_Colonna_M2 dal Traslo: ");
      Serial.println(passi_Colonna_M2);
    } else if (datiClient.comando == 15) {
      maxCol = (uint8_t)datiClient.dato;
      Serial.print("Sync maxCol dal Traslo: ");
      Serial.println(maxCol);
    }
  }
}
//
// seriale wireless LORA  ----> FINE
//

