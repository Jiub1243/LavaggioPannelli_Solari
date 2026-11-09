#ifndef MYDEF_H

  #define MYDEF_H 
  
  int signalPin = 12;

  #define LCD_ADR 0x27  // 26 Indirizzo I2C Display

  #define VELOCITA_M1_ZERO           87  // Velocità M1 durante lo 0 asse
  #define VELOCITA_M2_ZERO           50  // Velocità M2 durante lo 0 asse
  #define VELOCITA_M1_AVANZAMENTO   693  // 800 Velocità M1 durante l'avanzamento
  #define VELOCITA_M2_AVANZAMENTO   400  // Velocità M2 durante l'avanzamento

  #define EE_ADDR_PASSI_M1   515   // long, 4 byte: 515-518
  #define EE_ADDR_PASSI_M2   519   // long, 4 byte: 519-522
  #define EE_ADDR_MAXCOL     523   // uint8_t, 1 byte: 523
  #define EE_ADDR_PASSI0_M1  524   // long, 4 byte: 524-527
  #define EE_ADDR_PASSI0_M2  528   // long, 4 byte: 528-531

  const uint8_t KEYPAD_ADDRESS = 0x39; //26
  I2CKeyPad keyPad(KEYPAD_ADDRESS);
  uint32_t lastKeyPressed = 0;
  int Old_Tasto;

  uint8_t STATO_CORRENTE = 0;
  bool ARRESTA = 0;

  uint8_t Colonna; // Colonna Corrente
  uint8_t maxCol = 39; // numero di colonne

  long passi_Colonna_M1 = 4800;
  long passi_Colonna_M2 = 2770;

  long passi0_M1 = 4800; // passi M1 tra la posizione 0 (Zero Traslo) e la colonna 1, configurabile separatamente
  long passi0_M2 = 2770; // passi M2 tra la posizione 0 (Zero Traslo) e la colonna 1, configurabile separatamente

  // Motore Superiore (Cremagliera)
  int StepPin_M1 = 22;
  int DirPin_M1 = 23;
  int EnaPin_M1 = 24;
  uint8_t Error_Driver_M1 = 38;
  uint8_t Sensore_Zero_M1       = 42; // Sensore di ZERO Motore 1
  uint8_t Sensore_Posizione_M1  = 43; // Sensore di Posizione M1
  // Motore Inferiore (Ruote lisciie)
  int StepPin_M2 = 25;
  int DirPin_M2 = 26;
  int EnaPin_M2 = 27;
  uint8_t Error_Driver_M2 = 39;
  uint8_t Sensore_Posizione_M2  = 40; // Sensore di ZERO e Posizione M2

  // Posizione Assi azzerati
  bool Traslo_OK = 0;

  #define Password_Length 7 
  char Data[Password_Length]; 
  char Master[Password_Length] = "123456"; 
  byte data_count = 0, master_count = 0;
  bool Pass_is_good;

  LiquidCrystal_I2C lcd(LCD_ADR, 20, 4);

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
  char RS232_1_buffer_RX[LUN_BUF_232];
  //char RS232_1_buffer_TX[LUN_BUF_232]; // TODO toglierlo? Non è mai usato
  bool Messaggio_OK;
  bool RS232_1_newData;

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
  char mittente;
  uint8_t comando;
  uint16_t dato;
} DATI_HOST;

RS232_1_PROTOCOL fromHost;
DATI_HOST datiHost;


#endif