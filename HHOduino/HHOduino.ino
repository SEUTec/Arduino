// Control d'un Convertidor DC-DC NQR010A0X4Z.
// amb LEDDR - LED Dependendent Resistor ( LED + LDR )
// en este cas NSL-32R3 controlad per PWM.
// Amb realimentació de Vout amb un Divisor de Tensió.

// Definició de Pins
// Entrades Analogiques
#define A_VOUT A0   // Pin Analogic on Conectem la tensió de sortida del DigiPot
#define Bits_ADC 10    // Bits del conversor Analogic Digital
#define VCC 5.00       // Tensió d'aliementació del conversor a Digital de turno

// Resistencies del divisor (sensor) de tensió de sortida.
double Rd1o=10000;
double Rd2o=10000;
double X_Vout=(Rd1o+Rd2o)/Rd2o;   // Multiplicador per compensar el divisor de tensió
                  // d'entrada que permet mesurar tensions superiors
                  // a la tensió d'alimentació del conversor AD.

// Configuració conversor Analog-Digital
// Bits = 10;        // Bits de resolució del Conversor Analogic a Digital
// MaxLec=pow(2,Bits);   // Final d'escala que correspon a la tensió AREF.
#define MaxLec 1024

#define NLec 30        // Nombre de Lectures de corrent per promijar
int Lec=0;            // Variable per contar lectures

// Control On/Off
#define ON_OFF_pin 5
//char ON_OFF_STATE  

// PWM Control Vout LEDDR
#define LEDDR_pin 3
// Convertir Duty Cycle en % a 0 a 255
float Duty = 0;
int Duty_byte = Duty * 2.55;

// Configuració del control
#define Volem_Vout 3.00 // Vout que volem
float Error_Ant = Volem_Vout;
float Error = Error_Ant;
float Kp = 0.5;     // Constant de Porporcionalitat

void Mira_Vout(){
  Lec = 0;
  for (int i = 0; i < NLec; i++){
    Lec = Lec + analogRead(A_VOUT);
    delay(1);  // Esperar ms
  }
  int Lec_Vout = Lec / NLec;
  // Convesió de Lectura Conversor a Volts.
  Vout = X_Vout * Lec_Vout * AREF / MaxLec;
}

void setup() {

  Serial.begin(9600);
  
  // Configuració de sortida PWM
  pinMode(LEDDR_pin, OUTPUT);
}

void loop() {

  

  Mira_Vout();
  //Mira_Out();
  //Pout = Vout * Iout;

  Serial.print(" Vout: ");
  Serial.print(Vout);

  Duty = Duty + Kp * ( Volem_Vout - Vout );

  if ( Duty > 100 ){ Duty=100; }
  if ( Duty < 0 ){ Duty = 0;}

  Duty_byte = Duty * 2.55;    // Cambiar rang de 0-100 a 0-255 de les sortides PWM
  analogWrite(LEDDR_pin, Duty_byte );

  Serial.print("  Duty [%]: ");
  Serial.println(Duty);
 
  delay(10);
}


