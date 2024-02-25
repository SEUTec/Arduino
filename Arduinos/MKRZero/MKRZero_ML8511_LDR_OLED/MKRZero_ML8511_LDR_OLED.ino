/*
 * Test Sensor 
 *
 * Amb un ML8511, per obtenir UV Intensity en mW/cm2
 * Ref: https://learn.sparkfun.com/tutorials/ml8511-uv-sensor-hookup-guide
*/

/*
 * EL MKRZERO está en les plaques SAMD
 * SAMD21 Cortex-M0+ 32bit low power ARM MCU
 */

#include <Arduino.h>
#include <U8g2lib.h>

#define SERIE 0  // 1 per enviar dades port serie per debug,0 no.

// Pins ML8511 amb 4 entrades Analogiques
#define UV_VCC A4
#define UV_GND A3
#define UV_OUT A2
#define UV_EN  A1

unsigned int Lect_UV = 0;
float Volts_UV = 0;

#define V_REF 3.3
#define Bits_ADC 12  // Si analogReadResolution(10) bits, per defecte
                     // 8, 10 o 12 per MKRZERO
const int Lect_Ref = pow(2, Bits_ADC)-1;

//Pins LDR
#define LDR_VCC A5
#define LDR_OUT A6
#define LDR_GND 0

unsigned int Lect_LDR = 0;
float Volts_LDR = 0;

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

// SEUTec OLED 0.91 128x32 I2C.
// MKRZERO, Adafruit Feather ESP8266/32u4 Boards, FeatherWing OLED
//U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
// Adafruit Feather ESP8266/32u4 Boards + FeatherWing OLED
//U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL_OLED, /* data=*/ SDA_OLED, /* reset=*/ U8X8_PIN_NONE);
// Adafruit ESP8266/32u4/ARM Boards + FeatherWing OLED
//U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// SEUTec OLED 0.96 128x64 I2C,
// _HW_ Hardware I2C no cal especificar pins I2C, utilitzar los oficials del Arduino de Turno, SCL=12, SDA=11, MKRZero.
// No va be si tenim un altre dispositiu I2C, tindrem d utilitza Simulated Wire, SW.
// Per que no deixa especificar la I2C adress de la pantalla
//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);  // No Rotation
//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R1, /* reset=*/ U8X8_PIN_NONE);  // 90º Clockwise
//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R2, /* reset=*/ U8X8_PIN_NONE);  // 180º Clockwise
//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R3, /* reset=*/ U8X8_PIN_NONE);  // 270º Clockwise
// _SW_ Simulated I2C, podem cambiar els pins de comunicacio I2C, per evitar problemes amb altres dispisitius I2c.
// Tot i que va molt més lento. de 76 ms a 600ms. 
//U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock, SCL=*/ A5, /* data,SDA =*/ A4, /* reset=*/ U8X8_PIN_NONE);

// SEUTec OLED 0.96 128x64 SPI
// Tenim d'utilitzar un pin de RESET per a que al alimentar a 3.3V la pantalla no es quedi negra.
// ZERO SPI
//U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 9, /* data=*/ 8, /* cs=*/ 10, /* dc=*/ 7, /* reset=*/ 6);
// UNO SPI
//U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
// UNO

//U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 12, /* dc=*/ 4, /* reset=*/ 6);	// Arduboy (Production, Kickstarter Edition)
//U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8G2_SSD1306_128X64_NONAME_F_3W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* reset=*/ 8);
//U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* reset=*/ 8);
//U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display
//U8G2_SSD1306_128X64_NONAME_F_6800 u8g2(U8G2_R0, 13, 11, 2, 3, 4, 5, 6, A4, /*enable=*/ 7, /*cs=*/ 10, /*dc=*/ 9, /*reset=*/ 8);
//U8G2_SSD1306_128X64_NONAME_F_8080 u8g2(U8G2_R0, 13, 11, 2, 3, 4, 5, 6, A4, /*enable=*/ 7, /*cs=*/ 10, /*dc=*/ 9, /*reset=*/ 8);
//U8G2_SSD1306_128X64_VCOMH0_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);	// same as the NONAME variant, but maximizes setContrast() range

// SEUTec OLED 1.3 128x64 SPI
// Tenim d'utilitzar un pin de RESET per a que al alimentar a 3.3V la pantalla no es quedi negra.
// ZERO
//U8G2_SH1106_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 9, /* data=*/ 8, /* cs=*/ 10, /* dc=*/ 7, /* reset=*/ 6);
// UNO
//U8G2_SH1106_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
// Si no podem utilitzar els pins 8, 9 i 10, per estar ocupats, podem utilitzar uns altres
//U8G2_SH1106_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 6, /* dc=*/ 5, /* reset=*/ 7);
// MKRZERO Sense Rotacio
//U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 7, /* dc=*/ 6, /* reset=*/ -1);
// Rotat 180º
U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R2, /* cs=*/ 7, /* dc=*/ 6, /* reset=*/ -1);
// SEUTec END

// End of constructor list
// Més exemples de configuracions als exemples de la llibraria.

void setup(void) {
   // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  
  // Configuracio Pins ML8511 per alimentarlo a traves del pins Analogics
  pinMode(UV_VCC, OUTPUT);
  digitalWrite(UV_VCC, HIGH);
  pinMode(UV_GND, OUTPUT);
  digitalWrite(UV_GND, LOW);
  pinMode(UV_OUT, INPUT);
  pinMode(UV_EN, OUTPUT);
  digitalWrite(UV_EN, HIGH);

  pinMode(LDR_VCC, OUTPUT);
  digitalWrite(LDR_VCC, HIGH);
  pinMode(LDR_OUT, INPUT);
  pinMode(LDR_GND, OUTPUT);
  digitalWrite(LDR_GND, LOW);

  analogReadResolution(Bits_ADC);
  
  u8g2.begin();
}

void loop(void) {
  // Lectura dades sensor UV
  Lect_UV = analogRead(UV_OUT);
  Volts_UV = Lect_UV * V_REF / Lect_Ref;
  float uvIntensity = mapfloat(Volts_UV, 0.99, V_REF, 0.0, 15.0); //Convert the voltage to a UV intensity level

  // Lectures dades sensor LDR
  Lect_LDR = analogRead(LDR_OUT);
  Volts_LDR = Lect_LDR * V_REF / Lect_Ref;

// Enviar dades el port Series per Debug
#if SERIE  
  Serial.println("Lectura ADC: " + String(Lect_UV));
  Serial.println("Volts ADC: " + String(Volts_UV));
  Serial.print("UV Intensity: " + String(uvIntensity) + " mW/cm2\n\n");
#endif

// Mostrar dades per la pantalla OLED 0.91 128x32, es petita pa mostrar més de 2 dades.
// Mostrar dades per la pantalla OLED 0.96 128x64, es petita pa mostrar més de 4 dades.
#define X_PXLS 128
#define Y_PXLS 64
#define NUM_LINS 5

//  u8g2.setFont(u8g2_font_ncenB10_tr);  // 4 Linies 128x64, 5 Linies molt juntes
  u8g2.setFont(u8g2_font_ncenB08_tr);  // 5 Linies 128x64.

#define ALT_LINEA Y_PXLS / NUM_LINS - 1
//#define ENTRE_LINEA 10
#define POS_NUMS X_PXLS / 3 - 5   //Posició on posar el numeros
#define POS_UNITS X_PXLS * 2 / 3 - 10    //Posició de les unitats a la dreta
#define POS_MAXS X_PXLS * 2 / 3    //Posició dels Maxims

  u8g2.clearBuffer();					// clear the internal memory

  int LineaActual = 1;
  u8g2.drawStr(0, LineaActual * ALT_LINEA,"UV");
  u8g2.setCursor(POS_NUMS, LineaActual * ALT_LINEA);
  u8g2.print(uvIntensity);
  u8g2.drawStr(POS_UNITS, LineaActual * ALT_LINEA,"mW/cm2");

  LineaActual++;
  u8g2.drawStr(0,LineaActual * ALT_LINEA,"Ta");
  u8g2.setCursor(POS_NUMS, LineaActual * ALT_LINEA);
  u8g2.print(Volts_LDR);   // Mostrar dato amb 0 decimals, sense utilitzar dtostrf.
  u8g2.setCursor(POS_MAXS, LineaActual * ALT_LINEA);
  u8g2.drawStr(POS_UNITS, LineaActual * ALT_LINEA,"oC");

  LineaActual++;
  u8g2.drawStr(0,LineaActual * ALT_LINEA,"mV");
  u8g2.setCursor(POS_NUMS, LineaActual * ALT_LINEA);
  u8g2.print(Volts_LDR);   // Mostrar dato amb 0 decimals, sense utilitzar dtostrf.
  u8g2.setCursor(POS_MAXS, LineaActual * ALT_LINEA);
  u8g2.print(Volts_LDR);   // Mostrar dato amb 0 decimals, sense utilitzar dtostrf.
  
  LineaActual++;
  u8g2.drawStr(0,LineaActual * ALT_LINEA,"mA");
  u8g2.setCursor(POS_NUMS, LineaActual * ALT_LINEA);
  u8g2.print(Volts_LDR);   // Mostrar dato amb 0 decimals, sense utilitzar dtostrf.
  u8g2.setCursor(POS_MAXS, LineaActual * ALT_LINEA);
  u8g2.print(Volts_LDR);   // Mostrar dato amb 0 decimals, sense utilitzar dtostrf.

  LineaActual++;
  u8g2.drawStr(0,LineaActual * ALT_LINEA,"mW");
  u8g2.setCursor(POS_NUMS, LineaActual * ALT_LINEA);
  u8g2.print(Volts_LDR);   // Mostrar dato amb 0 decimals, sense utilitzar dtostrf.
  u8g2.setCursor(POS_MAXS, LineaActual * ALT_LINEA);
  u8g2.print(Volts_LDR);   // Mostrar dato amb 0 decimals, sense utilitzar dtostrf.
  
  u8g2.sendBuffer();					// transfer internal memory to the display
}

//The Arduino Map function but for floats
//From: http://forum.arduino.cc/index.php?topic=3922.0
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
