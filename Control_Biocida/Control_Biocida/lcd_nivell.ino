// Pantalla principal de mostra de dades.

void lcd_nivell(){
  
  int LINEA = 0;
  lcd.setCursor(0, LINEA);
  lcd.print("                    ");
  lcd.setCursor(0, LINEA);
  lcd.print("       NIVELL       ");

  LINEA = 1;  
  lcd.setCursor(0, LINEA);
  lcd.print("                    ");
  lcd.setCursor(0, LINEA);

  if (Fallo_RTC){
    lcd.print("FALLO RTC TST I2C BS");
  }
  else {
    DateTime now = rtc.now();

    char Data[10];
    sprintf(Data,"%2u/%2u/%4u", now.day(), now.month(), now.year());
    lcd.print(Data);

    lcd.setCursor(11,LINEA);
    char Hora[8];  
    sprintf(Hora,"%2u:%2u:%2u", now.hour(), now.minute(), now.second());
    lcd.print(Hora);
  } 
  
  if (Fallo_ADS1115){ 
    lcd.print("FALLO  ADC");

    LINEA = 2;
    lcd.setCursor(0, LINEA);
    lcd.print("                    ");
    lcd.setCursor(6, LINEA);
    lcd.print("FALLO  ADC  ADS 1115");

    LINEA = 3;
    lcd.setCursor(0, LINEA);
    lcd.print("                    ");
    lcd.setCursor(0, LINEA);
    lcd.print("  REVISAR BUS  I2C  ");
  }
  else {
    LINEA = 2;  
    lcd.setCursor(0, LINEA);
    lcd.print("                    ");
    lcd.setCursor(0, LINEA);
    lcd.print(sns_Nivell_Volts, 3);  // Lectura Entrada Analògica en volts, 2 decimals
    lcd.print(" V ");

    lcd.setCursor(11, LINEA);
    lcd.print(sns_Nivell_mA, 2);
    lcd.print(" mA ");


    LINEA = 3;
    lcd.setCursor(0, LINEA);
    lcd.print("                    ");
    lcd.setCursor(0, LINEA);
    //lcd.print(sns_Nivell_Mtrs,3);
    //lcd.print(" m ");
    lcd.print(sns_Nivell_MM, 1);
    lcd.print(" mm ");

    lcd.setCursor(11, LINEA);
    lcd.print(sns_Nivell_Ltrs, 1);
    lcd.print(" L");
  }
}