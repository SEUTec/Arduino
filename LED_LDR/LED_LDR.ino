/* LED + LDR Tester
 *  
 *  Dispositiu un LED amb un Resistencia de 100 a una sortida PWM del Arduino,
 *  encarad a un LDR, per aconseguir un Optoacoplador Analogic o una
 *  resistencia controlada per lum.
*/

// Els valor del Duty va variant automaticament de 0 a 100%
// i de 100 a 0.

// Sortides PWM, Arduino UNO and NANO:
// Pins Digitals 3,5,6,9,10,11
// Micro 3,5,6,9,10,11 i 13.
// MKRZERO 0, 1, 2, 3, 4, 5, 6, 7, 8, 10, A3 - or 18 -, A4 -or 19
#define PWM1_pin 6

// Convertir Duty Cycle en % a 0 a 255
// Es a dir * 255 / 100 que es lo materix que * 2.55
int Duty = 0;
int Duty_byte = Duty * 2.55;

int Temps = 5000; // ms

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(PWM1_pin, OUTPUT);
  Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {
  // put your main code here, to run repeatedly:
  for ( Duty = 0; Duty < 10; Duty++ ){
    Duty_byte = Duty * 25.5;
    Serial.print("Duty [%]: ");
    Serial.print(Duty*10);
    Serial.print(" [byte]: ");
    Serial.println(Duty_byte);
    analogWrite(PWM1_pin, Duty_byte );
    delay(Temps);
  }
  for ( Duty = 10; Duty >= 0; Duty-- ){
    Duty_byte = Duty * 25.5;
    Serial.print("Duty [%]: ");
    Serial.print(Duty*10);
    Serial.print(" [byte]: ");
    Serial.println(Duty_byte);
    analogWrite(PWM1_pin, Duty_byte );
    delay(Temps);
  }
}
