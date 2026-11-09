// changelog
// aggiunto i parametri Motore Superiore (Cremagliera)
// cambiato host con client per sottolineare che i messaggi ricevuti sono risposte e non comandi

#ifndef MYDEF_H

  #define MYDEF_H 
  
  int signalPin = 12; // Led integrato

  // Wifi credentials 
  const char* ssid_AP1     = "Sintel";
  const char* password_AP1 = "sintel123";
  const char* ssid_AP2     = "Matilde";
  const char* password_AP2 = "Francesco10";
  const char* ssid_AP3     = "Alessandro";
  const char* password_AP3 = "francesco";
  // Limours; Champagne; 
  const char* ssid_AP4     = "T-BOX";
  const char* password_AP4 = "T-BoxW&G23";

  WiFiMulti WiFiMulti;
  const uint32_t connectTimeoutWiFi = 12000;  // Timeout connessione WiFi 12 sec. TODO perché 12 secondi?

  // MQTT TODO sono corrette?
  #define MQTT_BROKER       "almotech.myqnapcloud.com"      // hostname o IP del broker 
  #define MQTT_PORT         32768                       
  #define MQTT_USER         "sintel"                        // vuoto se broker pubblico
  #define MQTT_PASSWORD     "telsin12"                      // vuoto se broker pubblico
  #define MQTT_CLIENT_ID    "LavaggioPannelli_Solari"       // deve essere unico sul broker

  // Topics MQTT
  #define MQTT_TOPIC_PREFIX     "sintel/lavaggio/quadro"
  #define MQTT_TOPIC_ONLINE     MQTT_TOPIC_PREFIX "/online"
  #define MQTT_TOPIC_TRASLO     MQTT_TOPIC_PREFIX "/traslo/stato"
  #define MQTT_TOPIC_CARRO      MQTT_TOPIC_PREFIX "/carro/stato"
  #define MQTT_TOPIC_CMD        MQTT_TOPIC_PREFIX "/cmd"

  // Istanza client MQTT
  WiFiClient wifiClientMQTT;
  PubSubClient mqttClient(wifiClientMQTT);

  // Stato MQTT - per riconnessione non bloccante 
  uint32_t lastReconnectAttemptMQTT = 0;
  #define MQTT_RECONNECT_INTERVAL 5000  // ritenta connessione ogni 5s

  // Per pubblicazione su cambio: ricorda l'ultimo stato pubblicato
  uint8_t ultimoStatoTrasloPubblicato = 0xFF;  // 0xFF = mai pubblicato
  uint8_t ultimoStatoCarroPubblicato  = 0xFF;

  // OTA Firmware Update
  #define FIRMWARE_URL "https://the-cerulean-pilgrimage.neocities.org/assets/Quadro.ino.bin"  // TODO: aggiornare con l'URL reale
  bool FW_UPDATE = 0;  // Flag settata da MQTT per triggerare l'aggiornamento
  
  // LCD
  #define LCD_ADR 0x38  // 26Indirizzo I2C Display
  LiquidCrystal_I2C lcd(LCD_ADR, 20, 4); // Definizione Display I2C 20 X 4

  // KP
  #define KEYPAD_ADDRESS 0x39 //26 Indirizzo KP
  I2CKeyPad keyPad(KEYPAD_ADDRESS); // Definizione Tastiera I2C
  uint32_t lastKeyPressed = 0;
  int Old_Tasto;

  // Usati nei menu
  #define VELOCITA_M1_ZERO           87  // Velocità M1 durante lo 0 asse
  #define VELOCITA_M2_ZERO           50  // Velocità M2 durante lo 0 asse
  #define VELOCITA_M1_AVANZAMENTO   693  // 800 Velocità M1 durante l'avanzamento
  #define VELOCITA_M2_AVANZAMENTO   400  // Velocità M2 durante l'avanzamento
  
  uint8_t STATO_CORRENTE_CARRO = 0;
  uint8_t STATO_CORRENTE_TRASLO = 0;
  bool ARRESTA = 0;

  uint8_t Colonna = 0; // Colonna Corrente
  uint8_t maxCol = 39; // 39 
  uint8_t maxCol_da_confermare;

  bool cneFlag = 0; // Comando Non Eseguibile. Diventa 1 quando ricevo questo messaggio
  bool commsFailFlag = false; // Nessuna risposta ricevuta. Diventa 1 quando passano TIMEOUT_RISPOSTA millisecondi senza ricevere una risposta
  bool offlineCarroFlag  = false; // flag per il timeout delle richieste di stato
  bool offlineTrasloFlag  = false;

  long passi_Colonna_M1 = 4800; // 4800
  long passi_Colonna_M1_da_confermare;
  long passi_Colonna_M2 = 2770; // 2770
  long passi_Colonna_M2_da_confermare;

  long passi0_M1 = 4800; // passi M1 tra Zero Traslo e colonna 1
  long passi0_M1_da_confermare;
  long passi0_M2 = 2770; // passi M2 tra Zero Traslo e colonna 1
  long passi0_M2_da_confermare;

  // Corse massime del Carro (programmabili e salvate in EEPROM sul Carro)
  // Sul Quadro le teniamo come modulo positivo per la visualizzazione; sul Carro CORSA_M1M2 è salvata negativa.
  long CORSA_M1M2 = 11500;   // modulo corsa sollevatori spazzola SX/DX. Protocollo: valore/100
  long CORSA_M1M2_da_confermare;
  long CORSA_M4 = 100000;    // corsa massima avanzamento carro. Protocollo: valore/1000
  long CORSA_M4_da_confermare;

  // Valori correnti del Carro letti via RQST (risposte 07-12), mostrati sulle pagine TEST
  uint8_t altezza_Sollevatore_SX = 0; // 07: altezza sollevatore SX (%)
  uint8_t altezza_Sollevatore_DX = 0; // 08: altezza sollevatore DX (%)
  uint8_t perc_Avanzamento = 0;       // 09: avanzamento carro (%)
  uint8_t perc_Spazzola = 0;          // 10: velocita' rotazione spazzola (%)
  uint8_t abbassamento_SX = 0;        // 11: altezza spazzolatura SX impostata (1/2 mm)
  uint8_t abbassamento_DX = 0;        // 12: altezza spazzolatura DX impostata (1/2 mm)

  
  // Motore Superiore (Cremagliera)
  int StepPin_M1 = 22;
  int DirPin_M1 = 23;
  int EnaPin_M1 = 24;
  uint8_t Error_Driver_M1 = 38;
  uint8_t Sensore_Zero_M1       = 43; // Sensore di ZERO Motore 1
  uint8_t Sensore_Posizione_M1  = 42; // Sensore di Posizione M1
  // Motore Inferiore (Ruote lisciie)
  int StepPin_M2 = 25;
  int DirPin_M2 = 26;
  int EnaPin_M2 = 27;
  uint8_t Error_Driver_M2 = 39;
  uint8_t Sensore_Posizione_M2  = 40; // Sensore di ZERO e Posizione M2

  // Posizione Assi azzerati
  bool Traslo_OK = 0;

  // PW
  //#define Password_Length 7 
  #define Password_Length 2
  char Data[Password_Length]; 
  //char Master[Password_Length] = "123456";
  char Master[Password_Length] = "1";  
  byte data_count = 0, master_count = 0;
  bool Pass_is_good;
  #define MAX_PAG 11 // num of pages for the LCD

  // Motori
  const int stepsPerRev_M1 = 400;    // Steps per revolution - may need to be adjusted
  const int stepsPerRev_M2 = 400;    // Steps per revolution - may need to be adjusted (10 impulsi su 800)
  MoToStepper stepper_M1( stepsPerRev_M1, STEPDIR );  // create a stepper instance
  MoToStepper stepper_M2( stepsPerRev_M2, STEPDIR );  // create a stepper instance

  // Uart_1
  #define MESS_START_MESS  '+' // Inizio messaggio = +
  #define MESS_END_MESS    0x0A // Fine messaggio = Line Feed
  #define LUN_BUF_232 64 // Lunghezza buffer UART
  #define MESS_LUN_REC 15 // Lunghezza messaggi ricevuti
  #define MESS_LUN_OK 5 // Lunghezza messaggio +OK
  #define MESS_LUN_ERR 10 // Lunghezza messaggio +ERR=xx

  #define CARRO_ADD '2' // address del carro per comunicazioni LoRa
  #define TRASLO_ADD '1' // address del traslo per comunicazioni LoRa

  char RS232_1_buffer_RX[LUN_BUF_232];
  //char RS232_1_buffer_TX[LUN_BUF_232]; // TODO toglierlo? Non è mai usato
  bool Messaggio_OK;
  bool RS232_1_newData;

  // Status polling, per le richieste di stato
  uint32_t Ultimo_Poll_Traslo = 0;
  uint32_t Ultimo_Poll_Carro = 0;
  #define INTERVALLO_POLL 60000  // 60 secondi

  // Offline detection, per le risposte 
  uint32_t Ultimo_Msg_Traslo = 0;
  uint32_t Ultimo_Msg_Carro = 0;
  #define TIMEOUT_OFFLINE 900000 // 15 minuti

  // Retry management: per rimandare i meesaggi che non ricevono risposta entro tot tempo 
  bool Attesa_Risposta = false;
  uint32_t Tempo_Invio = 0;
  uint8_t Tentativi_Invio = 0;
  char Destinatario_Atteso = '0';
  String Messaggio_Buffered = "";
  #define TIMEOUT_RISPOSTA 100000 // 100 secondi
  #define MAX_TENTATIVI 3

  typedef struct S_ANTENNA_PROTOCOL {
    char piu;
    char tipo_messaggio[3];
    char uguale;
    char mittente;
    char virgola;
    char lunghezza_msg[5];
    char CrLf[8];
    char tappo;
  } RS232_1_PROTOCOL;

  typedef struct S_DATI_ANTENNA {
    char messaggio[6];
    char mittente; // 1: 
    uint8_t comando;
    uint16_t dato;
  } DATI_CLIENT;

  RS232_1_PROTOCOL fromHost; // TODO serve a qualcosa
  DATI_CLIENT datiClient;

  // Struct per gestire i tentativi per un comando successivo ad un errore
  struct RetryInfo {
    uint8_t contatore;
    uint8_t maxTentativi;
    bool    fallito;       // flag per la UI
    char    comando[6];
    char    destinatario;
  };

  RetryInfo retryErroreTraslo  = { 0, 3, false, "", TRASLO_ADD };
  RetryInfo retryErroreCarro = { 0, 3, false, "", CARRO_ADD }; // Siccome i comandi 2001, 2002, 3001, 04xxx e 05xxx possono triggerare ognuno dei 4 errori del carro, il comando da richiamare viene copiato al bisogno.

  // Stati per la pulizia totale
  enum StatoPulisciTutto {
    PT_ZERO_TRASLO,        // ho mandato 02001 a Traslo, aspetto che torni Idle
    PT_VAI_COLONNA,        // ho mandato 03NNN a Traslo, aspetto che arrivi
    PT_PULIZIA_COLONNA,    // ho mandato 03001 a Carro, aspetto fine spazzolatura
    PT_COMPLETATO,
    PT_ABORTITO_STOP,
    PT_ABORTITO_ERRORE,
    PT_ABORTITO_OFFLINE
  };
  


#endif