// Pantalla principal de mostra de dades.

void lcd_dosi(){

//  lcd.clear();    // Si fem clear i reescribim tot les línies inferiors
                    // parpadejen més que les superiors.
                    // Millor borrar i escriure línea a línea,
                    // delay(1000) (ms) per reduir parpadeix.

  int LINEA = 0;
  lcd.setCursor(0, LINEA);
  lcd.print("                    ");
  lcd.setCursor(0, LINEA);
  lcd.print("DOSIFICACIO");

  lcd.setCursor(16, LINEA);
//  lcd.print(BOMBA_DOSI, 0);
  lcd.print(Bomba_Dosi, 0);
  lcd.print(" %");
//  lcd.print(lecPotBomba);   // Lectura Entrada Potenciòmetre Bomba
//  lcd.print(DAC_Data);      // Data que s'ecriu al DAC, Sortida Analògica Bomba Dosificadora

  LINEA = 1;  
  lcd.setCursor(0, LINEA);
  lcd.print("                    ");
  lcd.setCursor(0, LINEA);
  lcd.print("T:   ");
//  lcd.print(T_ms, 0);
//  lcd.print(" ms");
  lcd.print(float(T_ms) / 1000, 0);
  lcd.print(" s");

  lcd.setCursor(10, LINEA);
  lcd.print("Duty: ");
  lcd.print(Duty, 0);
  lcd.print(" %");

  
  LINEA = 2;
  lcd.setCursor(0, LINEA);
  lcd.print("                    ");
  lcd.setCursor(0, LINEA);
  lcd.print("Ton: ");
//  lcd.print(Ton_ms);
//  lcd.print(" ms ");
  lcd.print(float(Ton_ms) / 1000, 0);
  lcd.print(" s ");
 
  lcd.setCursor(10, LINEA);
  lcd.print("Toff: ");
//  lcd.print(Toff_ms);
//  lcd.print(" ms");
  lcd.print(float(Toff_ms) / 1000, 0);
  lcd.print(" s");

  LINEA = 3;
  lcd.setCursor(0, LINEA);
  lcd.print("                    ");
  lcd.setCursor(0, LINEA);
  lcd.print("ESTAT: ");
  if ( Fallo_DAC ){
    lcd.print("FALLO  DAC");
  }
  else {
    if( EstatBomba ){
      //lcd.print("     DOSIFICANT     ");    
      lcd.print("DOSIFICANT");    
    }
    else {
      lcd.print("PARADA");    
    }
  }
}  
