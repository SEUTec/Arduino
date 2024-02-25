/* MKRZero Timer 
 *  Basat en Arduino Zero Timer Demo en:
 *  https://github.com/nebs/arduino-zero-timer-demo
 *  
 *  Originalment Requereix: 
 *  - Potenciometre al Pin Analogic 2, per ajustar frequencia.
 *  - LED al pin Digital 2.
 *  
 *  Ara ja funciona i tot.
 */

#include "Arduino.h"

#define LED_PIN 0
#define POT_PIN A2

#define POT_JITTER_THRESHOLD 10
#define CPU_HZ 48000000
#define TIMER_PRESCALER_DIV 1024

#define T_count 0.001   // Periode de conteix en segons, per comptar ms.
//#define T_count 1   // Periode de conteix en segons, comptar segons.
//#define T_count 60   // Periode de conteix en segons, compta minuts de 60s.

int Freq_Count = 1/T_count;
//unsigned long int Comptes_T = 15;   // Cicles a comptar
unsigned long int Comptes_T = 10000;   // Cicles a comptar, 5000ms = 5s, Periode
unsigned long int Comptes_Dty = 5000; // Cicles en on, 1000 ms = 1s, Duty Cycle o Ton.
unsigned long int Comptador = 0;          // Comptador de Cicles.
unsigned long int Comptador_Ant = 0;

bool Nou_T = false;

void setup(){
  pinMode(LED_PIN, OUTPUT);
  //pinMode(POT_PIN, INPUT);

  Serial.begin(9600);

  startTimer(Freq_Count);
}

void loop(){
  //Serial.print("Periode Conteix [s]: ");
  //Serial.println(T_count);

  Serial.print("Periode Conteix [ms]: ");
  Serial.println(T_count * 1000);
  
  Serial.print(" Freq. Comptador[Hz]: ");
  Serial.println(Freq_Count);
  
  if(Comptador != Comptador_Ant){
    Serial.print(" Comptador: ");
    Serial.println(Comptador);
    Comptador_Ant = Comptador;
  }

  if (Nou_T == true){
    Serial.println("          Nou Cicle.");
    Nou_T= false;
  }
  delay(500);   // Per reduir la fre. d'actualització de les dades al monitor serie.
}

void setTimerFrequency(int frequencyHz) {

  int compareValue = (CPU_HZ / (TIMER_PRESCALER_DIV * frequencyHz)) - 1;
  TcCount16* TC = (TcCount16*) TC3;
  // Make sure the count is in a proportional position to where it was
  // to prevent any jitter or disconnect when changing the compare value.
  TC->COUNT.reg = map(TC->COUNT.reg, 0, TC->CC[0].reg, 0, compareValue);
  TC->CC[0].reg = compareValue;
  while (TC->STATUS.bit.SYNCBUSY == 1);
}

/*
This is a slightly modified version of the timer setup found at:
https://github.com/maxbader/arduino_tools
 */
void startTimer(int frequencyHz) {
  REG_GCLK_CLKCTRL = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID (GCM_TCC2_TC3)) ;
  while ( GCLK->STATUS.bit.SYNCBUSY == 1 );

  TcCount16* TC = (TcCount16*) TC3;

  TC->CTRLA.reg &= ~TC_CTRLA_ENABLE;

  // Use the 16-bit timer
  TC->CTRLA.reg |= TC_CTRLA_MODE_COUNT16;
  while (TC->STATUS.bit.SYNCBUSY == 1);

  // Use match mode so that the timer counter resets when the count matches the compare register
  TC->CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;
  while (TC->STATUS.bit.SYNCBUSY == 1);

  // Set prescaler to 1024
  TC->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1024;
  while (TC->STATUS.bit.SYNCBUSY == 1);

  setTimerFrequency(frequencyHz);

  // Enable the compare interrupt
  TC->INTENSET.reg = 0;
  TC->INTENSET.bit.MC0 = 1;

  NVIC_EnableIRQ(TC3_IRQn);

  TC->CTRLA.reg |= TC_CTRLA_ENABLE;
  while (TC->STATUS.bit.SYNCBUSY == 1);
}

void TC3_Handler() {

  Comptador++;

  if (Comptador < Comptes_Dty){
    digitalWrite(LED_PIN, true);
  }
  else{
    digitalWrite(LED_PIN, false);
  }
  
  if (Comptador >= Comptes_T){
    Nou_T = true;
    Comptador = 0;
  }
  
  TcCount16* TC = (TcCount16*) TC3;
  // If this interrupt is due to the compare register matching the timer count
  // we toggle the LED.
  if (TC->INTFLAG.bit.MC0 == 1) {
    TC->INTFLAG.bit.MC0 = 1;
  }
}
