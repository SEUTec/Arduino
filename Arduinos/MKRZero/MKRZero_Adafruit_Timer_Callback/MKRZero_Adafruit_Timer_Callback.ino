/*
 * MKRZero Adafruit Timer Callback
 * 
 * Es te d'obrir el monitor serie per a s'inicie la generació del PWM.
 * Si el monitor serie no va be, inicial el PWM pero no mostra el missatges
 * que envia l'arduino, en Linux, es pot utilitzar moserial.
 */

#include <Arduino.h>
#include "Adafruit_ZeroTimer.h"

// This example can have just about any frequency for the callback
// automatically calculated!
//float freq = 3500.0; // 1 KHz massa freq. per un LED
float freq = 0.5; // 10 Hz millor per a fer parpadejar un LED
float freq_pwm = 2 * freq;  // freq_pwm = 2 * freq. interrupció

#define CLOCK 48000000  // Freq. Rellotje del MKRZero

// Sortida
#define PWM_OUT_PIN 10

// timer tester
Adafruit_ZeroTimer zerotimer = Adafruit_ZeroTimer(3);

void TC3_Handler() {
  Adafruit_ZeroTimer::timerHandler(3);
}

// the timer callback
volatile bool togglepin = false;
void TimerCallback0(void){
  //digitalWrite(LED_BUILTIN, togglepin);
  digitalWrite(PWM_OUT_PIN, togglepin);
  togglepin = !togglepin;
}


void setup() {
  pinMode(PWM_OUT_PIN, OUTPUT);

  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }
  
  Serial.println("Timer callback tester");

  Serial.print("Desired freq (Hz):");
  Serial.println(freq);

  // Set up the flexible divider/compare
  uint16_t divider  = 1;
  uint16_t compare = 0;
  tc_clock_prescaler prescaler = TC_CLOCK_PRESCALER_DIV1;

  if ((freq_pwm < 24000000) && (freq_pwm > 800)) {
    divider = 1;
    prescaler = TC_CLOCK_PRESCALER_DIV1;
    compare = CLOCK/freq_pwm;
  } else if (freq_pwm > 400) {
    divider = 2;
    prescaler = TC_CLOCK_PRESCALER_DIV2;
    compare = CLOCK/2/freq_pwm;
  } else if (freq_pwm > 200) {
    divider = 4;
    prescaler = TC_CLOCK_PRESCALER_DIV4;
    compare = CLOCK/4/freq_pwm;
  } else if (freq_pwm > 100) {
    divider = 8;
    prescaler = TC_CLOCK_PRESCALER_DIV8;
    compare = CLOCK/8/freq_pwm;
  } else if (freq_pwm > 50) {
    divider = 16;
    prescaler = TC_CLOCK_PRESCALER_DIV16;
    compare = CLOCK/16/freq_pwm;
  } else if (freq_pwm > 12) {
    divider = 64;
    prescaler = TC_CLOCK_PRESCALER_DIV64;
    compare = CLOCK/64/freq_pwm;
  } else if (freq_pwm > 3) {
    divider = 256;
    prescaler = TC_CLOCK_PRESCALER_DIV256;
    compare = CLOCK/256/freq_pwm;
  } else if (freq_pwm >= 0.75) {
    divider = 1024;
    prescaler = TC_CLOCK_PRESCALER_DIV1024;
    compare = CLOCK/1024/freq_pwm;
  } else {
    Serial.println("Invalid frequency");
    while (1) delay(10);
  }
  Serial.print("Divider:"); Serial.println(divider);
  Serial.print("Compare:"); Serial.println(compare);
  Serial.print("Final freq:"); Serial.println((float)CLOCK/compare/divider/2);

  zerotimer.enable(false);
  zerotimer.configure(prescaler,       // prescaler
          TC_COUNTER_SIZE_16BIT,       // bit width of timer/counter
          TC_WAVE_GENERATION_MATCH_PWM // frequency or PWM mode
          );

  zerotimer.setCompare(0, compare);
  zerotimer.setCallback(true, TC_CALLBACK_CC_CHANNEL0, TimerCallback0);
  zerotimer.enable(true);
}

void loop() {
  Serial.println("tick");
  delay(1000);
}
