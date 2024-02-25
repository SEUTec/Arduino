// Control d'un Convertidor DC-DC NQR010A0X4Z.
// amb VACTROL - LED Dependendent Resistor ( LED + LDR ).
// en este cas NSL-32R3 controlad per PWM.
// Amb realimentació de Vout amb un Divisor de Tensió.

// Al Serial Plotter vorem
// Vout - Blau
// Error - Roig
// Duty - Verd

#include <Arduino.h>

// Definició de Pins

// PWM Control LEDDR
#define LEDDR_pin 3
#define DC_DC_ON_OFF 2  // Pin 2 Arduino a l entrada ON-OFF del DC-DC.

// Entrades Analogiques
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
//float Vref = 3.28; // UNO
float Vref = 3.30; // 4Duino USB 3.300 V.

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
double X_Vout = 6 / 3.300;

// Convertir Duty Cycle en % a 0 a 255
float Duty = 0;  // Valor Inicial
int Duty_byte = Duty * 2.55;

// Configuració del control de Tensió del DC-DC
#define Volem_Vout 6 // Vout que volem
float Error_Ant = Volem_Vout;
float Error = Error_Ant;
float Kp = 0.5;     // Constant de Porporcionalitat

// Distribució de Pins per Controlar la polaritat de la tensió de sortida.
// Amb driver de motor L9110
//#define MKRZERO
#define UNO

#ifdef MKRZERO
int M1_DIR = 3;  // A1
int M1_PWM = 2;    // B1 - PWM pin.
int M2_PWM = 5;  // A2 - PWM pin.
int M2_DIR = 4;    // B2
#endif

#ifdef UNO
int M1_DIR = 4;  // A1
int M1_PWM = 5;    // B1 - PWM pin.
int M2_PWM = 6;  // A2 - PWM pin.
int M2_DIR = 7;    // B2
#endif

float ms=1;
float s=1000;
float Ton=0.1*s;   // Temps en ON [ms]
                  // Si fiquem un temps prou gran >> 2s podrem fer mesures amb multimetre.
float Toff=20*ms;  // Temps en OFF [ms]

void setup() {

  // Configuració de sortida PWM pal LED del VACTROL
  pinMode(DC_DC_ON_OFF, OUTPUT);
  pinMode(LEDDR_pin, OUTPUT);

  digitalWrite(DC_DC_ON_OFF, LOW);
  analogWrite(LEDDR_pin, Duty_byte );


  //analogReference(DEFAULT);  // DEFAULT, 5.0 V USB, 4.38 V 7 < Vin < 12, 
                             // Este valor pot cambiar en funcio de la connexió del port USB
                             // i del Arduino de turno
  analogReference(EXTERNAL);  // AREF, que pot ser la sortida 3.3V, o un chip de referencia de tensió
                              // que proporcioni una tensió precisa y constant entre 0 y 5V.
  //analogReference(INTERNAL);  // UNO: NTERNAL de 1.1V
                              // MEGA: INTERNAL1V1 de 1.1 o INTERNAL2V56 de 2.56V

  // Configuracio Driver del Motor per fer DC_AC.
//  pinMode(M1_PWM, OUTPUT);
//  pinMode(M1_DIR, OUTPUT);
//  digitalWrite(M1_PWM, LOW);
//  digitalWrite(M1_DIR, LOW);
  
  pinMode(M2_PWM, OUTPUT);
  pinMode(M2_DIR, OUTPUT);
  digitalWrite(M2_PWM, LOW);
  digitalWrite(M2_DIR, LOW);
    
  Serial.begin(9600);

  digitalWrite(DC_DC_ON_OFF, HIGH);
}

void loop() {
  int ADC_A0 = analogRead(A_VOUT);

  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - Vref):
  float Vout = X_Vout * ADC_A0 * Vref / (MaxBin-1);

  // Control de Tensió
  Error_Ant = Error;
  Error = Volem_Vout - Vout;

  Duty = Duty + Kp*Error;

  if ( Duty > 100 ){ Duty=100; }
  if ( Duty < 0 ){ Duty = 0;}

  Duty_byte = Duty * 2.55;    // Cambiar rang de 0-100 a 0-255 de les sortides PWM
  analogWrite(LEDDR_pin, Duty_byte );

// Utilitzar Delay aquí per controlar Ton i Toff impredeix que el
// del DC-DC funcione correctament, sobretot si els delay són de s.
// si son curts mira, pero amb s, ba que fa fastics.
  analogWrite(M2_PWM, 254);   // PWM Motor 100% 
//  digitalWrite(M2_PWM, HIGH);   // PWM Motor 100% 
  digitalWrite(M2_DIR, HIGH);  // Dirar en un serntido
  delay(Ton);

  digitalWrite(M2_PWM, LOW);   // PWM Motor 100% 
  digitalWrite(M2_DIR, HIGH);  // Dirar en un serntido
  delay(Toff);

  digitalWrite(M2_PWM, HIGH);   // PWM Motor 100% 
  digitalWrite(M2_DIR, LOW);  // Dirar en el otro serntido
  delay(Ton);

  digitalWrite(M2_PWM, LOW);   // PWM Motor 100% 
  digitalWrite(M2_DIR, LOW);  // Dirar en un serntido
  delay(Toff);

  // Enviar Datos al Monitor Serie
  Serial.print(" Vout: ");
  Serial.print(Vout);
  Serial.print("  Error: ");
  Serial.print(Error);
  Serial.print("  Duty [%]: ");
  Serial.println(Duty);
}


