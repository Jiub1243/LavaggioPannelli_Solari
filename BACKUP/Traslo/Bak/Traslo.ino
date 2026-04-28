/*////////////////////////////////////////////////////////////////////
//  Lavaggio Pannelli Solari 31/01/2025                             //
//                                                                  //
//  TRASLO                                                          //
//  (Arduino nella scatola sopra il traslo)                         //
//  M1 - Motore avanzamento superiore (cremagliera)                 //
//  0-285500 (1mm = 28,5 tic )                                      //
//                                                                  //
//  M2 - Motore avanzamento inferiore (ruote liscie)                //
//  0-11520  (1mm =   Solleva   tic)                                //
//    M2 = M1 / 1,732851985559567                                   //
////////////////////////////////////////////////////////////////////*/

#include <Wire.h> 
#include <LiquidCrystal_I2C.h> // https://github.com/johnrickman/LiquidCrystal_I2C
#include <Keypad.h> // Keypad da Mark Stanley, Alexander Brevig
#include <EEPROM.h>
#include <MobaTools.h>
#include "I2CKeyPad.h"

int signalPin = 12;

#define LCD_ADR 0x38  // 26Indirizzo I2C Display

const uint8_t KEYPAD_ADDRESS = 0x39; //26
I2CKeyPad keyPad(KEYPAD_ADDRESS);
uint32_t lastKeyPressed = 0;
int Old_Tasto;


// Motore Superiore (Cremagliera)
int StepPin_M1 = 22;
int DirPin_M1 = 23;
int EnaPin_M1 = 24;
uint8_t Error_Driver_M1 = 38;
uint8_t Sens_Zero_M1 = 42;
// Motore Inferiore (Ruote lisciie)
int StepPin_M2 = 25;
int DirPin_M2 = 26;
int EnaPin_M2 = 27;
uint8_t Error_Driver_M2 = 39;
uint8_t Sens_Zero_M2 = 43;


// Posizione Assi
bool Posizione_M1 = 0;
bool Posizione_M2 = 0;


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

void setup(){

  lcd.init(); 
  lcd.backlight();

  Wire.begin();
  Wire.setClock(400000);
  if (keyPad.begin() == false)
  {
    Serial.println("\nTastiera scollegata\n");
  }

  //stepper_M1.attachEnable( EnaPin_M1, 10, LOW );  // Commentare per FRENO
  stepper_M1.attach( StepPin_M1, DirPin_M1 );
  stepper_M1.setSpeed( 800 );             // 30 rev/min (if stepsPerRev is set correctly)
  stepper_M1.setRampLen( 400 ); // Ramp length is 1/2 revolution

  //stepper_M2.attachEnable( EnaPin_M2, 10, LOW );  // Commentare per FRENO
  stepper_M2.attach( StepPin_M2, DirPin_M2 );
  stepper_M2.setSpeed( 400 );             // 30 rev/min (if stepsPerRev is set correctly)
  stepper_M2.setRampLen( 200 ); // Ramp length is 1/2 revolution

  pinMode(Error_Driver_M1, INPUT_PULLUP);
  pinMode(Error_Driver_M2, INPUT_PULLUP);
  
  //pinMode(Error_Driver_M3, INPUT_PULLUP);
  //pinMode(Error_Driver_M4, INPUT_PULLUP);
  pinMode(Sens_Zero_M1, INPUT_PULLUP);
  pinMode(Sens_Zero_M2, INPUT_PULLUP);
  //pinMode(Sens_Zero_M4, INPUT_PULLUP);
  //pinMode(OltreCorsa_M4, INPUT_PULLUP);
  digitalWrite(signalPin, HIGH);
  Serial.begin( 115200 );
  while ( !Serial )
  {
    ; // wait for serial port to connect. Needed for native USB
  }

  Serial.println( "*********************************************************" );
  Serial.print( "Arduino - Lavaggio Pannelli Solari - TRASLO -" );
  Serial.print( __DATE__ );
  Serial.print( " " );
  Serial.println( __TIME__ );
  Serial.println( "*********************************************************" );
  Homepage();
  Serial1.begin( 115200 );

}

void loop(){
  do{

  }while( password());

  Test_01();
  Serial.println( "Fine Test" );
  Homepage();
}

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
    Serial1.print("AT+SEND=2,5,HELLO");
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

void Test_01(){
  uint8_t customKey, pagina_test=1;
  uint8_t sensore_M1, sensore_M2, sensore_M4;
  long Pos_M4;
  do{
    Display_Test( pagina_test );
    do{
      customKey = GetKey();
    } while (customKey == ' ');
    switch(customKey) {
      case '1' : { // Sollevatore
              if (pagina_test == 2) {
                //if(Posizione_M1 && Posizione_M2) {
                  if(1) {
                  Serial.println("Traslo SX");
                  sensore_M1 = digitalRead(Sens_Zero_M1);
                  sensore_M2 = digitalRead(Sens_Zero_M2);
                  //if( sensore_M1 ) stepper_M1.write(-800);
                  stepper_M1.doSteps( -4800 );
                  //if( sensore_M2 ) stepper_M2.write(-600);
                  stepper_M2.doSteps( -2770 );
                  do {
                    LCD_Posizione_M1_M2();
                    customKey = GetKey();
                    sensore_M1 = digitalRead(Sens_Zero_M1);
                    sensore_M2 = digitalRead(Sens_Zero_M2);
                    if( ! sensore_M1 ) stepper_M1.rotate(0);
                    if( ! sensore_M2 ) stepper_M2.rotate(0);
                    //delay(200);
                  }while ( ( stepper_M1.moving() || stepper_M2.moving() ) && ( customKey == ' ' ) );
                  stepper_M1.rotate(0);
                  stepper_M2.rotate(0);
                  LCD_Posizione_M1_M2();
                }
              }
              if (pagina_test == 3);
              if (pagina_test == 4);
              if (pagina_test == 5);
              if (pagina_test == 6);
        }
        break;
          case '2': {
              if (pagina_test == 2) ;
              if (pagina_test == 3) ;
              if (pagina_test == 4) ;
              if (pagina_test == 5) ;
        }
        break;
          case '3': {
              if (pagina_test == 2) ;
              if (pagina_test == 3) ;
              if (pagina_test == 4) ;
              if (pagina_test == 5) ;
        }
        break;
      case '4' : {
              if (pagina_test == 2) {
                //if(Posizione_M1 && Posizione_M2) {
                  if(1) {
                  Serial.println("Traslo DX");
                  sensore_M1 = digitalRead(Sens_Zero_M1);
                  sensore_M2 = digitalRead(Sens_Zero_M2);
                  //if( sensore_M1 ) stepper_M1.write(800);
                  stepper_M1.doSteps( 4800 );
                  //if( sensore_M2 ) stepper_M2.write(600);
                  stepper_M2.doSteps( 2770 );
                  do {
                    LCD_Posizione_M1_M2();
                    customKey = GetKey();
                    sensore_M1 = digitalRead(Sens_Zero_M1);
                    sensore_M2 = digitalRead(Sens_Zero_M2);
                    if( ! sensore_M1 ) stepper_M1.rotate(0);
                    if( ! sensore_M2 ) stepper_M2.rotate(0);
                    //delay(200);
                  }while ( ( stepper_M1.moving() || stepper_M2.moving() ) && ( customKey == ' ' ) );
                  stepper_M1.rotate(0);
                  stepper_M2.rotate(0);
                  LCD_Posizione_M1_M2();
                }
              }
              if (pagina_test == 3);
              if (pagina_test == 4);
              if (pagina_test == 5) ;
        }
        break;
      case '5': {
              if (pagina_test == 2) ;
              if (pagina_test == 3) ;
              if (pagina_test == 4) ;
              if (pagina_test == 5) ;
        }
        break;
      case '6': {
              if (pagina_test == 2) ;
              if (pagina_test == 3) ;
              if (pagina_test == 4) ;
              if (pagina_test == 5) ;
        }
        break;
      case '7': {
              if (pagina_test == 2) ;
              if (pagina_test == 3) ;
              if (pagina_test == 4) ;
              if (pagina_test == 5) ;
        }
        break;
      case '8': {
              if (pagina_test == 2) ;
              if (pagina_test == 3) ;
              if (pagina_test == 4) ;
              if (pagina_test == 5) ;

        }
        break;
      case '9': {  // Menu Principale
              if (pagina_test == 2) {
                Serial.println("Zero Traslo");
                lcd.clear();
                lcd.setCursor(0,1);
                lcd.print("Zero Traslo... ");
                Zero_Traslo();
                Display_Test(pagina_test);
              }
              if (pagina_test == 3);
              if (pagina_test == 4);
              if (pagina_test == 5);
          }
        break;
      case 'A': {
              if(pagina_test < 6) pagina_test++;
        }
        break;
      case 'B': {
              if(pagina_test > 1) pagina_test--;
        }
        break;
      case 'D': {
              pagina_test = 0;
        }
        break;
    }
    delay(200);
  } while(pagina_test);      //while(customKey != 'D');
  Homepage();
}

void LCD_Posizione_M1_M2 (void) {
  long Pos_M1, Pos_M2;
  lcd.setCursor(0,3);
  lcd.print("M1=");
  Pos_M1 = stepper_M1.read();
  lcd.print(Pos_M1);
  lcd.print("   ");
  lcd.setCursor(11,3);
  lcd.print("M2=");
  Pos_M2 = stepper_M2.read();
  lcd.print(Pos_M2);
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

    case 2 : { // Traslo
          lcd.clear();
          Pos_M1 = stepper_M1.read();
          Pos_M2 = stepper_M2.read();
          lcd.setCursor(0,0);
          lcd.print("1 Traslo SX    ");
          lcd.setCursor(0,1);
          lcd.print("4 Traslo DX   ");
          lcd.setCursor(0,2);
          lcd.print("9 Zero Traslo  ");
          lcd.setCursor(0,3);
          lcd.print("M1=");
          if(! Posizione_M1 ) {
            lcd.print("-----");
          }
          else lcd.print(Pos_M1);
          lcd.setCursor(11,3);
          lcd.print("M2=");
          if(! Posizione_M2 ) {
            lcd.print("-----");
          }
          else lcd.print(Pos_M2);
      }
      break;
    }
}

void Zero_Traslo(void){
  int sensore_SX_SU, sensore_SX_GIU;
  uint8_t customKey;
  Posizione_M1 = 0;
  Posizione_M2 = 0;
  sensore_SX_SU = digitalRead(Sens_Zero_M1);
  sensore_SX_GIU = digitalRead(Sens_Zero_M2);
  if( sensore_SX_SU ) stepper_M1.rotate(1);
  if( sensore_SX_GIU ) stepper_M2.rotate(1);
  do{
    customKey = GetKey();
    sensore_SX_SU = digitalRead(Sens_Zero_M1);
    sensore_SX_GIU = digitalRead(Sens_Zero_M2);
    if(! sensore_SX_SU) {
      stepper_M1.rotate(0);
      Posizione_M1 = 1;
      stepper_M1.setZero();
    }
    if(! sensore_SX_GIU) {
      stepper_M2.rotate(0);
      Posizione_M2 = 1;
      stepper_M2.setZero();
    }
  } while ( ( stepper_M1.moving() || stepper_M2.moving() ) && ( customKey == ' ' ) );
  stepper_M1.rotate(0);
  stepper_M2.rotate(0);
}
