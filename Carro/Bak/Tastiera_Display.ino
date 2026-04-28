// Leggi Tasto da Tastiera
char GetKey(void){
  char ritorno = ' ',  keys[] = "123A456B789C*0#DNF";  //  N = NoKey, F = Fail
  uint8_t  index = keyPad.getKey();

    if( index == 16) Old_Tasto = index;
    if((index != 16) && (Old_Tasto != index)) {
      Old_Tasto = index;
      ritorno = keys[index];
    }
  return( ritorno );
}

void clearData(void){
  while(data_count !=0){
    Data[data_count--] = 0; 
  }
  return;
}

// Display Pagina Iniziale
void Homepage() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("********************");
  lcd.setCursor(0,1);
  lcd.print("* Pulizia Pannelli *");
  lcd.setCursor(0,2);
  lcd.print("*   SINTEL V.d.H   *");
  lcd.setCursor(0,3);
  lcd.print("********************");
  Serial.println( "Homepage" );
}

// Digitazione Password
int password() {
  uint8_t customKey, passOK = 1;
  customKey = GetKey();
  if (customKey > ' '){
    if(data_count == 0) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("  PASSWORD...");
      millisecondi = millis();
    }
    Data[data_count] = customKey; 
    lcd.setCursor(data_count,1); 
    lcd.print(Data[data_count]); 
    data_count++; 
    delay(350);
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
  if( (data_count > 0) && (millis() > (millisecondi + 15000)) ){
    data_count = 0;
    clearData(); 
    Homepage();
  }
  return(passOK);
}

// Menu di TEST
void Test_01() {
  uint8_t customKey, pagina_test=1;
  uint8_t sensore_M1, sensore_M2, sensore_M4;
  long Pos_M4;
  do{
    Display_Test( pagina_test);
    do{
      customKey = GetKey();
    }while (customKey == ' ');
    switch(customKey) {
      case '1' : { // Sollevatore
              if (pagina_test == 2) {
                if(Posizione_M1 && Posizione_M2) {
                  Serial.println("Sollevatore SU");
                  sensore_M1 = digitalRead(Sens_Zero_M1);
                  sensore_M2 = digitalRead(Sens_Zero_M2);
                  if( sensore_M1 ) stepper_M1.write(0);
                  if( sensore_M2 ) stepper_M2.write(0);
                  //stepper_M1.doSteps( 5044 );
                  //stepper_M2.doSteps( 5044 );
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
              if (pagina_test == 3) {
                if( Posizione_M4 ) {
                  Serial.println("Avanzamento Avanti");
                  Accendi_M4();
                  Pos_M4 = stepper_M4.read() + 72000;
                  if (Pos_M4 > 285000) Pos_M4 = 285500;
                  stepper_M4.write(Pos_M4);
                  do {
                    LCD_Posizione_M4();
                    customKey = GetKey();
                    sensore_M4 = digitalRead(OltreCorsa_M4);
                    if( ! sensore_M4 ) stepper_M4.stop( );
                    //delay(200);
                  }while ( ( stepper_M4.moving() ) && ( customKey == ' ' ) );
                  stepper_M4.rotate(0);
                  while ( stepper_M4.moving() );
                  LCD_Posizione_M4();
                  //digitalWrite(Collega_M4, LOW);
                  //digitalWrite(Accendi_Driver_M4, LOW);
                }
              }
              if (pagina_test == 4) {
                if(! stepper_M3.moving()){
                  Serial.println("1 Spazzola On");
                  //lcd.setCursor(0,1);
                  //lcd.print("1 Spazzola Off      ");
                }
                if( stepper_M3.moving()) {
                  Serial.println("1 Spazzola OFF");
                  //lcd.setCursor(0,1);
                  //lcd.print("1 Spazzola On        ");
                }
                Rotazione_Spazzola();
              }
              if (pagina_test == 5);
              if (pagina_test == 6) Spazzolatura();
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
              if (pagina_test == 2) {
                Serial.println("Tasto 3");
                for(int i=0; i<8; i++){
                  stepper_M1.doSteps( -214 );
                  stepper_M2.doSteps( -214 );
                  while(stepper_M1.moving() || stepper_M2.moving());
                  delay(1000);
                }
              }

              if (pagina_test == 3) ;
              if (pagina_test == 4) ;
              if (pagina_test == 5) ;
        }
        break;
      case '4' : {
              if (pagina_test == 2) {
                if(Posizione_M1 && Posizione_M2) {
                  Serial.println("Sollevatore GIU");
                  stepper_M1.write(-11520);
                  stepper_M2.write(-11520);
                  //stepper_M1.doSteps( -5044 );
                  //stepper_M2.doSteps( -5044 );
                  /*stepper_M1.doSteps( -950 );
                  stepper_M2.doSteps( -950 );
                  delay(5000);
                  stepper_M1.doSteps( -2550 );
                  stepper_M2.doSteps( -2550 );
                  delay(5000);
                  stepper_M1.doSteps( 950 );
                  stepper_M2.doSteps( 950 );
                  delay(5000);
                  stepper_M1.doSteps( 2550 );
                  stepper_M2.doSteps( 2550 );*/

                  do {
                    LCD_Posizione_M1_M2();
                    customKey = GetKey();
                    delay(200);

                  }while ( ( stepper_M1.moving() || stepper_M2.moving() ) && ( customKey == ' ' ) );
                  stepper_M1.rotate(0);
                  stepper_M2.rotate(0);
                  LCD_Posizione_M1_M2();
                }
              }

              if (pagina_test == 3) {
                if( Posizione_M4 ) {
                  Serial.println("Avanzamento Indietro");
                  Accendi_M4();
                  Pos_M4 = stepper_M4.read();
                  if(Pos_M4 > 72000) { 
                    Pos_M4 -= 72000;
                  }
                  else Pos_M4 = 0;
                  stepper_M4.write(Pos_M4);
                  do {
                    LCD_Posizione_M4();
                    customKey = GetKey();
                    sensore_M4 = digitalRead(Sens_Zero_M4);
                    if( ! sensore_M4 ) stepper_M4.stop( );
                    //delay(200);
                  }while ( ( stepper_M4.moving() ) && ( customKey == ' ' ) );
                  stepper_M4.rotate(0);
                  while ( stepper_M4.moving() );
                  LCD_Posizione_M4();
                  //digitalWrite(Collega_M4, LOW);
                  //digitalWrite(Accendi_Driver_M4, LOW);
                }
              }
              //if (pagina_test == 4);
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
              if (pagina_test == 2) {
                Serial.println("Tasto 6");
                for(int i=0; i<8; i++){
                  stepper_M1.doSteps( 214 );
                  stepper_M2.doSteps( 214 );
                  while(stepper_M1.moving() || stepper_M2.moving());
                  delay(1000);
                }
              }
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
              if (pagina_test == 4) {
                if ( Velocita_Spazzola > 100 ) {
                  Velocita_Spazzola-=100;
                }
                Serial.print("Velocita Rotazione spazzola ");
                Serial.println(Velocita_Spazzola);
                stepper_M3.setSpeed( Velocita_Spazzola );
              }
              if (pagina_test == 5) ;

        }
        break;
      case '9': {  // Menu Principale
              if (pagina_test == 2) {
                Serial.println("Zero Sollevatore");
                lcd.clear();
                lcd.setCursor(0,1);
                lcd.print("Zero Sollevatore... ");
                Zero_Sollevatore();
                Display_Test(pagina_test);
              }
              if (pagina_test == 3) {
                Serial.println("Zero Avanzamento");
                lcd.clear();
                lcd.setCursor(0,1);
                lcd.print("Zero Avanzamento... ");
                Zero_Avanzamento();
                Display_Test(pagina_test);
              }
              if (pagina_test == 4) {
                Velocita_Spazzola+=100;
                Serial.print("Velocita Rotazione spazzola ");
                Serial.println(Velocita_Spazzola);
                stepper_M3.setSpeed( Velocita_Spazzola );
              }
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

void LCD_Posizione_M1_M2 () {
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

void LCD_Posizione_M4 () {
  long Pos_M4;
  lcd.setCursor(0,3);
  lcd.print("Pos. M4= ");
  Pos_M4 = stepper_M4.read();
  if( Posizione_M4 ) lcd.print(Pos_M4);
  else lcd.print("--------");
  lcd.print("    ");
}               

void LCD_Spazzolatura() {
  long Posiz_M1, Posiz_M2, Posiz_M4, misura_1, misura_2;
  //lcd.clear();
  //misura_1 = Leggi_Altezza_SX();
  lcd.setCursor(0,0);
  lcd.print("   Spazzolatura     ");
  lcd.setCursor(0,1);
  lcd.print("                    ");
  lcd.setCursor(0,1);
  if(misura_1 == 0) {
    lcd.print("--------");
  }
  else lcd.print(misura_1);
  //misura_2 = Leggi_Altezza_DX();
  lcd.print("  ");
  lcd.setCursor(10,1);
  if(misura_2 == 0) {
    lcd.print("--------");
  }
  else lcd.print(misura_2);
  lcd.print("  ");

  Posiz_M1 = stepper_M1.read();
  lcd.setCursor(0,2);
  lcd.print("M1=");
  if(! Posizione_M1 ) {
    lcd.print("-----");
  }
  else lcd.print(Posiz_M1);

  Posiz_M2 = stepper_M2.read();
  lcd.setCursor(11,2);
  lcd.print("M2=");
  if(! Posizione_M2 ) {
    lcd.print("------");
  }
  else lcd.print(Posiz_M2);
  lcd.setCursor(0,3);
  lcd.print("Pos. M4= ");
  Posiz_M4 = stepper_M4.read();
  if(! Posizione_M4) { //(Posizione_M4 non definita
    lcd.print("-------");
  }
  else lcd.print(Posiz_M4);

}     

void Display_Test(uint8_t pagina){
  uint8_t customKey;
  long Pos_M1, Pos_M2, Pos_M4, mis_1, mis_2;
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

    case 2 : { // Sollevatore
          lcd.clear();
          Pos_M1 = stepper_M1.read();
          Pos_M2 = stepper_M2.read();
          lcd.setCursor(0,0);
          lcd.print("1 Sollevatore SU    ");
          lcd.setCursor(0,1);
          lcd.print("4 Sollevatore GIU   ");
          lcd.setCursor(0,2);
          lcd.print("9 Zero Sollevatore  ");
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

    case 3 : { // Avanzamento
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("1 Avanti            ");
          lcd.setCursor(0,1);
          lcd.print("4 Indietro          ");
          lcd.setCursor(0,2);
          lcd.print("9 Zero Avanzamento  ");
          lcd.setCursor(0,3);
          lcd.print("Pos. M4= ");
          if(! Posizione_M4) { //(Posizione_M4 non definita
            lcd.print("--------");
          }
          else lcd.print(Pos_M4);
      }
      break;

    case 4: { // Spazzola
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(" Rotazione Spazzola ");
          lcd.setCursor(0,1);
          if(! stepper_M3.moving()) lcd.print("1 Spazzola On       ");
          if(  stepper_M3.moving()) lcd.print("1 Spazzola Off      ");
          lcd.setCursor(0,2);
          lcd.print("8 Vel. -   9 Vel. + ");
          lcd.setCursor(0,3);
          lcd.print(" Velocita' ");
          lcd.print(Velocita_Spazzola);
      }
      break;
    case 5: { // Sensori Distanza
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Sensore Distanza  SX");
          lcd.setCursor(0,1);
          lcd.print(" 00000 cen.         ");
          lcd.setCursor(0,2);
          lcd.print("Sensore Distanza  DX");
          lcd.setCursor(0,3);
          lcd.print(" 00000 cen.         ");
          customKey = ' ';
          do {
            mis_1 = Leggi_Altezza_SX();
            lcd.setCursor(0,1);
            lcd.print(mis_1);
            lcd.print(" cen.      ");
            mis_2 = Leggi_Altezza_DX();
            lcd.setCursor(0,3);
            lcd.print(mis_2);
            lcd.print(" cen.      ");
            customKey = GetKey();
          } while ( customKey == ' ' );
          if( customKey == '9' ) {
            Serial2.print("R");
            Serial3.print("R");
          }
          pagina = 4;
      }
      break;
      case 6: { // Test spazzolatura
          lcd.clear();
          LCD_Spazzolatura ();
          lcd.setCursor(0,0);
          lcd.print("1= Test Spazzolatura");
          /*do {
            customKey = GetKey();
            delay(300);
        } while ( customKey == ' ' );
        pagina = 4;*/
      }
      break;
    }
}
