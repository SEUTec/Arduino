/* Dades a mostrar en la pantalla durant el Test de LEDS Inicial
*/

void lcd_test(){

  lcd.clear();

  int LINEA = 0;
  lcd.setCursor(0, LINEA);
  lcd.print("CTRL DOSI BIOCIDA" );

  LINEA = 1;  
  if (!Fallo_ADS1115) {
    lcd.setCursor(0,LINEA);
    lcd.print("ADC ADS1115 OK");
  }
  else {
    lcd.setCursor(0,LINEA);
    lcd.print("ADC ADS1115 FALLO");
  }

  LINEA = 2;
  if (!Fallo_RTC){
    lcd.setCursor(0,LINEA);
    lcd.print("RTC OK");
  }
  else {
    lcd.setCursor(0,LINEA);
    lcd.print("RTC FALLO");
  }

  LINEA = 3;
  if (!Fallo_DAC){
    lcd.setCursor(0,LINEA);
    lcd.print("DAC MCP4725 OK");
  }
  else {
    lcd.setCursor(0,LINEA);
    lcd.print("DAC MCP4725 FALLO");
  }
}
