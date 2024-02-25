/* SuperVisor Dosificació
 *  
 * Compta el temps, t, que tarda en baixar el nivell d'un liquid desde un punt
 * determinar a un altre, amb un volum conegut, V, per calcular Caudal = V/t.
 * 
 * Tenim un parell de sensors de nivel capacitus.
 * El comptador de temp s'inicia quan el nivel del punt més alt deixa de detectar producte.
 * I deixa de comptar quan segón detector de nivell deixa de detectar producte.
 * 
*/

//#define SERIAL    // Comentar per NO enviar dades al Monitor Serie
#define OLED_091    // Comentar per NO usar OLED 0.91

#include <U8g2lib.h>          // OLED
//#include <Wire.h>             // I2C

// Pins Sensors de Nivell Capcitus
#define CCH_pn 2       // Pin Entrada sensor Columna Calibració Nivell Alt
#define CCL_pn 3     // Pin Entrada sensor Columna Calibració Nivell Baix

// Pins OLED
#define SDA_OLED 15
#define SCL_OLED 14
#define VCC_OLED 16
#define GND_OLED 10

//U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL_OLED, /* data=*/ SDA_OLED, /* reset=*/ U8X8_PIN_NONE);

#define ALT_LINEA 11
#define ENTRE_LINEA 10 

bool CCH_Ant = HIGH;
bool CCH = LOW;

bool CCL_Ant = HIGH;
bool CCL = HIGH;  

unsigned long t_ini;
unsigned long t_fi;
double temps;

#define DV 500   // Diferencial de Volum entre sensors en mL
//float DV = 500.0;

void setup(void){

  #ifdef SERIAL
    Serial.begin(115200);
/*    while (!Serial) {
      // Pause until serial console opens
      delay(1);
    }*/
    Serial.println("SuperVisor Dosificació.");
  #endif

  // Configuracio Alimentacio OLED I2C 128x32
  #ifdef OLED_091
  pinMode(GND_OLED, OUTPUT);
  digitalWrite(GND_OLED, LOW);
  pinMode(VCC_OLED, OUTPUT);
  digitalWrite(VCC_OLED, HIGH);

  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB10_tr);  // choose a suitable font
                                       // Una lletra més petita s escriu en menys temps.
  #endif

  // Configuració Entrades
  pinMode(CCH_pn, INPUT);
  pinMode(CCL_pn, INPUT);

}

void loop(void){

  CCH_Ant = CCH;
  CCH = digitalRead(CCH_pn);

  CCL_Ant = CCL;
  CCL = digitalRead(CCL_pn);
  
  #ifdef SERIAL
  Serial.print("CCH: "); Serial.println(CCH);
  Serial.print("CCL: "); Serial.println(CCL);
  #endif

  // CCH, CC comença a baixar de nivell, inici temporització
  if (CCH_Ant == HIGH & CCH == LOW){
    t_ini = millis(); // en milisegons 
  }

  // CCL, CC passa a nivell baix, fi temporització
  if (CCL_Ant == HIGH & CCL == LOW){
    t_fi = millis();  // en milisegons
    temps = (t_fi-t_ini) / 1000.0;  // dividint per 1000 temps en enter
                                    // dividint per 1000.0 temps es float
  }
  float Dosificacio = DV / temps;

  #ifdef SERIAL
  Serial.print("Temps: "); Serial.print(temps); Serial.println(" s");
  Serial.print("Dosificació: "); Serial.print(Dosificacio); Serial.println(" mL/s");
  Serial.println("");
  #endif
  
  // Mostar dades per pantalla
  #ifdef OLED_091
  u8g2.clearBuffer();          // clear the internal memory

  u8g2.setCursor(0,ALT_LINEA);
  u8g2.print(DV);
  u8g2.drawStr(30,ALT_LINEA,"mL");

  u8g2.setCursor(70,ALT_LINEA);
  u8g2.print(temps);
  u8g2.setCursor(110,ALT_LINEA);
  u8g2.print("s");

  u8g2.setCursor(60,ALT_LINEA*2 + ENTRE_LINEA);
  u8g2.print(int(Dosificacio));
  u8g2.setCursor(90,ALT_LINEA*2 + ENTRE_LINEA);
  u8g2.print("mL/s");

  u8g2.setCursor(0, ALT_LINEA*2 + ENTRE_LINEA);
  u8g2.print("      ");

  u8g2.sendBuffer();          // transfer internal memory to the display
  #endif
}
