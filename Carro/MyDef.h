/* changelog
aggiunto il parametro Velocita_Corrente_M3 che tiene contro dei cambiamenti assegnati alla velocità della spazzola
*/
#ifndef MYDEF_H

  #define MYDEF_H 

  #define debug 1

  int signalPin = 12;

  #define LCD_ADR 0x27  // 38 26 Indirizzo I2C Display
  #define KEYPAD_ADDRESS 0x39 //26 Indirizzo I2C Tastiera

  uint32_t lastKeyPressed = 0;
  int Old_Tasto;
  uint8_t STATO_CORRENTE = 0;
  
  bool ARRESTA = 0;

  // Corsa massima assi
  #define CORSA_M1M2 -11500   // Corsa massima M1 e M2
  #define CORSA_M4   100000   // Corsa massima M4 285500
  // Altezza START per controllo sensori Altezza
  #define ALTEZZA_M1 -1500 // -1400 Altezza di TEST SENSORE SX Appoggio ruote misuratrici Altezza
  #define ALTEZZA_M2 -1670 // -1610 Altezza di TEST SENSORE DX Appoggio ruote misuratrici Altezza

  //Altezza Spazzola dai pannelli (mezzo millimetro)
  unsigned int Abbassamento_SX;
  unsigned int Abbassamento_DX;
  // Sollevatore Spazzola Sinistro
  int StepPin_M1 = 22;
  int DirPin_M1 = 23;
  int EnaPin_M1 = 24;
  uint8_t Error_Driver_M1 = 38;
  uint8_t Sens_Zero_M1 = 42;
  // Sollevatore Spazzola Destro
  int StepPin_M2 = 25;
  int DirPin_M2 = 26;
  int EnaPin_M2 = 27;
  uint8_t Error_Driver_M2 = 39;
  uint8_t Sens_Zero_M2 = 43;
  // Motore Spazzola
  int StepPin_M3 = 28;
  int DirPin_M3 = 29;
  int EnaPin_M3 = 30;
  uint8_t Accendi_Driver_M3 = 34;
  uint8_t Error_Driver_M3 = 40;
  uint8_t Spazzola = 0 ;
  #define VELOCITA_MAX 3500 // 1000 velocità massima per la spazzola
  uint16_t Velocita_Spazzola = 800; // 1000 velocità correntemente impostata per quando la spazzola deve accendersi
  uint16_t Velocita_Corrente_M3 = 0; // velocità corrente della spazzola. Uguale a Velocita_Spazzola quando la spazzola è attiva, 0 altrimenti

  // Motore Avanzamento
  int StepPin_M4 = 31;
  int DirPin_M4 = 32;
  int EnaPin_M4 = 33;
  uint8_t Accendi_Driver_M4 = 35;
  uint8_t Collega_M4 = 36; // Spegnere per freno
  uint8_t Error_Driver_M4 = 41;
  uint8_t Sens_Zero_M4 = 44;
  uint8_t OltreCorsa_M4 = 45;
  // Ventola Quadro
  uint8_t Ventola = 37;

  //uint8_t contaLetture_SX, contaLetture_DX;

  // Posizione Assi non azzerate -> Azzerare assi
  bool Posizione_M1 = 0;
  bool Posizione_M2 = 0;
  bool Posizione_M4 = 0;

  // Sensori Posizione Altezza Spazzola
  // unsigned char  data1[13];
  //int Ultrasuoni_1, Ultrasuoni_2;
  int CalcUS_1, CalcUS_2;
  long CalcDis_1, CalcDis_2;
  int CntDis_1, CntDis_2;
  #define TIMEOUT_COM1 200 // Millisecondi di timeout sulle COM

  #define Password_Length 7 
  char Data[Password_Length]; 
  char Master[Password_Length] = "123456"; 
  byte data_count = 0, master_count = 0;
  bool Pass_is_good;
  uint32_t millisecondi;


  const int stepsPerRev = 400;    // Steps per revolution - may need to be adjusted
  const int stepsPerRev_LO = 200; // Steps per revolution - Motori driver Rossi (devono essere programmati via seriale)
  MoToStepper stepper_M1( stepsPerRev, STEPDIR );  // Instanza Motore Sollevatore Spazzola Sinistro
  MoToStepper stepper_M2( stepsPerRev, STEPDIR );  // Instanza Motore Sollevatore Spazzola Destro
  MoToStepper stepper_M3( stepsPerRev_LO, STEPDIR );  // Instanza Motore Spazzola
  MoToStepper stepper_M4( stepsPerRev_LO, STEPDIR );  // Instanza Motore Carro

  #define VELOCITA_M4_ZERO          1500  // Velocità M4 durante lo 0 asse
  #define VELOCITA_M4_SPAZZOLATURA  3000  // Velocità M4 durante la spazzolatura
  #define VELOCITA_M4_RITORNO       5000  // Velocità M4 durante il ritorno

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