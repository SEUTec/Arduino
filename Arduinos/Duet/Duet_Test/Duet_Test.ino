// Test de la Duet amb drivers de motors sucarrats.

// Sortides MOSFETs
#define FAN0_PWM 34  //PC2
#define E0_PWM 40    //PC8
#define BED_PWM 6    //PC24

// Entrades Digitals. S'ha dalimentar la potencia pa que s'encenguen els LEDs.
// Los LEDs s encenen quan fiquem un LOW, GND.
#define X_STOP 11  //PD7
#define Y_STOP 28  //PD3
#define E0_STOP 31 //PA7


// I/O Digitals Expansion HEad



// Sortida Digital a Provar
int Digi_Pin=BED_PWM;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(Digi_Pin, OUTPUT);
  pinMode(FAN0_PWM, OUTPUT);
  pinMode(35, INPUT);

  //Serial.begin(115200);   //Monitor Serie el Programming Port
  SerialUSB.begin(115200);  //El monitor serie pel Native Port, Duet.
}

int Loop=0;

// the loop function runs over and over again forever
void loop() {
  Loop++;

  SerialUSB.print("Loop:");
  SerialUSB.println(Loop);
    
  digitalWrite(Digi_Pin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(Digi_Pin, LOW);    // turn the LED off by making the voltage LOW
  int lect=digitalRead(35);
  SerialUSB.println("Lec:" + String(lect));
  if (lect > 0) { 
    digitalWrite(FAN0_PWM, HIGH); 
  }
  else {
    digitalWrite(FAN0_PWM, LOW); 
  }
  delay(1000);                       // wait for a second

  
}
