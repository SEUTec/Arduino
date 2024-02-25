/* High Speed PWM Using TCC Timer/Counters

On the Arduino with Cortex-M0 (ATSAMD21) there is more choice in frequency and speed for your
  PWM control than just the AnalogWrite() functions.

Why not use the setPwmFrequency() function : 
  this changes the Clock pre-scaler, possibly influencing timing of other Arduino functions
  for time.

The difficulty is to figure out the right register settings, using the datasheet, 
  Chapter 31 - TCC timers/counters, specific 31.6.2 for setting up the PWM counters - using 
  GCLKC4 (not used by arduino) and connecting them to the right TCCs, and to the right IO pin.

Code is self explaining, and using the dual-slope PWM mode to create glitch-free PWM signals 
  for low (several Hz) and high speed (>>500Khz).

MKR1000's pins than can be defined as TCC timer pins and their associated channel (WO[X]) 
unless stated otherwise the TCC timers are on peripheral F:

A0 - PA02 - None
A1 - PB02 - None
A2 - PB03 - None
A3 - PA04 - TCC0/WO[0] (same channel as TCC0/WO[4])
A4 - PA05 - TCC0/WO[1] (same channel as TCC0/WO[5])
A5 - PA06 - TCC1/WO[0]
A6 - PA07 - TCC1/WO[1]
D0 - PA22 - TCC0/WO[4] (same channel as TCC0/WO[0])
D1 - PA23 - TCC0/WO[5] (same channel as TCC0/WO[1])
D2 - PA10 - TCC1/WO[0]
*D3 - PA11 - TCC1/WO[1]
D4 - PB10 - TCC0/WO[4] (same channel as TCC0/WO[0])
D5 - PB11 - TCC0/WO[5] (same channel as TCC0/WO[1])
D6 - PA20 - TCC0/WO[6] (same channel as TCC0/WO[2])
D7 - PA21 - TCC0/WO[7] (same channel as TCC0/WO[3])
D8 - PA16 - TCC0/WO[6] (same channel as TCC0/WO[2]) on peripheral F, TCC2/WO[0] on peripheral E
D9 - PA17 - TCC0/WO[7] (same channel as TCC0/WO[3]) on peripheral F, TCC2/WO[1] on peripheral E
D10 - PA19 - TCCO/WO[3] (same channel as TCC0/WO[7])
*D11 - PA08 - TCC1/WO[2] (same channel as TCC1/WO[0]) on peripheral F, TCC0/WO[0] on peripheral E
D12 - PA09 - TCC1/WO[3] (same channel as TCC1/WO[1]) on peripheral F, TCC0/WO[1] on peripheral E
D13 - PB22 - None
D14 - PB23 - None

Note the timer TCC0 has only 4 channels (0-3 and 4-7 are the same), 
while TCC1 and TCC2 each have 2, giving you 8 channels in total.

*/

// Al pin 3 el MKRZero Surt un PWM amb Duty cambiant.
// Al pin 11 hi ha un altre que en pot config amb un Duty diferent, pero a la mateixa Freq
// Si es modifica este programa.

// Ajust amb Potenciometres, mante la configuració al reiniciar si no es toquen el pots.
// Ajust amb Encoders permet reajustar automaticament uns parametres al modificar un.

// TO DO:
// Mostar Activació de Fault y tipo.
// Seleccio entre Pulsos +, Pulsos -, AC.

/*
 * PWM, Pulse Width Modulation, en Duty y Frequencia.
 */

// Sortides Digitals
#define PWM_OUT0_PIN 3
#define PWM_OUT1_PIN 11

// Entrades Analògiques
#define DUTY_PIN A3
float Duty = 0;

#define FREQ_PIN A2
float Freq = 0;

// Inductancies Petites de 5 a 50 mH
// 10 mH, 2KHz
#define FREQ_MAX 10e3   // Per obtenir bona conmutacio amb el Pololu756 millor no passar de 50 kHz
                        // Pero pot conmmutar relaticament be fins a 150kHz
#define FREQ_MIN 1e3

// Inducancies grans de 50 a 1000 mH
//#define FREQ_MAX 1e3   // Per obtenir bona conmutacio amb el Pololu756 millor no passar de 50 kHz
                        // Pero pot conmmutar relaticament be fins a 150kHz
//#define FREQ_MIN 50

float T = 0;
float Ton = 0;
float Toff = 0;

#define CPU_CLCK 48000000   // MKRZero va a 48 Mhz
int NewPer = 0;   // Config Periode Proporcional a la Freq
int NewReg = 0;   // Config Ton proporcional a la Freq y la Duty

/*
 * LCD. Liquid Crystal Display
 */

// include the library code:
#include <LiquidCrystal.h>

/*
LiquidCrystal(rs, enable, d4, d5, d6, d7)
LiquidCrystal(rs, rw, enable, d4, d5, d6, d7)
LiquidCrystal(rs, enable, d0, d1, d2, d3, d4, d5, d6, d7)
LiquidCrystal(rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7)
*/
LiquidCrystal lcd(8, 2, 4, 5, 6, 7);           // select the pins used on the LCD panel
int LCD_Updt = 0;     
#define LCD_UPDATE_CCLS 20  // Per millorar visibilitat de les dades, reduim freq actualitzacio.
#define LCD_POS_COL1 0     // Posicio Cursor LCD on escriure els valors
#define LCD_POS_COL2 6
#define LCD_POS_COL3 10

int m = 1e3;
int u = 1e6;

/*
 * Driver Pololu756
 */

// Entrades Digitals.
// Fault Pins
#define FF1_pin 10
#define FF2_pin 9

// Entrada Analogica
#define V_REF 3.29
// Bits=10;    // Bits del conversor AD.
// Max_Lec = pow(2,Bits) - 1;   // Final d'escala en Binari
#define MAX_LEC 1023
#define Vp_pin A1
float Vp = 0;
#define VP_MULT 11  // Factor multiplicació de mesura per compensar el Divisor de tensió
                    // necesari per a que la tensió aplicada a la entrada del Arduino
                    // no supere el 3.3, quan Vp es de 33V.
bool Vp_nvll = LOW;  // Indicador de Vpower driver massa bauxa, a menos de 5V el DRV no funciona.
#define VP_MIN 4.5
#define VP_MAX 30   // La máxima Vpower del DRV es de 44 o 50V.

// Sortides Digitals
//#define RESET_pin 2   // Millor que sigue manual, 
#define DIR_pin 12

int FF1 = 0;
int FF2 = 0;
bool Fault = LOW;

void setup() {

// Entrades Analogiques;
//  analogReadResolution(8); //set the ADC resolution to match the PWM max resolution (0 to 255)
// Sortides Digitals PWM
  pinMode(PWM_OUT0_PIN, OUTPUT);
  digitalWrite(PWM_OUT0_PIN, LOW);
  pinMode(PWM_OUT1_PIN, OUTPUT);
  digitalWrite(PWM_OUT1_PIN, LOW);
  
// Entrades Digitals Driver Pololu756
  pinMode(FF1_pin, INPUT);
  pinMode(FF2_pin, INPUT);

// Sortides Digital al Driver Pololu756
  pinMode(DIR_pin, OUTPUT);
  digitalWrite(DIR_pin, HIGH);  // Forward, Vout 0 a Vpower,
                                // Al intercambiar el PWM i el DIR podem obtener una tansió quadrada AC.
                                // Llavors este pin DIR actua com PWM, ha ha d'estar en HIGH, LOW sería BRAKE.
                                // Al pin DIR aplicarem la señal PWM, que en HIGH dona -Vpower.
                                // y en LOW donarà -Vpower a la carga del Driver..

  pinMode(DUTY_PIN, INPUT);
    // Ajustem escala de lectura de 0-1023 de les entrades a 0-1
  Duty = (float)analogRead(DUTY_PIN) / MAX_LEC;  // La lectura de la mostra en un enter que
                                              // s ha de convertir a float per poder dividir

  pinMode(FREQ_PIN, INPUT);
    // Ajustem escala de lectura de 0-1023 de les entrades a 0-1
  Freq = (float)analogRead(FREQ_PIN) / MAX_LEC;  // La lectura de la mostra en un enter que
                                              // s ha de convertir a float per poder dividir
  Freq = Freq * (FREQ_MAX - FREQ_MIN) + FREQ_MIN;

  T = 1 / Freq;
  Ton = Duty * T;
  Toff = T - Ton;

  NewPer = CPU_CLCK / 2 * T;
  NewReg = Duty*NewPer;

  setupTimers();

// Control V+
  pinMode(Vp_pin, INPUT);

// set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  Serial.begin(9600);
}

void loop() {

  // Control V+
  pinMode(Vp_pin, INPUT);
  Vp = V_REF * (float)analogRead(Vp_pin) / MAX_LEC * VP_MULT;
  Serial.println("Vp: " + String(Vp));

  if (Vp < VP_MIN){
    Vp_nvll = LOW;
  }
  else {
    Vp_nvll = HIGH;
  }

  //Control Alarmes Driver
  FF1=digitalRead(FF1_pin);
  FF2=digitalRead(FF2_pin);

  Serial.println("FF1: " + String(FF1) + " FF2: " + String(FF2));

  if (FF1 && FF2){
    Fault=HIGH;
    Serial.println("  Fault: Under Voltage.");
  }
  else if (FF1 && !FF2) {
    Fault=HIGH;
    Serial.println("  Fault: Over Temperature.");
  }
  else if (!FF1 && FF2) {
    Fault=HIGH;
    Serial.println("  Fault: Short Circuit.");
  }
  else if (!FF1 && !FF2) {
    Fault=LOW;
    //Serial.println("  No Fault. ");
  }

// Lectura entrades Analogiques
  Duty = (float)analogRead(DUTY_PIN) / 1023;  // La lectura de la mostra en un enter que
                                              // s ha de convertir a float per poder dividir

    // Ajustem escala de lectura de 0-1023 de les entrades a 0-1
  Freq = (float)analogRead(FREQ_PIN) / 1023;  // La lectura de la mostra en un enter que
                                              // s ha de convertir a float per poder dividir
  Freq = Freq * FREQ_MAX;
  if (Freq < FREQ_MIN){
    Freq = FREQ_MIN;
  }

  T = 1 / Freq;
  Ton = Duty * T;
  Toff = T - Ton;

  lcd.setCursor(LCD_POS_COL1,0);
  lcd.print("      ");// Borrar dato anterior
  lcd.setCursor(LCD_POS_COL1,0);
  lcd.print((int)Freq);

  lcd.setCursor(LCD_POS_COL2,0);
  lcd.print("      ");// Borrar dato anterior
  lcd.setCursor(LCD_POS_COL2,0);
  lcd.print(int(Duty*100));

  lcd.setCursor(LCD_POS_COL3,0);
  lcd.print("      ");// Borrar dato anterior
  lcd.setCursor(LCD_POS_COL3,0);
  lcd.print(Vp);

  lcd.setCursor(LCD_POS_COL1,1);
  lcd.print("      ");// Borrar dato anterior
  lcd.setCursor(LCD_POS_COL1,1);
  lcd.print(int(Ton*u));   // Ton en MicroSegons

  lcd.setCursor(LCD_POS_COL2,1);
  lcd.print("      ");// Borrar dato anterior
  lcd.setCursor(LCD_POS_COL2,1);
  lcd.print(int(Toff*u));  // Toff en MicroSegons

  lcd.setCursor(LCD_POS_COL3,1);
  lcd.print("      ");// Borrar dato anterior
  lcd.setCursor(LCD_POS_COL3,1);
  if (Vp_nvll){ 
    if (Vp < VP_MAX) { lcd.print("V+ OK"); }
    else { lcd.print("V+ HGH"); }
  }
  else { lcd.print("V+ LW"); }
  
  Serial.println("Freq:" + String(Freq) + " Hz");
  Serial.println("Duty: " + String(Duty));
  Serial.println("T:    " + String(T*u) + " us");
  Serial.println("Ton:  " + String(Ton*u) + " us");
  Serial.println("Toff: " + String(Toff*u) + " us");
  Serial.println(" ");

  // Modificant REG_TCC1_PER asjustarem el periode y per tant la frequencia del PWM
  // Each timer counts up to a maximum or TOP value set by the PER register,
  // this determines the frequency of the PWM operation: Freq = 48Mhz/(2*N*PER) ?? N ??
  // Freq = 48e6/1000/2 = 24 kHz
  // PER = 48e6/Freq/2
  NewPer = CPU_CLCK / 2 * T;
  if (NewPer != REG_TCC1_PER){
    REG_TCC1_PER = NewPer;                           // Set the FreqTcc of the PWM on TCC1 to 24Khz
    while (TCC1->SYNCBUSY.bit.PER);                // Wait for synchronization
  }
  //Serial.println("NewPer: " + String(NewPer));

  // Despres de modificar la freq tindrem d'ajustar el Ton per mantenir el Duty

  // Modificant REG_TCC1_CC0 cambiarem el duty de D13.
  NewReg = Duty*NewPer;
  if (NewReg != REG_TCC1_CC1){
    REG_TCC1_CC1 = NewReg;                              // TCC1 CC1 - on D3  - PWM signalling
    while (TCC1->SYNCBUSY.bit.CC1);                // Wait for synchronization
    //Serial.println("Updadte NewReg.");
  }
  //Serial.println("NewReg: " + String((int)NewReg));

  // Modificant REG_TCC1_CC0 cambiarem el duty de D11.
  if (NewReg != REG_TCC1_CC0){
    REG_TCC1_CC0 = NewReg;         // TCC1 CC1 - on D3  - PWM signalling, 0 to 1000 equals 0 to 100%.
    while (TCC1->SYNCBUSY.bit.CC0);                // Wait for synchronization
    //Serial.println("Updadte NewReg.");
  }
  //Serial.println("NewReg: " + String((int)NewReg));

  delay(500);
}


// Output PWM 24Khz on digital pin D3 and D11 using timer TCC1 (10-bit resolution)
void setupTimers(){
  REG_GCLK_GENDIV = GCLK_GENDIV_DIV(1) |          // Divide the 48MHz clock source by divisor N=1: 48MHz/1=48MHz
                    GCLK_GENDIV_ID(4);            // Select Generic Clock (GCLK) 4
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC |           // Set the duty cycle to 50/50 HIGH/LOW
                     GCLK_GENCTRL_GENEN |         // Enable GCLK4
                     GCLK_GENCTRL_SRC_DFLL48M |   // Set the 48MHz clock source
                     GCLK_GENCTRL_ID(4);          // Select GCLK4
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  // Enable the port multiplexer for the digital pin D3 and D11  **** g_APinDescription() converts Arduino Pin to SAMD21 pin
  PORT->Group[g_APinDescription[PWM_OUT0_PIN].ulPort].PINCFG[g_APinDescription[PWM_OUT0_PIN].ulPin].bit.PMUXEN = 1;
  PORT->Group[g_APinDescription[PWM_OUT1_PIN].ulPort].PINCFG[g_APinDescription[PWM_OUT1_PIN].ulPin].bit.PMUXEN = 1;
  
  // Connect the TCC1 timer to digital output D3 and D11 - port pins are paired odd PMUXO and even PMUXE
  // F & E specify the timers: TCC0, TCC1 and TCC2
  // Este 2 tindria de ser un 3 ???
  PORT->Group[g_APinDescription[2].ulPort].PMUX[g_APinDescription[2].ulPin >> 1].reg = PORT_PMUX_PMUXO_E;  // D3 is on PA11 = odd, use Device E on TCC1/WO[1]
  PORT->Group[g_APinDescription[11].ulPort].PMUX[g_APinDescription[11].ulPin >> 1].reg = PORT_PMUX_PMUXE_F; // D11 is on PA08 = even, use device F on TCC1/WO[0]

  // Feed GCLK4 to TCC0 and TCC1
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |         // Enable GCLK4 to TCC0 and TCC1
                     GCLK_CLKCTRL_GEN_GCLK4 |     // Select GCLK4
                     GCLK_CLKCTRL_ID_TCC0_TCC1;   // Feed GCLK4 to TCC0 and TCC1
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  // Dual slope PWM operation: timers countinuously count up to PER register value then down 0
  REG_TCC1_WAVE |= TCC_WAVE_POL(0xF) |           // Reverse the output polarity on all TCC0 outputs
                    TCC_WAVE_WAVEGEN_DSBOTH;     // Setup dual slope PWM on TCC0
  while (TCC1->SYNCBUSY.bit.WAVE);               // Wait for synchronization

  // Each timer counts up to a maximum or TOP value set by the PER register,
  // this determines the frequency of the PWM operation: Freq = 48Mhz/(2*N*PER) ?? N ??
  // Freq = 48e6/1000/2 = 24 kHz
  REG_TCC1_PER = NewPer;                           // Set the FreqTcc of the PWM on TCC1 to 24Khz
  while (TCC1->SYNCBUSY.bit.PER);                // Wait for synchronization
 
  // Set the PWM signal to output , PWM ds = 2*N(TOP-CCx)/Freqtcc => PWM=0 => CCx=PER, PWM=50% => CCx = PER/2
  REG_TCC1_CC1 = NewReg;                             // TCC1 CC1 - on D3
  while (TCC1->SYNCBUSY.bit.CC1);                   // Wait for synchronization
  REG_TCC1_CC0 = NewReg;                             // TCC1 CC0 - on D11
  while (TCC1->SYNCBUSY.bit.CC0);                   // Wait for synchronization
 
  // Divide the GCLOCK signal by 1 giving  in this case 48MHz (20.83ns) TCC1 timer tick and enable the outputs
  REG_TCC1_CTRLA |= TCC_CTRLA_PRESCALER_DIV1 |    // Divide GCLK4 by 1
                    TCC_CTRLA_ENABLE;             // Enable the TCC0 output
  while (TCC1->SYNCBUSY.bit.ENABLE);              // Wait for synchronization
}
