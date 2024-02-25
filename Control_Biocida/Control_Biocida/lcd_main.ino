// Pantalla principal de mostra de dades.

void lcd_main(){
  
  DateTime now = rtc.now();

  int LINEA = 0;  
  lcd.setCursor(0, LINEA);
  lcd.print("                    ");
  lcd.setCursor(0, LINEA);
  char Data[10];
  sprintf(Data,"%2u/%2u/%4u", now.day(), now.month(), now.year());
  lcd.print(Data);

  lcd.setCursor(11,LINEA);
  char Hora[8];  
  sprintf(Hora,"%2u:%2u:%2u", now.hour(), now.minute(), now.second());
  lcd.print(Hora);

  LINEA = 1;
  lcd.setCursor(0, LINEA);
  lcd.print("                    ");
  lcd.setCursor(0, LINEA);
  lcd.print("NVL: ");
  lcd.print(sns_Nivell_MM, 0);
  lcd.print(" mm ");

  lcd.setCursor(10, LINEA);
  lcd.print(sns_Nivell_Ltrs, 1);
  lcd.print(" L");

  LINEA = 2;  
  lcd.setCursor(0, LINEA);
  lcd.print("                    ");
  lcd.setCursor(0, LINEA);
  lcd.print("BOMBA DOSI:");
//  lcd.print(BOMBA_DOSI, 0);
  lcd.print(Bomba_Dosi, 0);
  lcd.print(" %");

  LINEA = 3;
  lcd.setCursor(0, LINEA);
  lcd.print("                    ");
  lcd.setCursor(0, LINEA);
  lcd.print("Duty: ");
  lcd.print(Duty, 0);
  lcd.print(" % ");
  lcd.setCursor(11, LINEA);
  lcd.print("Ton:");
//  lcd.print(Ton_ms);
//  lcd.print(" ms ");
  lcd.print(float(Ton_ms) / 1000, 0);
  lcd.print(" s ");
}  
