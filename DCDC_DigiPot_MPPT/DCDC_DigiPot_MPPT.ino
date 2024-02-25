// Control dun Convertidor DC-DC NQR010A0X4Z
// amb Potenciometres Digital CAT5113
// Amb realimentació de Vout amb un Divisor de Tensió.
// Busqueda de la Máxima Transferencia de Potencia.
// Sense Realimentació de Corrent de Sortida,
// Per lo que suposem que obtindrem la máxima potencia de sortida
// amb la máxima tensió de sortida.

// Definició de Pins
// Potenciometre Digital
#define DGP1_INC_NEG 3   // DigiPot 1 Increment Negada
#define DGP1_U_D_NEG 4   // DigiPot 1 Uncrease - Decrease Negada
                         // 1 = Increment, 0 = Decrement
#define DGP1_TAPS 100    // Nombre de posicions del DigiPot 1

// Entrades Analogiques
#define A_VOUT A0   // Pin Analogic on Conectem la tensió de sortida del DigiPot

#define  Bits_ADC 10    // Bits del conversor Analogic Digital
#define  VCC 4.90       // Tensió d'aliementació del conversor a Digital de turno

// Resistencies del divisor (sensor) de tensió de sortida.
double Rd1o=10000;
double Rd2o=10000;
double X_Vout=(Rd1o+Rd2o)/Rd2o;   // Multiplicador per compensar el divisor de tensió
                  // d'entrada que permet mesurar tensions superiors
                  // a la tensió d'alimentació del conversor AD.

const int MaxBin=pow(2,Bits_ADC);   // Final d'escala en Binari

float Vout=0;
float Vout_Ant=0;
//float Vout_MAX=12;     // Tensió máxima que volem a la sortida [Volts].
float Rout=4.7;     // Resistencia de Sortida.
float Iout_MAX=1;       // Corrent máxima de sortida, que no volem sobrepassar.
float Vout_MAX=Iout_MAX*Rout;         // I=V/R, V=I*R.

void dgp1(int increment){
  // Configurem el digipot per incrementar o decrementar
  if (increment > 0){
    digitalWrite(DGP1_U_D_NEG, HIGH);
  }
  else{
    digitalWrite(DGP1_U_D_NEG, LOW);
  }
  increment=abs(increment);
  
  // Enviem els impulsos de variació del pot
  for(int incr = 0; incr < increment; incr++){
    digitalWrite(DGP1_INC_NEG, HIGH);
//    delay(100);
    digitalWrite(DGP1_INC_NEG, LOW);
  }
}


void setup() {

  Serial.begin(9600);
  Serial.println("Test Control Potenciometre Digital CAT5113 ");
  
  // Configuració de sortides digitals
  pinMode(DGP1_INC_NEG, OUTPUT);
  Serial.print("Pin Incremental:");
  Serial.println(DGP1_INC_NEG);

  pinMode(DGP1_U_D_NEG, OUTPUT);
  Serial.print("Pin Incremental:");
  Serial.println(DGP1_U_D_NEG);
}

void loop() {

  dgp1(1);    // Incrementam el valor del potencionemtre digital
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  Vout_Ant = Vout;
  Serial.print("Vout_Ant: ");
  Serial.println(Vout_Ant);
  Vout = X_Vout * analogRead(A_VOUT) * VCC / (MaxBin-1);
  Serial.print("Vout: ");
  Serial.println(Vout);
  if (Vout < Vout_Ant ){ dgp1(-1); }  // Si Vout ha baixat tornem a la pos anterior
  if (Vout > Vout_MAX ){ dgp1(-1); }

  dgp1(-1);   // Provem baixant el valor del potenciometre
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  Vout_Ant = Vout;
  Serial.print("Vout_Ant: ");
  Serial.println(Vout_Ant);
  Vout = X_Vout * analogRead(A_VOUT) * VCC / (MaxBin-1);
  Serial.print("Vout: ");
  Serial.println(Vout);
  if (Vout < Vout_Ant ){ dgp1(1); }  // Si Vout ha baixat tornem a la pos anterior
  if (Vout > Vout_MAX ){ dgp1(1); }
  
  delay(100);
}


