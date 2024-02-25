// Control d'un Convertidor DC-DC NQR010A0X4Z o LM2596
// amb VACTROL - LED Dependendent Resistor ( LED + LDR ).
// en este cas NSL-32R3 controlad per PWM.
// Amb realimentació de Vout amb un Divisor de Tensió.

// Al Serial Plotter vorem
// Vout - Blau
// Error - Roig
// Duty - Verd

#include <Arduino.h>
#include <Wire.h>               // I2C
#include <Adafruit_MCP4725.h>   // DAC I2C

//#define NQR010A0X4Z
#define LM2596        
#define CLOSED_LOOP  // Comentar per obrir el llaç de realimentacio

// Definició de Pins

// DCDC Converter Control Pins
#define CONTROL_PIN 3
#define ON_OFF_PIN 2  // Pin 2 Arduino a l entrada ON-OFF del DC-DC.

// DAC
Adafruit_MCP4725 dac;
#define DAC_MAX 4095  // 12 bits DAC, 2**12=4096
uint32_t DAC_Data=DAC_MAX; // Inici data max per vout dc minima.

// Entrades Analogiques per Realimentació
#define A_VOUT A0      // Pin Analogic on Conectem la tensió de sortida DC-DC
#define Bits_ADC 10    // Resolucio 10 Bits dels ADC Arduino UNO
                       // Resolucio 12 Bits dels ADC Arduino Due y Zero
                       // Resolucions 8/10/12 Bits MKRZERO.
const int MaxBin=pow(2,Bits_ADC);   // Final d'escala Lectura Digital

// analogReference(DEFAULT) 5V en sistemes de 5V, 3.3 en sistemes de 3.3V
//float Vref = 5.0;  // UNO connectad al port USB
//float Vref = 4.38; // UNO connectad connector alimentació 7-12V
//float Vref = 1.1; // analogReference(INTERNAL)

// analogReference(EXTERNAL) Conectar pin AREF amb 3.3V..
float Vref = 3.28; // UNO
//float Vref = 3.30; // 4Duino USB 3.300 V.

// Resistencies del divisor de tensió de sortida.
// Que pot ser un potenciometre multivolta de 20k
// que ajustarem per obtener Vref volts a A0 quan
// Vo está en el valor máxim al que pot arrivar.

// També podem definir a partir de mesures de tensió realitzades
// com X_Vout=Vo/Vodiv on Vo será Vomax y Vodiv 
// per obtenir la máxima resolució de la conversió a digital
// podem definir X_Vout = Vo_max / Vref;
// Tenint en compte que tindrem l'ajustar el potenciometre multivolta
// de fa la divisió de manera que amb Vo igual al valor máxim al que pot
// arrivar la tensió del cursor sigue igual a Vref.

#ifdef NQR010A0X4Z
  double X_Vout = 6 / Vref;  // NQR010A0X4Z amb del divisor ajustad convenientment
#endif

#ifdef LM2596
  double X_Vout = 32 / Vref;  // LM2596 amb del divisor ajustad convenientment
#endif

// Convertir Duty Cycle en % a 0 a 255
//float Duty = 0;  // Valor Inicial NQR010A0X4Z
//float Duty = 0;  // Valor Inicial LM2596
//int Duty_byte = Duty * 2.55;

// Configuració del control
#define Volem_Vout 26 // Vout que volem 0-6 V pal NQR010A0X4Z
                     // 2.5 a 30 V LM2596
float Error_Ant = Volem_Vout;
float Error = Error_Ant;
//float Kp = 0.5;     // Constant de Porporcionalitat, NQR010A0X4Z
float Kp = 1;     // Constant de Porporcionalitat, LM2596
float Kp_dac = 10;

void setup() {

  // Configuració de sortida PWM pal LED del VACTROL
//  pinMode(ON_OFF_PIN, OUTPUT);
//  pinMode(CONTROL_PIN, OUTPUT);

//  digitalWrite(ON_OFF_PIN, LOW);
//  analogWrite(CONTROL_PIN, Duty_byte );

  // For Adafruit MCP4725A1 the address is 0x62 (default) or 0x63 (ADDR pin tied to VCC)
  // For MCP4725A0 the address is 0x60 or 0x61
  // For MCP4725A2 the address is 0x64 or 0x65
  // Els meus me donen amb A0 en Circuit Obert o GND 0x62. A0 a VCC 0x63 => Son MCP4725A1.
  dac.begin(0x62);
    
  //analogReference(DEFAULT);  // DEFAULT, 5.0 V USB, 4.38 V 7 < Vin < 12, 
                             // Este valor pot cambiar en funcio de la connexió del port USB
                             // i del Arduino de turno
  analogReference(EXTERNAL);  // AREF, que pot ser la sortida 3.3V, o un chip de referencia de tensió
                              // que proporcioni una tensió precisa y constant entre 0 y 5V.
  //analogReference(INTERNAL);  // UNO: NTERNAL de 1.1V
                              // MEGA: INTERNAL1V1 de 1.1 o INTERNAL2V56 de 2.56V
  Serial.begin(9600);
  Serial.print("DCDC amb PWM to Analog Converter,");

//  digitalWrite(ON_OFF_PIN, HIGH);
}

void loop() {

  int ADC_A0 = analogRead(A_VOUT);

  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - Vref):
  float Vout = X_Vout * ADC_A0 * Vref / (MaxBin-1);

  // Control de Tensió
  Error_Ant = Error;
  Error = Volem_Vout - Vout;

#ifdef CLOSED_LOOP
  //Duty = Duty + Kp*Error; // NQR010A0X4Z
//  Duty = Duty - Kp*Error;   // LM2596

//  if ( Duty > 100 ){ Duty=100; }
//  if ( Duty < 0 ){ Duty = 0;}

   DAC_Data = DAC_Data - Kp_dac * Error;

   if (DAC_Data > DAC_MAX ) { 
     DAC_Data = DAC_MAX;
   }  // Per evitar que la tensió baixe per passarse.
   if (DAC_Data < 0 ) {    // Per evitar 5 V quan s'intiquen tensions negatives
     DAC_Data = 0;         // No cal si DAC_Data es un uint32_t, enter sense signe.
   }
#endif

//  Duty_byte = Duty * 2.55;    // Cambiar rang de 0-100 a 0-255 de les sortides PWM
//  analogWrite(CONTROL_PIN, Duty_byte );

  dac.setVoltage(DAC_Data, false);   // false pa que no guarde el valor a la EEPROM

  // Enviar Datos
  Serial.print(" Vout: ");
  Serial.print(Vout);
  Serial.print("  Error: ");
  Serial.print(Error);
  //Serial.print("  Duty [%]: ");
  //Serial.print(Duty);
  Serial.print(" DAC_Data ");
  Serial.print(DAC_Data);
  Serial.println();
}


