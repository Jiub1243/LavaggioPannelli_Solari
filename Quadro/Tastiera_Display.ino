// TODO come gestire il refresh automatico quando ricevo un update? Da Uart_1 non posso invocare Display_Test. Forse quando la risposta sarà automatica il delay(1000) sarà sufficiente per aggiornare i parametri prima del secondo Display_Test?
///*
char GetKey(){
  char ritorno = ' ',  keys[] = "123A456B789C*0#DNF";  //  N = NoKey, F = Fail
  uint8_t  index = keyPad.getKey();

    if( index == 16) Old_Tasto = index;
    if((index != 16) && (Old_Tasto != index)) {
      Old_Tasto = index;
      ritorno = keys[index];
    }
  return( ritorno );
}

void clearData(){
  while(data_count !=0){
    Data[data_count--] = 0; 
  }
  return;
}

// converte il condice int in una etichetta per il traslo
String inttolb_traslo(uint16_t stato) {
  switch (stato) {
    case  0:  return "Idle       ";
    case  1:  return "Zero Traslo";
    case  2:  return "Ritorno    ";
    case  3:  return "Spostamento";
    case  5:  return "Test       ";
    case 10:  return "ERR DRV SUP";
    case 11:  return "ERR DRV INF";
    case 12:  return "ERR ZERO M1";
    case 13:  return "ERR ZERO M2";
    case 14:  return "ERR EXCORSA";
    case 0xFF: return "   -----   ";
    default:  return "err lettura";
  }
}

// converte il codice int in una etichetta per il carro
String inttolb_carro(uint16_t stato) {
  switch (stato) {
    case  0:  return "Idle       ";
    case  1:  return "Fine Spazz.";
    case  2:  return "Zero Avanz."; //TODO in teoria è come status 8 ma non viene mai assegnato in pratica
    case  3:  return "Ritorno    ";
    case  4:  return "Spazzolatur";
    case  5:  return "Test       ";
    case  7:  return "Zero Sollev";
    case  8:  return "Zero Avanz.";
    case  9:  return "Cmd N/Eseg.";
    case 10:  return "ERR DRV SX ";
    case 11:  return "ERR DRV DX ";
    case 12:  return "ERR DRV SPZ";
    case 13:  return "ERR DRV AVZ";
    case 14:  return "ERR EXCORSA";
    case 15:  return "ERR ALT SOL";
    case 0xFF: return "   -----   ";
    default:  return "err lettura";
  }
}



// Controlla se almeno uno slave è in uno stato attivo (non Idle, non errore)
// Attesa_Risposta Dovrebbe essere 1 se e solo se uno degli slaves è attivo quindi in teoria è ridondante, ma non si sa mai. Potrebbe essere che abbiamo mandato un comando ma non abbiamo ancora ricevuto una risposta e gli stati devono ancora aggiornarsi
bool slavesInMovimento() {
  bool trasloAttivo = (STATO_CORRENTE_TRASLO > 0 && STATO_CORRENTE_TRASLO < 6);
  bool carroAttivo  = (STATO_CORRENTE_CARRO  > 0 && STATO_CORRENTE_CARRO  < 9);
  return trasloAttivo || carroAttivo || Attesa_Risposta;
}

// Controlla se il tasto premuto è da usare per fermare gli slaves
// Da chiamare nel loop interno di Test_01(), prima di processare customKey per i menu
// Ritorna 1 se il tasto è da usare per inviare un messaggio di STOP, 0 altrimenti
uint8_t controllaTastoStop(char customKey) {
  if (customKey == ' ' ||customKey == 'A' ||customKey == 'B') return 0; // nessun tasto premuto
  /*Serial.print("STATO_CORRENTE_TRASLO: ");
  Serial.println(STATO_CORRENTE_TRASLO);
  Serial.print("STATO_CORRENTE_CARRO: ");
  Serial.println(STATO_CORRENTE_CARRO);
  Serial.print("Attesa_Risposta: ");
  Serial.println(Attesa_Risposta);
  Serial.print("in movimento -> ");
  Serial.println(slavesInMovimento());*/
  if (!slavesInMovimento()) return 0; // nessuno slave in movimento

  return 1;
}

//*/
void Homepage(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("********************");
  lcd.setCursor(0,1);
  lcd.print("* Pulizia Pannelli *");
  lcd.setCursor(0,2);
  lcd.print("*   SINTEL V.d.H   *");
  lcd.setCursor(0,3);
  lcd.print("********************");
}
///*
// Ritorna 0 se la pw è corretta, 1 altrimenti
int password(){
  uint8_t customKey, passOK = 1;
  customKey = GetKey();
  if (customKey > ' '){
    if(data_count == 0) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("  PASSWORD...");
    }
    Data[data_count] = customKey; 
    lcd.setCursor(data_count,1); 
    lcd.print(Data[data_count]); 
    data_count++; 
    delay(350);
    //Serial.println(customKey);
    //Serial1.print("AT+SEND=2,1,");
    //Serial1.println(customKey);
    //Serial1.print("AT+SEND=2,5,HELLO");
    }

  if(data_count == Password_Length-1){
    lcd.clear();
    if(!strcmp(Data, Master)){
      lcd.print(" Pass Corretta");
      passOK = 0;
      digitalWrite(signalPin, LOW); 
      delay(1500);
      digitalWrite(signalPin, HIGH);
      }
    else{
      lcd.print("Pass Errata");
      delay(3000);
      Homepage();
      }
    
    clearData(); 
  }
  return(passOK);
}

// Helper per quando viene richiesto di muoversi ad una certa colonna
// Apre una pagina dove inserire il numero di colonna da raggiungere
uint8_t chiediColonna() {
  char buf[4] = {0, 0, 0, 0};
  uint8_t idx = 0;
  uint8_t customKey;

  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Colonna (0-39):     "); // sostituire con maxCol
  lcd.setCursor(0, 1); lcd.print("# Conferma  * Annul.");

  while (true) {
    if (ReadDataFromAntenna()) {  
      if (parseAntenna()) esaminaMessaggio();
    }
    richiediStato(); // interroga periodicamente gli slaves
    controllaRisposta(); // Controlla se è scaduto il timeout e ritrasmette
    controllaConnessione(); // // Controlla che gli slaves abbiano risposto entro un certo intervallo

    customKey = GetKey();

    if (customKey >= '0' && customKey <= '9' && idx < 3) {
      buf[idx] = customKey;
      lcd.setCursor(16 + idx, 0);
      lcd.print(buf[idx++]);
    }
    else if (customKey == '#' && idx > 0) { // Premuto invio
      uint8_t col = (uint8_t)atoi(buf);
      if (col <= 39) return col;
      // Out of range
      lcd.setCursor(0, 3); lcd.print("Max colonna = 39!   "); // sostituire con maxCol
      delay(1500);
      memset(buf, 0, sizeof(buf));
      idx = 0;
      lcd.setCursor(0, 3); lcd.print("                    ");
      lcd.setCursor(5, 2);  lcd.print("   ");
    }
    else if (customKey == '*') { // premuto cancella
      return 0xFF;  
    }
    delay(150);
  }
}

// Funzione più generale di chiediColonna che richiede una stringa con il come della variabile da assegnare e gli estremi dell'insieme dei vaolir possibili
uint16_t chiediValore(const char* label, uint16_t minVal, uint16_t maxVal) {
  uint16_t val = 0;
  uint8_t digits = 0;
  uint8_t key;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(label);
  lcd.setCursor(0, 1);
  lcd.print("Valore: ");
  lcd.setCursor(8, 1);
  lcd.print("___");

  do {
    // Ascoltiamo mentre aspettiamo che venga inserito un numero
    if (ReadDataFromAntenna()) {  
      if (parseAntenna()) esaminaMessaggio();
    }
    richiediStato(); // interroga periodicamente gli slaves
    controllaRisposta(); // Controlla se è scaduto il timeout e ritrasmette
    controllaConnessione(); // // Controlla che gli slaves abbiano risposto entro un certo intervallo

    key = GetKey();
    if (key >= '0' && key <= '9') {
      if (digits < 3) {
        val = val * 10 + (key - '0'); //key è un char quindi fare la differenza è come castarlo a int 
        digits++;
        lcd.setCursor(8, 1);
        lcd.print(val);
      }
    } else if (key == '*') { // Backspace
      val = val / 10;
      if (digits > 0) digits--;
      lcd.setCursor(8, 1);
      lcd.print("   ");
      lcd.setCursor(8, 1);
      lcd.print(val);
    } else if (key == 'D') { // Annulla
      return 0xFFFF; // valore sentinella = annullato
    }
  } while (key != '#'); // conferma con #

  if (val < minVal || val > maxVal) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Valore non valido!");
    delay(1000);
    return 0xFFFF;
  }

  return val;
}

/*
void LCD_Posizione_M1_M2 (void) {
  long Pos_M1, Pos_M2;
  lcd.setCursor(0,3);
  lcd.print("M1=");
  if( ! Traslo_OK ) {
    lcd.print("-----");
  }
  else lcd.print(Pos_M1);
  lcd.setCursor(11,3);
  lcd.print("M2=");
  if(! Traslo_OK ) {
    lcd.print("-----");
  }
  else lcd.print(Pos_M2);
}                             
*/

// Comunica sul display se c'è stato un cambio di stato
void aggiornaDisplayPT(StatoPulisciTutto stato, uint8_t colonna) {
  char buf[21];  // 20 caratteri + terminatore

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("== PULISCI TUTTO ==");  // 19 char, ok

  lcd.setCursor(0, 1);
  switch (stato) {
    case PT_ZERO_TRASLO:
      lcd.print("Fase: Zero Traslo   ");  // 20
      break;
    case PT_VAI_COLONNA:
      snprintf(buf, sizeof(buf), "Fase: Vai Col %-2u    ", colonna);
      lcd.print(buf);
      break;
    case PT_PULIZIA_COLONNA:
      snprintf(buf, sizeof(buf), "Fase: Pulizia Col %-2u", colonna);
      lcd.print(buf);
      break;
    default:
      lcd.print("                    ");  // 20
      break;
  }

  lcd.setCursor(0, 2);
  if (stato == PT_VAI_COLONNA || stato == PT_PULIZIA_COLONNA) {
    snprintf(buf, sizeof(buf), "Progresso: %2u / %-2u ", colonna, maxCol);
    lcd.print(buf);
  } else {
    lcd.print("                    ");  // 20
  }

  lcd.setCursor(0, 3);
  lcd.print("F4 = STOP           ");  // 20
}

// Comunica sul display l'esito finale della pulizia
void mostraEsitoPT(StatoPulisciTutto stato) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("== PULISCI TUTTO ==");  // 19

  switch (stato) {
    case PT_COMPLETATO:
      lcd.setCursor(0, 1);
      lcd.print("   COMPLETATO !     ");  // 20
      lcd.setCursor(0, 2);
      lcd.print("Tutte le colonne OK ");  // 20
      break;

    case PT_ABORTITO_STOP:
      lcd.setCursor(0, 1);
      lcd.print("   INTERROTTO       ");  // 20
      lcd.setCursor(0, 2);
      lcd.print("Stop utente         ");  // 20
      break;

    case PT_ABORTITO_ERRORE:
      lcd.setCursor(0, 1);
      lcd.print("   ERRORE !         ");  // 20
      if (STATO_CORRENTE_TRASLO >= 10 && STATO_CORRENTE_TRASLO <= 14) {
        lcd.setCursor(0, 2);
        lcd.print("Traslo: ");
        lcd.setCursor(8, 2);
        lcd.print(inttolb_traslo(STATO_CORRENTE_TRASLO));
      } else if (STATO_CORRENTE_CARRO >= 10 && STATO_CORRENTE_CARRO <= 15) {
        lcd.setCursor(0, 2);
        lcd.print("Carro:  ");
        lcd.setCursor(8, 2);
        lcd.print(inttolb_carro(STATO_CORRENTE_CARRO));
      }
      break;

    case PT_ABORTITO_OFFLINE:
      lcd.setCursor(0, 1);
      lcd.print("  COMUNICAZIONE KO  ");  // 20
      lcd.setCursor(0, 2);
      if (offlineTrasloFlag)        lcd.print("Traslo non risponde ");  // 20
      else if (offlineCarroFlag)    lcd.print("Carro non risponde  ");  // 20
      else                          lcd.print("Timeout risposta    ");  // 20
      break;

    default:
      break;
  }

  lcd.setCursor(0, 3);
  lcd.print("Premere un tasto... ");  // 20
}


void pulisciTutto() {
  StatoPulisciTutto stato = PT_ZERO_TRASLO;
  uint8_t colonna_corrente = 0;
  uint8_t old_stato_traslo, old_stato_carro;
  char customKey;
  bool primo_ingresso_stato = true;  // per mandare il comando una sola volta

  // Schermata iniziale
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("== PULISCI TUTTO ==");
  lcd.setCursor(0, 3); lcd.print("F4 = STOP");

  do {
    old_stato_traslo = STATO_CORRENTE_TRASLO;
    old_stato_carro  = STATO_CORRENTE_CARRO;

    // Infrastruttura comms (come in Test_01)
    if (ReadDataFromAntenna()) {
      if (parseAntenna()) esaminaMessaggio();
    }
    controllaRisposta();
    controllaConnessione();

    // --- AZIONE IN INGRESSO DI STATO ---
    // Mando il comando UNA sola volta quando entro in uno stato
    if (primo_ingresso_stato) {
      primo_ingresso_stato = false;
      switch (stato) {
        case PT_ZERO_TRASLO:
          aggiornaDisplayPT(stato, colonna_corrente);
          inviaConRisposta("02001", TRASLO_ADD);
          break;
        case PT_VAI_COLONNA: {
          aggiornaDisplayPT(stato, colonna_corrente);
          char buf[6];
          snprintf(buf, sizeof(buf), "03%03u", colonna_corrente);
          inviaConRisposta(buf, TRASLO_ADD);
          break;
        }
        case PT_PULIZIA_COLONNA:
          aggiornaDisplayPT(stato, colonna_corrente);
          inviaConRisposta("03001", CARRO_ADD);
          break;
        default: break;
      }
    }

    // --- TRANSIZIONI SU EVENTI ---
    // Controllo errori slaves PRIMA delle transizioni "felici"
    if (STATO_CORRENTE_TRASLO >= 10 && STATO_CORRENTE_TRASLO <= 14) {
      stato = PT_ABORTITO_ERRORE;
      primo_ingresso_stato = true;
      continue;
    }
    if (STATO_CORRENTE_CARRO >= 10 && STATO_CORRENTE_CARRO <= 15) {
      stato = PT_ABORTITO_ERRORE;
      primo_ingresso_stato = true;
      continue;
    }
    if (offlineTrasloFlag || offlineCarroFlag || commsFailFlag) {
      stato = PT_ABORTITO_OFFLINE;
      primo_ingresso_stato = true;
      continue;
    }

    // Transizioni normali: rilevamento per edge sugli stati slave
    switch (stato) {
      case PT_ZERO_TRASLO:
        // ZT partito (1) e tornato a Idle (0) = finito
        if (old_stato_traslo == 1 && STATO_CORRENTE_TRASLO == 0) {
          stato = PT_VAI_COLONNA;
          primo_ingresso_stato = true;
        }
        break;

      case PT_VAI_COLONNA:
        // Spostamento (3) → Idle (0) = arrivato
        if (old_stato_traslo == 3 && STATO_CORRENTE_TRASLO == 0) {
          stato = PT_PULIZIA_COLONNA;
          primo_ingresso_stato = true;
        }
        break;

      case PT_PULIZIA_COLONNA:
        // Spazzolatura (4) → Fine Spazz. (1) = pulizia colonna finita
        if (old_stato_carro == 4 && STATO_CORRENTE_CARRO == 0) {
          if (colonna_corrente >= maxCol) {
            stato = PT_COMPLETATO;
          } else {
            colonna_corrente++;
            stato = PT_VAI_COLONNA;
          }
          primo_ingresso_stato = true;
        }
        break;

      default: break;
    }

    // --- STOP utente ---
    customKey = GetKey();
    if (customKey == 'D') {  // scegli tu il tasto STOP dedicato
      stato = PT_ABORTITO_STOP;
      primo_ingresso_stato = true;
    }

    // Refresh display se è cambiato qualcosa di rilevante
    if (STATO_CORRENTE_TRASLO != old_stato_traslo ||
        STATO_CORRENTE_CARRO  != old_stato_carro) {
      aggiornaDisplayPT(stato, colonna_corrente);
    }

    delay(100);
  } while (stato != PT_COMPLETATO &&
           stato != PT_ABORTITO_STOP &&
           stato != PT_ABORTITO_ERRORE &&
           stato != PT_ABORTITO_OFFLINE);

  // --- USCITA ---
  // In caso di STOP o errore, manda arresto a entrambi
  if (stato != PT_COMPLETATO) {
    spedisciStringa("02003", TRASLO_ADD);
    spedisciStringa("02003", CARRO_ADD);
    Attesa_Risposta = false;
    Tentativi_Invio = 0;
    Messaggio_Buffered = "";
  }

  // Mostra messaggio finale per qualche secondo
  mostraEsitoPT(stato);
  delay(3000);
}



void Display_Test(uint8_t pagina){
  uint8_t customKey;
  int Ultrasuoni_1, Ultrasuoni_2;
  long Pos_M1, Pos_M2, Pos_M4;
  switch (pagina) {

    case 1: { 
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("    Menu   TEST     ");
      lcd.setCursor(0,1);
      lcd.print("F1 Pagina Avanti    ");
      lcd.setCursor(0,2);
      lcd.print("F2 Pagina Indietro  ");
      lcd.setCursor(0,3);
      lcd.print("F4 USCITA Menu      ");
      }
      break;

    case 2: { // Impostazioni
      lcd.clear();
      lcd.setCursor(0,0); lcd.print("1 N.Colonne (ora:  )");
      lcd.setCursor(17,0); lcd.print(maxCol);
      lcd.setCursor(0,1); lcd.print("2 PassiM1 (ora:    )");
      lcd.setCursor(15,1); lcd.print(passi_Colonna_M1);
      lcd.setCursor(0,2); lcd.print("3 PassiM2 (ora:    )");
      lcd.setCursor(15,2); lcd.print(passi_Colonna_M2);
      lcd.setCursor(0,3); lcd.print("4 Pulisci tutto     ");
      }
      break;

    case 3 : { // Traslo
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("1 Stato:            ");
          lcd.setCursor(9,0);
          lcd.print(inttolb_traslo(STATO_CORRENTE_TRASLO));

          lcd.setCursor(0,1);
          lcd.print("2 Vai Col (ora:  )  ");
          lcd.setCursor(15,1);
          lcd.print(Colonna);

          lcd.setCursor(0,2);
          lcd.print("3 Azzera            "); // Zero traslo

          lcd.setCursor(0,3);
          lcd.print("4 STOP              "); // Ferma tutto
      }
      break;

    case 4 : { // Carro, pag 1
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("1 Stato:            ");
        lcd.setCursor(9,0);
        lcd.print(inttolb_carro(STATO_CORRENTE_CARRO));

        lcd.setCursor(0,1);
        lcd.print("2 Zero Avanzamento  "); // Azzera avanzamento

        lcd.setCursor(0,2);
        lcd.print("3 Zero Sollevamento "); // Azzera sollevamento

        lcd.setCursor(0,3);
        lcd.print("4 STOP              "); // Ferma tutto
      }
      break;

    case 5 : { // Carro, pag 2
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("1 Pulizia Col       "); // Pulizia colonna Corrente
      lcd.setCursor(14,0);
      lcd.print(Colonna);

      lcd.setCursor(0,1);
      lcd.print("2 Test Sollevatore  "); // TEST Sollevatore xxx

      lcd.setCursor(0,2);
      lcd.print("3 Test Avanzamento  "); // TEST Avanzamento xxx

      lcd.setCursor(0,3);
      lcd.print("4 Test Spazzola     "); // TEST Spazzola    xxx
      }
      break;

    case 6 : { // Carro, pag 3
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("1 RQST Sollevatore S"); // RQST Sollevatore SX

      lcd.setCursor(0,1);
      lcd.print("2 RQST Sollevatore D"); // RQST Sollevatore DX

      lcd.setCursor(0,2);
      lcd.print("3 RQST Avanzamento  "); // RQST Avanzamento

      lcd.setCursor(0,3);
      lcd.print("4 RQST Spazzola     "); // RQST Spazzola
      }
      break;

    case 7 : { // Carro, pag 4
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("1 Set Spazzola SX   "); // Setta l'altezza SX della spazzola

      lcd.setCursor(0,1);
      lcd.print("2 Set Spazzola DX   "); // Setta l'altezza DX della spazzola
      }
      break;

    case 8 : { // Traslo, pag 2 
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("1 RQST Passi M1     "); // RQST passi_Colonna_M1

      lcd.setCursor(0,1);
      lcd.print("2 RQST Passi M2     "); // RQST passi_Colonna_M2

      lcd.setCursor(0,2);
      lcd.print("3 RQST maxCol       "); // RQST maxCol
      }
      break;
    }
}

void Test_01(){
  uint8_t customKey, pagina_test=1;
  uint8_t sensore_M1, sensore_M2, sensore_M4;
  int32_t passi, passi_M1;
  uint8_t old_stato_traslo; // per salvare i vecchi stati e vedere se cambiano
  uint8_t old_stato_carro;
  //long Pos_M4;

  do {
    Display_Test( pagina_test );
    // lettura messaggi, controllo errori
    do {
      old_stato_traslo = STATO_CORRENTE_TRASLO; // salvo i vecchi stati per vedere se cambiano
      old_stato_carro  = STATO_CORRENTE_CARRO;

      //LoRa
      if (ReadDataFromAntenna()) {  
        if (parseAntenna()) esaminaMessaggio();
      }
      // Controllo connessione WiFi 
      static uint32_t ultimoTentativoWiFi = 0;
        if (WiFi.status() != WL_CONNECTED) {
          if (millis() - ultimoTentativoWiFi >= 10000) {  // riprova ogni 10s
            ultimoTentativoWiFi = millis();
            Serial.print("WiFi disconnesso, tentativo riconnessione... SSID precedente: ");
            Serial.print(WiFi.SSID());
            Serial.print(" RSSI: ");
            Serial.println(WiFi.RSSI());
            WiFiMulti.run(connectTimeoutWiFi);  
          }
      }

      // MQTT (riconnessione non bloccante + processing messaggi in arrivo)
      gestisciMQTT();

      //Controllo OTA: triggerato da MQTT, eseguito quando gli slave sono fermi
      if (shouldUpdateFirmware()) {
        updateFirmware();
      }

      richiediStato(); // interroga periodicamente gli slaves
      controllaRisposta(); // Controlla se è scaduto il timeout e ritrasmette
      controllaConnessione(); // // Controlla che gli slaves abbiano risposto entro un certo intervallo

      if( STATO_CORRENTE_TRASLO != old_stato_traslo || STATO_CORRENTE_CARRO  != old_stato_carro ) {
        Display_Test( pagina_test ); // se uno dei due stati è cambiato, refresha la pagina (anche se non siamo ad una pagina che mostra lo stato)
      }

      if (cneFlag){ // Se ad un comando abbiamo ricevuto la risposta 'comando non eseguibile'
        cneFlag = 0;
        lcd.clear();
        lcd.setCursor( 0, 1 );
        lcd.print( "CMD Non Eseguibile.." );
        delay(2000);
        Display_Test( pagina_test );
      }

      // Se uno slave supera il limite di timeout senza rispondere mostrare questi avvisi
      // Time out alle richieste specifiche (Ex. vai alla colonna xxx)
      if (commsFailFlag) { 
        commsFailFlag = false;
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("Nessuna Risposta!");
        delay(2000);
        Display_Test(pagina_test);
      }
      // timeout alla richiesta di stato  TODO SCOMMENTARE QUANDO HO FINITO DI TESTARE IL TRASLO
      if (offlineTrasloFlag) { /*
        offlineTrasloFlag = false;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ALLARME:");
        lcd.setCursor(0, 1);
        lcd.print("Traslo non risponde!");
        delay(2000);
        Display_Test(pagina_test);  */
      }
      // timeout alla richiesta di stato
      if (offlineCarroFlag) { /*
        offlineCarroFlag = false;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ALLARME:");
        lcd.setCursor(0, 1);
        lcd.print("Carro non risponde! ");
        delay(2000);
        Display_Test(pagina_test); */
      } 

      if (retryErroreTraslo.fallito) {
        uint8_t errore = STATO_CORRENTE_TRASLO; // salva prima di resettare
        resetRetry(retryErroreTraslo);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ERRORE Traslo:");
        lcd.setCursor(0, 1);
        lcd.print(inttolb_traslo(errore));
        lcd.setCursor(0, 2);
        lcd.print("Cmd: ");
        lcd.print(retryErroreTraslo.comando);
        delay(2000);
        Display_Test(pagina_test);
      }

      if (retryErroreCarro.fallito) {
        uint8_t errore = STATO_CORRENTE_CARRO; // salva prima di resettare
        resetRetry(retryErroreCarro);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ERRORE CARRO:");
        lcd.setCursor(0, 1);
        lcd.print(inttolb_carro(errore));
        lcd.setCursor(0, 2);
        lcd.print("Cmd: ");
        lcd.print(retryErroreCarro.comando);
        delay(2000);
        Display_Test(pagina_test);
      }

      customKey = GetKey();
      // Conrtolla se è il caso di usare il tasto premuto come stop o no
      if (controllaTastoStop(customKey)){
        // Manda STOP a entrambi con spedisciStringa perché inviaConRisposta gestisce solo un dispositivo alla volta
        spedisciStringa("02003", TRASLO_ADD);
        spedisciStringa("02003", CARRO_ADD);

        // Pulisce lo stato di retry in corso
        Attesa_Risposta = false;
        Tentativi_Invio = 0;
        Messaggio_Buffered = "";

        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("STOP inviato...     ");
        delay(1000);
        Display_Test(pagina_test);

        customKey = ' ';
      }
    } while (customKey == ' ');

    switch( pagina_test ) {

      case 2: // Impostazioni
        switch( customKey ) {

          case '1': // Numero colonne
            {
              uint16_t val = chiediValore( "Idx Ultima Colonna", 0, 99 );
              if( val != 0xFFFF ) {
                char buf[6];
                maxCol_da_confermare = (uint8_t)val;
                snprintf( buf, sizeof(buf), "06%03u", val );
                inviaConRisposta( buf, TRASLO_ADD );
              }
              Display_Test( pagina_test );
            } 
            break;

          case '2': // passi m1 !!! Assumo che se inserisco un valore >= 1000 mi sono dimenticato di inserire il valore diviso per 10 e lo faccio qui
            {
              uint16_t val = chiediValore( "Passi M1 / 10", 1, 10000 );
                if( val != 0xFFFF ) {
                  char buf[6];
                  if (val > 999) {val = val / 10;}
                  passi_Colonna_M1_da_confermare = val * 10;
                  snprintf( buf, sizeof(buf), "04%03u", val );
                  inviaConRisposta( buf, TRASLO_ADD );
                }
                Display_Test( pagina_test );
            }            
            break;

          case '3': // passi m2 !!! Assumo che se inserisco un valore >= 1000 mi sono dimenticato di inserire il valore diviso per 10 e lo faccio qui
            {
              uint16_t val = chiediValore( "Passi M2 / 10", 1, 10000 );
                if( val != 0xFFFF ) {
                  char buf[6];
                  if (val > 999) {val = val / 10;}
                  passi_Colonna_M2_da_confermare = val * 10;
                  snprintf( buf, sizeof(buf), "05%03u", val );
                  inviaConRisposta( buf, TRASLO_ADD );
                }
                Display_Test( pagina_test );
            }            
            break;

          case '4': // pulisci tutto
            {
              lcd.clear();
              lcd.setCursor( 0, 1 );
              lcd.print( "Pulizia Completa    " );
              delay(1000);
              pulisciTutto();
              Display_Test( pagina_test );
            }
            break;

          case 'A': 
            if( pagina_test < MAX_PAG ) pagina_test++; else pagina_test = 1; // cicla a pagina 1 se andiamo oltre la pagina massima  
            break;

          case 'B': 
            if( pagina_test > 1 ) 
              pagina_test--; 
            else
              pagina_test = MAX_PAG -1; // cicla all'ultima pagina se andiamo prima della prima pagina 
            break;

          case 'D': pagina_test = 0; break;
        }
        break;

      case 3: // Pagina Traslo
        switch( customKey ) {
          
          case '1': // Richiesta stato
            inviaConRisposta( "01001", TRASLO_ADD ); // per questa richiesta di stato uso inviaConRisposta invece di spedisciStringa perché voglio una risposta immediata
            delay(1000);
            Display_Test( pagina_test ); // ricaricao la pagina per mostrare il nuovo stato, se è cambiato
            break;
          
          case '2': // Vai alla colonna
            {
              //uint8_t col = chiediColonna();
              uint16_t col = chiediValore("Colonna:", 0, maxCol);
              if( col != 0xFFFF ) {
                char buf[6];
                snprintf( buf, sizeof(buf), "03%03u", col );
                inviaConRisposta( buf, TRASLO_ADD );
              }
              Display_Test( pagina_test );
            }
            break;

          case '3': // Azzera traslo
            inviaConRisposta( "02001", TRASLO_ADD );
            lcd.clear();
            lcd.setCursor( 0, 1 );
            lcd.print( "Zero Traslo... " );
            delay(1000);
            Display_Test( pagina_test );
            break;

          case '4': // Stop
            // TODO per ora quando schiaccio un tasto qualsiasi mentre è in corso un'operazione, se quel tasto è 4 allora il traslo riceve questo messaggio 2 volte 
            inviaConRisposta( "02003", TRASLO_ADD );
            lcd.clear();
            lcd.setCursor( 0, 1 );
            lcd.print( "Ferma tutto... " );
            delay(1000);
            Display_Test( pagina_test );
            break;

          case 'A': 
            if( pagina_test < MAX_PAG ) pagina_test++; else pagina_test = 1; // cicla a pagina 1 se andiamo oltre la pagina massima  
            break;

          case 'B': 
            if( pagina_test > 1 ) 
              pagina_test--; 
            else
              pagina_test = MAX_PAG -1; // cicla all'ultima pagina se andiamo prima della prima pagina 
            break;

          case 'D': pagina_test = 0; break;
        }
        break;

      case 4: // Carro, pag 1
        switch( customKey ) {
          
          case '1': // Richiesta stato
            inviaConRisposta( "01001", CARRO_ADD ); // per questa richiesta di stato uso inviaConRisposta invece di spedisciStringa perché voglio una risposta immediata
            delay(1000);
            Display_Test( pagina_test ); // ricaricao la pagina per mostrare il nuovo stato, se è cambiato
            break;
          
          case '2': // Zero avanzamento
            inviaConRisposta( "02002", CARRO_ADD );
            lcd.clear();
            lcd.setCursor( 0, 1 );
            lcd.print( "Zero Avanz... " );
            delay(1000);
            Display_Test( pagina_test );
            break;

          case '3': // Zero sollevamento
            inviaConRisposta( "02001", CARRO_ADD );
            lcd.clear();
            lcd.setCursor( 0, 1 );
            lcd.print( "Zero Sollev... " );
            delay(1000);
            Display_Test( pagina_test );
            break;

          case '4': // Stop
            // TODO per ora quando schiaccio un tasto qualsiasi mentre è in corso un'operazione, se quel tasto è 4 allora il carro riceve questo messaggio 2 volte 
            inviaConRisposta( "02003", CARRO_ADD );
            lcd.clear();
            lcd.setCursor( 0, 1 );
            lcd.print( "Ferma tutto... " );
            delay(1000);
            Display_Test( pagina_test );
            
            break;

          case 'A': 
            if( pagina_test < MAX_PAG ) pagina_test++; else pagina_test = 1; // cicla a pagina 1 se andiamo oltre la pagina massima  
            break;

          case 'B': 
            if( pagina_test > 1 ) 
              pagina_test--; 
            else
              pagina_test = MAX_PAG -1; // cicla all'ultima pagina se andiamo prima della prima pagina 
            break;

          case 'D': pagina_test = 0; break;
        }
        break;

      case 5: // Carro pag 2
        switch( customKey ) {

          case '1': // Pulizia colonna corrente
            inviaConRisposta( "03001", CARRO_ADD );
            lcd.clear();
            lcd.setCursor( 0, 1 );
            lcd.print( "Pulizia Col " );
            lcd.print( Colonna );
            lcd.print( "...   " );
            delay(1000);
            Display_Test( pagina_test );
            break;

          case '2': // Test Sollevatore
            {
              uint16_t val = chiediValore( "Test Sollevatore %", 0, 100 );
              if( val != 0xFFFF ) {
                char buf[6];
                snprintf( buf, sizeof(buf), "04%03u", val );
                inviaConRisposta( buf, CARRO_ADD );
              }
              Display_Test( pagina_test );
            }
            break;

          case '3': // Test Avanzamento
            {
              uint16_t val = chiediValore( "Test Avanzamento %", 0, 100 );
              if( val != 0xFFFF ) {
                char buf[6];
                snprintf( buf, sizeof(buf), "05%03u", val );
                inviaConRisposta( buf, CARRO_ADD );
              }
              Display_Test( pagina_test );
            }
            break;

          case '4': // Test Spazzola
            {
              uint16_t val = chiediValore( "Test Spazzola %   ", 0, 100 );
              if( val != 0xFFFF ) {
                char buf[6];
                snprintf( buf, sizeof(buf), "06%03u", val );
                inviaConRisposta( buf, CARRO_ADD );
              }
              Display_Test( pagina_test );
            }
            break;

          case 'A': 
            if( pagina_test < MAX_PAG ) pagina_test++; else pagina_test = 1; // cicla a pagina 1 se andiamo oltre la pagina massima  
            break;

          case 'B': 
            if( pagina_test > 1 ) 
              pagina_test--; 
            else
              pagina_test = MAX_PAG -1; // cicla all'ultima pagina se andiamo prima della prima pagina 
            break;

          case 'D': pagina_test = 0; break;
        }
        break;

      case 6: // Carro pag 3 - Richieste
        switch( customKey ) {
          case '1': // RQST Sollevatore SX
            inviaConRisposta( "07001", CARRO_ADD );
            lcd.clear();
            lcd.setCursor( 0, 1 );
            lcd.print( "RQST Sollev. SX..." );
            delay(1000);
            Display_Test( pagina_test );
            break;

          case '2': // RQST Sollevatore DX
            inviaConRisposta( "08001", CARRO_ADD );
            lcd.clear();
            lcd.setCursor( 0, 1 );
            lcd.print( "RQST Sollev. DX..." );
            delay(1000);
            Display_Test( pagina_test );
            break;

          case '3': // RQST Avanzamento
            inviaConRisposta( "09001", CARRO_ADD );
            lcd.clear();
            lcd.setCursor( 0, 1 );
            lcd.print( "RQST Avanzamento..." );
            delay(1000);
            Display_Test( pagina_test );
            break;

          case '4': // RQST Spazzola
            inviaConRisposta( "10001", CARRO_ADD );
            lcd.clear();
            lcd.setCursor( 0, 1 );
            lcd.print( "RQST Spazzola...  " );
            delay(1000);
            Display_Test( pagina_test );
            break;

          case 'A': 
            if( pagina_test < MAX_PAG ) pagina_test++; else pagina_test = 1;
            break;

          case 'B': 
            if( pagina_test > 1 ) pagina_test--; else pagina_test = MAX_PAG ;
            break;

          case 'D': pagina_test = 0; break;
        }
        break;

      case 7: // Carro pag 4 - Set Altezza Spazzola
        switch( customKey ) {

          case '1': // Set Spazzola SX
            {
              uint16_t val = chiediValore( "Altezza Spazzola SX", 0, 255 );
              if( val != 0xFFFF ) {
                char buf[6];
                snprintf( buf, sizeof(buf), "11%03u", val );
                inviaConRisposta( buf, CARRO_ADD );
              }
              Display_Test( pagina_test );
            }
            break;

          case '2': // Set Spazzola DX
            {
              uint16_t val = chiediValore( "Altezza Spazzola DX", 0, 255 );
              if( val != 0xFFFF ) {
                char buf[6];
                snprintf( buf, sizeof(buf), "12%03u", val );
                inviaConRisposta( buf, CARRO_ADD );
              }
              Display_Test( pagina_test );
            }
            break;

          case 'A': 
            if( pagina_test < MAX_PAG ) pagina_test++; else pagina_test = 1;
            break;

          case 'B': 
            if( pagina_test > 1 ) pagina_test--; else pagina_test = MAX_PAG ;
            break;

          case 'D': pagina_test = 0; break;
        }
        break;

      case 8: // Traslo pag 2 - Richieste parametri
        switch( customKey ) {
          case '1': // RQST passi_Colonna_M1
            inviaConRisposta( "13001", TRASLO_ADD );
            lcd.clear();
            lcd.setCursor( 0, 1 );
            lcd.print( "RQST Passi M1...   " );
            delay(1000);
            Display_Test( pagina_test );
            break;

          case '2': // RQST passi_Colonna_M2
            inviaConRisposta( "14001", TRASLO_ADD );
            lcd.clear();
            lcd.setCursor( 0, 1 );
            lcd.print( "RQST Passi M2...   " );
            delay(1000);
            Display_Test( pagina_test );
            break;

          case '3': // RQST maxCol
            inviaConRisposta( "15001", TRASLO_ADD );
            lcd.clear();
            lcd.setCursor( 0, 1 );
            lcd.print( "RQST maxCol...     " );
            delay(1000);
            Display_Test( pagina_test );
            break;

          case 'A':
            if( pagina_test < MAX_PAG ) pagina_test++; else pagina_test = 1;
            break;

          case 'B':
            if( pagina_test > 1 ) pagina_test--; else pagina_test = MAX_PAG;
            break;

          case 'D': pagina_test = 0; break;
        }
        break;

      case 1: // navigation help page — only navigation keys active
      default:
        switch( customKey ) {
          case 'A': 
            if( pagina_test < MAX_PAG ) pagina_test++; else pagina_test = 1; // cicla a pagina 1 se andiamo oltre la pagina massima  
            break;

          case 'B': 
            if( pagina_test > 1 ) 
              pagina_test--; 
            else
              pagina_test = MAX_PAG; // cicla all'ultima pagina se andiamo prima della prima pagina 
            break;

          case 'D': pagina_test = 0; break;
        }
        break;
    }
    delay( 200 );
  } while( pagina_test );

  Homepage();
}
