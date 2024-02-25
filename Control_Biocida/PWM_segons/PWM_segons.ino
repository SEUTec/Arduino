/* PWM Temporitzador basat amb millis()
 * Per Freq. de conmutacio molt baixes o Periodes de conmutacio molt 
 * elevats, de segons,

  Test:
  Connectar l'osciloscop entre GND y D13 ( LED_BUILDIN ).
  Configurar T_ms = 1000, Duty = 50
  La freqüència mesurada ha de ser de 1 Hz, El periode de 1 segon.

  Note: Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO 
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino model, check
  the Technical Specs of your board  at https://www.arduino.cc/en/Main/Products
 */

const int ledPin = LED_BUILTIN;   // LED pin

int outState = LOW;             // outState, Estat Sortida

// El valor de millis() el guardarem en una variable tipo "unsigned long"
// Un unsgined int o int, es derbordaría massa ràpid
unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long currentMillis = 0;

// Podem utilitzar usngined int o int per testar els efectes del desbordament
// de millis en poc temps, i verificarne la gestió del desbordament
//unsigned int previousMillis = 0;
//unsigned int currentMillis = 0;

unsigned long dt = 0;

unsigned int T_s = 10;            // Periode total en Segons
unsigned long T_ms = T_s * 1000;            // Periode total en miliSegons.
unsigned long Duty = 50;            // Duty Cycle 0 a 100
unsigned long Ton = T_ms * Duty / 100;          // Periode en ON.
unsigned long Toff = T_ms - Ton;

void setup() {
  // set the digital pin as output:
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);

  Serial.print("T: ");
  Serial.print(T_s);
  Serial.print(" s = ");
  Serial.print(T_ms);
  Serial.println(" ms");

  Serial.print("Ton:");
  Serial.print(Ton);
  Serial.println(" ms");
  
  Serial.print("Toff:");
  Serial.print(Toff);
  Serial.println(" ms\n");
}

void loop() {
  // check to see if it's time to blink the LED; that is, if the
  // difference between the current time and last time you blinked
  // the LED is bigger than the interval at which you want to
  // blink the LED.
  // millis() retorna els milisegons que fa que corre el programa
  // valor máxim per un unsigned log = 2exp(32)-1 = 4 294 967 295 ms
  // = 4 294 967.3 s 
  // = 71582.8 min
  // = 1193 hores
  // = 49,7 díes

  currentMillis = millis();
  //Serial.print("currentMillis=");
  //Serial.println(currentMillis);
  //Serial.print("previousMillis=");
  //Serial.println(previousMillis);

  dt = currentMillis - previousMillis;
  //Serial.print("dt=");
  //Serial.println(dt);

  if (previousMillis > currentMillis){
    //Serial.println("Desbordament de millis()");
    //unsigned long dt = pow(2,16)-1 - previousMillis + currentMillis;  // Per al test de desbordament amb unsgined int
    unsigned long dt2 = pow(2,32)-1 - previousMillis + currentMillis; // Per al funcionament normal amb unsignet long
    //Serial.print("dt=");
    //Serial.println(dt);
  }

  //Serial.print("Temps Periode Actual: ");
  //Serial.print(dt);
  //Serial.println(" ms\n");
 
  //if (dt >= Ton && outState == HIGH) {
  if (dt >= Ton && outState) {
    // save the last time you switched the OUTPUT
    previousMillis = currentMillis;
    outState = LOW;
  }
  //else if (dt >= Toff && outState == LOW) {
  else if (dt >= Toff && !outState) {
    // save the last time you switched the OUTPUT
    previousMillis = currentMillis;
    outState = HIGH;
  }
  // set the LED with the outState of the variable:
  digitalWrite(ledPin, outState);
}
