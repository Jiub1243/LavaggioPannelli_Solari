
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
    Serial1.print("AT+SEND=2,5,HELLO"); //TODO commentare questa linea?
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
  int32_t passi, passi_M1;
  //long Pos_M4;
  do{
    Display_Test( pagina_test );
    do{
      customKey = GetKey();
    } while (customKey == ' ');
    switch(customKey) {
      case '1' : { // Sollevatore
              if (pagina_test == 2) {
                //if(Posizione_M1 && Posizione_M2) {
                  if(Traslo_OK) {
                  Serial.println("Traslo SX");
                  sensore_M1 = digitalRead(Sensore_Zero_M1);
                  sensore_M2 = digitalRead(Sensore_Posizione_M2);
                  passi = -2770;
                  passi_M1 = passi * 1.732851985559567; //TODO usare il punto o la virgola?
                  stepper_M1.doSteps( passi_M1 ); // 4800
                  stepper_M2.doSteps( passi ); // 2770
                  do {
                    LCD_Posizione_M1_M2();
                    customKey = GetKey();
                    sensore_M1 = digitalRead(Sensore_Zero_M1);
                    sensore_M2 = digitalRead(Sensore_Posizione_M2);
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
                  if(Traslo_OK) {
                  Serial.println("Traslo DX");
                  sensore_M1 = digitalRead(Sensore_Zero_M1);
                  sensore_M2 = digitalRead(Sensore_Posizione_M2);
                  //if( sensore_M1 ) stepper_M1.write(800);
                  stepper_M1.doSteps( 4800 );
                  //if( sensore_M2 ) stepper_M2.write(600);
                  stepper_M2.doSteps( 2770 );
                  do {
                    LCD_Posizione_M1_M2();
                    customKey = GetKey();
                    sensore_M1 = digitalRead(Sensore_Zero_M1);
                    sensore_M2 = digitalRead(Sensore_Posizione_M2);
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
                //Serial.println("Zero Traslo");
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
      break;
    }
}
