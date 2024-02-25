// Control dun Convertidor DC-DC NQR010A0X4Z
// amb Potenciometres Digital CAT5113
// Amb realimentació de Vout amb un Divisor de Tensió.
// Busqueda de la Máxima Transferencia de Potencia.
// Sense Realimentació de Corrent de Sortida,
// Per lo que suposem que obtindrem la máxima potencia de sortida
// amb la máxima tensió de sortida.

// Amb Patalleta OLED 128x64
#include <Adafruit_SSD1306.h>   // OLED

// Definició de Pins
// Potenciometre Digital
#define DGP1_INC_NEG 3   // DigiPot 1 Increment Negada
#define DGP1_U_D_NEG 4   // DigiPot 1 Uncrease - Decrease Negada
                         // 1 = Increment, 0 = Decrement
#define DGP1_TAPS 100    // Nombre de posicions del DigiPot 1

// Entrades Analogiques
#define A_VOUT A0   // Pin Analogic on Conectem la tensió de sortida del DigiPot

#define  Bits_ADC 10    // Bits del conversor Analogic Digital
#define  VCC 5.00       // Tensió d'aliementació del conversor a Digital de turno

// Resistencies del divisor (sensor) de tensió de sortida.
float Rd1o=10000;
float Rd2o=10000;
float X_Vout=(Rd1o+Rd2o)/Rd2o;   // Multiplicador per compensar el divisor de tensió
                  // d'entrada que permet mesurar tensions superiors
                  // a la tensió d'alimentació del conversor AD.

const int MaxBin=pow(2,Bits_ADC);   // Final d'escala en Binari

// OLED
#define OLED_RESET A4
Adafruit_SSD1306 display(OLED_RESET);

// Mides de la Pantalla
#define PantallaX 128
#define PantallaY 64

#if (SSD1306_LCDHEIGHT != PantallaY)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

int Lec_Vout=0;
float Vout=0;
float Vout_Ant=0;

float Vout_MAX_V=20;     // Tensió máxima que volem a la sortida [Volts].
float Rout=4.7;     // Resistencia de Sortida.
float Iout_MAX=10;       // Corrent máxima de sortida, que no volem sobrepassar.
float Vout_MAX_I=Iout_MAX*Rout;         // I=V/R, V=I*R.
float Vout_MAX=Vout_MAX_V;

void dgp1(int increment){
  // Configurem el digipot per incrementar o decrementar
  if (increment > 0){ digitalWrite(DGP1_U_D_NEG, HIGH); }
  else{ digitalWrite(DGP1_U_D_NEG, LOW); }
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
  Serial.print("Pin _Inc:");
  Serial.println(DGP1_INC_NEG);

  pinMode(DGP1_U_D_NEG, OUTPUT);
  Serial.print("Pin U/_D:");
  Serial.println(DGP1_U_D_NEG);

  Serial.print("X_Vout: ");
  Serial.println(X_Vout);

  // Configuracio Pantalla
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Estes plaques inicialitcen a la adresa I2C 0x3C.
  // Clear the buffer. Borra el log de Adafruit de la memoria de la pantalla
  display.clearDisplay();

  // text display tests
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("SEUTec");
  display.setTextSize(2);
  display.setCursor(30,40);
  display.println("MxPwrPnT");
  display.display();
  delay(2000);
  display.clearDisplay();
 
}

void loop() {

  dgp1(1);    // Incrementam el valor del potencionemtre digital
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  Vout_Ant = Vout;
  Serial.print("Vout_Ant: ");
  Serial.println(Vout_Ant);
  Lec_Vout = analogRead(A_VOUT);
  Vout = X_Vout * Lec_Vout * VCC / (MaxBin-1);
  Serial.print("Vout: ");
  Serial.println(Vout);
  if (Vout < Vout_Ant ){ dgp1(-1); }  // Si Vout ha baixat tornem a la pos anterior
  if (Vout > Vout_MAX ){ dgp1(-1); }

  dgp1(-1);   // Provem baixant el valor del potenciometre
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  Vout_Ant = Vout;
  Serial.print("Vout_Ant: ");
  Serial.println(Vout_Ant);
  Lec_Vout = analogRead(A_VOUT);
  Vout = X_Vout * Lec_Vout * VCC / (MaxBin-1);
  Serial.print("Vout: ");
  Serial.println(Vout);
  if (Vout < Vout_Ant ){ dgp1(1); }  // Si Vout ha baixat tornem a la pos anterior
  if (Vout > Vout_MAX ){ dgp1(1); }

  // Configurar tamaño de Text
  display.setTextSize(2);

  display.setCursor(0,0);
  display.print("Vout");
  display.setCursor(0,18);
  if ( Lec_Vout == 1024  ) { display.print(">"); }
  display.print(Vout);

  display.setCursor(65,0);
  display.print("MAX");
  display.setCursor(65,18);
  display.print(Vout_MAX);

  display.display();
  display.clearDisplay();

  delay(10);
}
