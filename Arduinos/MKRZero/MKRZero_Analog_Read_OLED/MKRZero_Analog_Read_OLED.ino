/*
 * Test Sensor 
 *
 * Amb un ML8511, per obtenir UV Intensity en mW/cm2
 * Ref: https://learn.sparkfun.com/tutorials/ml8511-uv-sensor-hookup-guide
*/

/*
 * EL MKRZERO está en les plaques SAMD
 */

#define SERIE 1  // 1 per enviar dades port serie per debug,0 no.

#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

// Pins OLED
// Utilitzarems el SDA y SCL per defecte sense conectar la pantalla
// directament al Arduino per no perdre el pins SPI

/*
  U8glib Example Overview:
    Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
    Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
    U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.
    
*/

// Please UNCOMMENT one of the contructor lines below
// U8g2 Contructor List (Frame Buffer)
// The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
// Please update the pin numbers according to your setup. Use U8X8_PIN_NONE if the reset pin is not connected

// SEUTec
// WaveShare OLED 0.96
// Tenim d'utilitzar un pin de RESET per a que al alimentar a 3.3V la pantalla no es quedi negra.
// ZERO SPI
//U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 9, /* data=*/ 8, /* cs=*/ 10, /* dc=*/ 7, /* reset=*/ 6);
// UNO SPI
//U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
// UNO

// _HW_ Hardware I2C no cal especificar pins I2C, utilitzar los oficials del Arduino de Turno, SCL=12, SDA=11, MKRZero.
// No va be si tenim un altre dispositiu I2C, tindrem d utilitza Simulated Wire, SW.
// Per que no deixa especificar la I2C adress de la pantalla
//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
// _SW_ Simulated I2C, podem cambiar els pins de comunicacio I2C, per evitar problemes amb altres dispisitius I2c.
// Tot i que va molt més lento. de 76 ms a 600ms. 
//U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock, SCL=*/ A5, /* data,SDA =*/ A4, /* reset=*/ U8X8_PIN_NONE);

// SEUTec
// WaveShare OLED 1.3
// Tenim d'utilitzar un pin de RESET per a que al alimentar a 3.3V la pantalla no es quedi negra.
// ZERO
//U8G2_SH1106_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 9, /* data=*/ 8, /* cs=*/ 10, /* dc=*/ 7, /* reset=*/ 6);
// UNO
//U8G2_SH1106_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
// Si no podem utilitzar els pins 8, 9 i 10, per estar ocupats, podem utilitzar uns altres
//U8G2_SH1106_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 6, /* dc=*/ 5, /* reset=*/ 7);
// SEUTec

//U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8G2_SH1106_128X64_VCOMH0_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);		// same as the NONAME variant, but maximizes setContrast() range
//U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 21, /* data=*/ 20, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather M0 Basic Proto + FeatherWing OLED

// SEUTec
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather ESP8266/32u4 Boards + FeatherWing OLED
//U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL_OLED, /* data=*/ SDA_OLED, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather ESP8266/32u4 Boards + FeatherWing OLED
// SEUTec
// End of constructor list

#define ADC_IN A1  // Entrada Analògica que volem utilitzar

#define V_REF 3.3  // Tensió de referencia dels Analog to Digital Converter
#define Bits_ADC 12  // Si analogReadResolution(10) bits, per defecte
                     // 8, 10 o 12 per MKRZERO
const int Lect_Ref = pow(2, Bits_ADC)-1;

void setup(void) {
   // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  
  analogReadResolution(Bits_ADC);
  
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB10_tr);  // choose a suitable font
                                       // Una lletra més petita sescriu en menys temps.
}

unsigned int Lect_ADC = 0;
float Volts_ADC = 0;

void loop(void) {
  Lect_ADC = analogRead(ADC_IN);

  // Estavilització de Lectura
  int Num_Mostres = 20;
  float Lect_ProMig = float(Lect_ADC);

  for (int i=0; i<Num_Mostres; i++){
    Lect_ADC = float(analogRead(ADC_IN));
    Lect_ProMig = Lect_ProMig + Lect_ADC;
    //Serial.println("i: " + String(i));
  }
  Lect_ProMig = Lect_ProMig / Num_Mostres;
  
//  Volts_ADC = Lect_ADC * V_REF / Lect_Ref;  // Volts a partir de la Última Lectura 
  Volts_ADC = Lect_ADC * V_REF / Lect_Ref;  // Volts a partir de la Lectura ProMijada

  // float x, float in_min, float in_max, float out_min, float out_max
  float Vplc = mapfloat(Volts_ADC, 0, V_REF, 0.0, 10.0); // Conversió Voltatje ADC 0..3 Vdc a Voltatje PLC 0..10 Vdc
  float Per100 = mapfloat(Volts_ADC, 0, V_REF, 0.0, 100.0); // Conversió Voltatje a PerCentatje

// Enviar dades el port Series per Debug
#if SERIE  
  Serial.println("Lectura ADC: " + String(Lect_ADC));
  Serial.println("Volts ADC: " + String(Volts_ADC));
  Serial.print("Vout PLC: " + String(Vplc) + " V\n");
  Serial.print("Percentatje: " + String(Per100) + " %\n\n");
#endif

// Mostrar dades per la pantalla OLED
#define X_PXLS 128
#define Y_PXLS 32
#define NUM_LINS 2

#define ALT_LINEA Y_PXLS / NUM_LINS - 1
//#define ENTRE_LINEA 10
#define POS_NUMS X_PXLS/2    //Posició on posar el numeros
#define POS_UNITS X_PXLS-16

  u8g2.clearBuffer();					// clear the internal memory
  
  u8g2.drawStr(0,ALT_LINEA,"Vo PLC:");
  u8g2.setCursor(POS_NUMS, ALT_LINEA);
  u8g2.print(Vplc);
  u8g2.setCursor(POS_UNITS, ALT_LINEA);
  u8g2.print("V");
  
  //u8g2.setCursor(POS_NUMS, ALT_LINEA *2 + ENTRE_LINEA);
  u8g2.drawStr(0, 2 * ALT_LINEA,"Percnt:");
  u8g2.setCursor(POS_NUMS, 2 * ALT_LINEA);
  u8g2.print(Per100);   // Mostrar dato amb 0 decimals, sense utilitzar dtostrf.
                        // print(String(var,decimals)); no funciona amb Arduino Micro.

  u8g2.setCursor(POS_UNITS, 2 * ALT_LINEA);
  u8g2.print("%");
  
  u8g2.sendBuffer();					// transfer internal memory to the display
}

//The Arduino Map function but for floats
//From: http://forum.arduino.cc/index.php?topic=3922.0
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
