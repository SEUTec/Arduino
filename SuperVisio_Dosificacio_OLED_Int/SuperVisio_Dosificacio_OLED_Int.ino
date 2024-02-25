/* SuperVisor Dosificació
 *  
 * Compta el temps, t, que tarda en baixar el nivell d'un liquid desde un punt
 * determinar a un altre, amb un volum conegut, V, per calcular Caudal = V/t.
 * 
 * Tenim un parell de sensors de nivel capacitus.
 * El comptador de temp s'inicia quan el nivel del punt més alt deixa de detectar producte.
 * I deixa de comptar quan segón detector de nivell deixa de detectar producte.
 * 
 * Versió que funciona amb Interrupcions
 * 
*/

//#define SERIAL    // Comentar per NO enviar dades al Monitor Serie
#define OLED_091    // Comentar per NO usar OLED 0.91

#include <U8g2lib.h>          // OLED
//#include <Wire.h>             // I2C

// Pins Sensors de Nivell Capcitus
// Tenen de ser pins utilitzables per interrupcions, 0,1,2,3 o 7 pal ProMicro
// Que coincideixen amb SDA y SCL per lo que no podrem d'utilitzat
// una altres pins per la pantalla I2C.
#define CCHpn 2     // Pin Entrada sensor Columna Calibració Nivell Alt
#define CCLpn 3     // Pin Entrada sensor Columna Calibració Nivell Baix

// Pins OLED
#define SDA_OLED 15
#define SCL_OLED 14
#define VCC_OLED 16
#define GND_OLED 10

//U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL_OLED, /* data=*/ SDA_OLED, /* reset=*/ U8X8_PIN_NONE);

#define ALT_LINEA 11
#define ENTRE_LINEA 10 

volatile unsigned long t_ini = 0;   // volatile, perque es modificarà dintre de ISR
unsigned long t_ini_ant = 0;        // per detectar si ha cambiat
volatile unsigned long t_fi = 0;
unsigned long t_fi_ant = 0;         // per detectar si ha cambiat
volatile double temps = 0;
float Dosificacio = 0;

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
  //pinMode(CCHpn, INPUT);
  //pinMode(CCLpn, INPUT);

  // Configuració Interrupcions
  // digitalPinToInterrupt, retorna el numero d'interrupció corresponent al pin utilitzat.
  // ISR, Interrupt Service Routin, Nom de funció que serveix a la interrupció
  // modo, que pot ser: LOW, RISING, CHANGE, FALLING
  // modo també pot ser HIGH, per Arduinos Due, ZERO y MKR1000
  attachInterrupt(digitalPinToInterrupt(CCHpn), ISR_CCH, FALLING);
  attachInterrupt(digitalPinToInterrupt(CCLpn), ISR_CCL, FALLING);
}

// ISR per quan el sensor Superior de Col. de Calibració passa deixa de detectar nivell.
// ultima_Int i t_Int s'utilitzen per filtrar revots en la senyal de la interrupció.
// pero en este cas funcionaba prou bé.
void ISR_CCH(){
  static unsigned int ultim_Int = 0;
  unsigned long t_Int = millis();

  if (t_Int - ultim_Int > 5 ){
    t_ini = t_Int;    // en milisegons, esta funcio deixa de comptar mentre
                      // s'executa una ISR, pero no hi ha problema. 
  }
  ultim_Int = t_Int;
}

// ISR per quan el sensor Inferior de Col. de Calibració passa deixa de detectar nivell.
void ISR_CCL(){
  static unsigned int ultim_Int = 0;
  unsigned long t_Int = millis();

  if (t_Int - ultim_Int > 5){
    t_fi = t_Int;  
  }
  
  ultim_Int = t_Int;
}

void loop(void){
 
#ifdef SERIAL
  if ( t_ini_ant != t_ini ){ Serial.print("t_ini: "); Serial.println(t_ini); }
  if ( t_fi_ant != t_fi ) { Serial.print("t_fi: "); Serial.println(t_fi); }
#endif

  // CCL, CC passa a nivell baix, fi temporització
  if ( t_ini != t_ini_ant and t_fi != t_fi_ant and t_fi > t_ini){
    temps = (t_fi-t_ini) / 1000.0;  // dividint per 1000 temps en enter
                                    // dividint per 1000.0 temps es float
    Dosificacio = DV / temps;
    t_ini_ant = t_ini;                                    
    t_fi_ant = t_fi;
  }

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
