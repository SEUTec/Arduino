// Cargador Solar Bici 42V MaxPowerPoint
// Control d'un Convertidor DC-DC Step Up.
// Amb Vin: 8 - 32 V, Vout: Vin+1 - 46 V, 7A.
// amb LEDDR - LED Dependendent Resistor ( LED + LDR )
// en este cas NSL-32R3 controlad per PWM.
// Amb realimentació de Vout amb un Divisor de Tensió.
// Limitador de Corrent de Carga.
// Test de Tensió de Bateria periodic.
// Accio de control segons Tensió de bateria, es a dir, aturar carga.
// Falta:
// Implementar el MaxPowerPoint


// Definició de Pins
// Entrades Analogiques
#define Vout_pn A0   // Pin Analogic on Conectem la tensió de sortida del Carregador
// Bits_ADC 10    // Bits del conversor Analogic Digital
// MaxLec=pow(2,Bits_ADC);   // Final d'escala en Binari
#define MaxLec 1024

// Ajustarem la referencia de tensió del Conversors a un valor
// més proxim però superior als valor de tensió máxima que 
// s'esperen dels sensors.
#define AREF 5.00

// Resistencies del divisor (sensor) de tensió de sortida.
#define Rdou 47000
#define Rdod 4700
double X_Vout=(Rdou+Rdod)/Rdod;   // Multiplicador per compensar el divisor de tensió
                  // d'entrada que permet mesurar tensions superiors
                  // a la tensió d'alimentació del conversor AD.

// Sensors de Corrent Hall ACS712 (4.5 a 5.5 V)
#define Iout_pn A1
unsigned char NLec=5; // Nombre de Lectures de corrent per promijar
float Sens_SC=0.185;  // Sensivilitat en Volts/Amper versió +-5A.
                  // Per lo que a 5A dona 5 * 0.185 = 0.925 Volts.
//double Sens_SC=0.100;  // V/A  versió +-20A.
                  // Per lo que a 20A dona 20 * 0.100 = 2.00 Volts.
//double Sens_SC=0.066;  // V/A  versió +-30A.
                  // Per lo que a 30A dona 30 * 0.066 = 1.98 Volts.
float Iout_LMT = 2.50;   // Limitació de Corrent de sortida en Ampers.
                // El carregador original es una Font de 42.20 V 
                // Amb limitador de Corrent a 2.00 A. Per lo que carrega
                // a 2.00 A fins que la tensió de la bateria s'aproxima
                // als 42 V que la corrent va baixant.
                // Com que una corrent de 2.0 A no acalenta la batería
                // podem intentar carregarla a una corrent més alta,
                // potser 3 o 4 A, per reduir el temps de carga.

// PWM Control LEDDR
#define LEDDR_pin 3
// Convertir Duty Cycle en % a 0 a 255
float Duty = 0;
int Duty_byte = Duty * 2.55;

float Vbat = 0;   // Tensió de la bateria actual
float Vbat_fnl = 42.20; // Tensió baterial final carga
float TVbat = 5;  // Temps [min.] de revisió de la tensió de la bateria.
#include <TimerOne.h> // Perporitzador per controlar este periode.

// Configuració del control
float Vcarga=Vbat_fnl+0.5; // Vout que volem
float Kv = 0.3;     // Constant de Porporcionalitat control de tensio
float Ki = 0.3;     // Constant de Proportcionlitat control limit de corrent

// OLED
// Amb Patalleta OLED 128x64
#include <Adafruit_SSD1306.h>   // OLED

#define OLED_RESET A4
Adafruit_SSD1306 display(OLED_RESET);

// Mides de la Pantalla
#define PantallaX 128
#define PantallaY 64

#if (SSD1306_LCDHEIGHT != PantallaY)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// Comptador de temps en segons
volatile unsigned long ContSegons;
void Crono() {
  ContSegons++;
}

void setup() {

  Timer1.initialize(1000000);  // 1000us = 1ms, 1000000us=1s.
  Timer1.attachInterrupt(Crono);

  // Configuració de sortida PWM
  pinMode(LEDDR_pin, OUTPUT);

  // Configuracio Pantalla
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Estes plaques inicialitcen a la adresa I2C 0x3C.
  // Borra el logo de Adafruit de la memoria de la pantalla
  display.clearDisplay();

  // text display tests
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("SEUTec");
  display.setTextSize(2);
  display.setCursor(15,25);
  display.println("BiciCrgdr");
  display.setCursor(15,42);
  display.println("MxPwrPnT");
  display.display();
  delay(5000);
  display.clearDisplay();

  Serial.begin(9600);
}

void loop() {

  // Lectura Tensió de Carga
  int Lec = 0;
  for (int i = 0; i < NLec; i++){
    Lec = Lec + analogRead(Vout_pn);
    delay(10);
  }
  int Lec_Vout = Lec / NLec;
  // Convesió de Lectura Conversor a Volts.
  float Vout = X_Vout * Lec_Vout * AREF / MaxLec;

  Serial.print(" Vout: ");
  Serial.print(Vout);

  // Lectura Corrent de Carga
  Lec = 0;
  for (int i = 0; i < NLec; i++){
    Lec = Lec + analogRead(Iout_pn);
    delay(10);
  }
  float Lec_Iout = Lec / NLec;
  // Convesió de Lectura Conversor a Volts.
  //float Iout = Lec_Iout * AREF / MaxLec;   // Lectura en volts
  float Iout = (( Lec_Iout * AREF / MaxLec ) - ( AREF / 2 )) / Sens_SC;

  if ( Iout < Iout_LMT) { Duty = Duty + Kv * ( Vcarga - Vout ); }
  else { Duty = Duty + Ki * ( Iout_LMT - Iout ); }
                                  // Si Iout > Iout_LMT reduir Duty cycle

  if ( Duty > 100 ){ Duty=100; }
  if ( Duty < 0 ){ Duty = 0;}

   // De tant en tant mirar la tensió de la bateria en circuit obert
  // es a dir sense carregar
  noInterrupts();
  unsigned long Minuts = ContSegons / 60;
  interrupts();
  
  if ( Minuts == TVbat ){
    // Aturar carga
    analogWrite(LEDDR_pin, 0 );
    delay(100);  // Esperar una mica

    // Lectura Tensió de Bateria sense carregar
    Lec = 0;
    for (int i = 0; i < NLec; i++){
      Lec = Lec + analogRead(Vout_pn);
      delay(10);
    }
    int Lec_Vout = Lec / NLec;
    // Convesió de Lectura Conversor a Volts.
    Vbat = X_Vout * Lec_Vout * AREF / MaxLec;

    if ( Vbat > Vbat_fnl ) {
      Vcarga = 0;   // Aturar carga quan està completa
    }

    ContSegons = 0;
  }

  Duty_byte = Duty * 2.55;    // Cambiar rang de 0-100 a 0-255 de les sortides PWM
  analogWrite(LEDDR_pin, Duty_byte );

  Serial.print("  Duty [%]: ");
  Serial.println(Duty);

  Serial.print("Vbat: "); // Vbat
  Serial.println(Vbat);

// Configurar tamaño de Text
  display.setTextSize(2);

  display.setCursor(0,0);
  display.print("Vcrg");
  display.setCursor(0,18);
  if ( Lec_Vout == 1024  ) { display.print(">"); }
  display.print(Vout,1);

  display.setCursor(65,0);
  display.print("Icrg");
  display.setCursor(65,18);
  display.print(Iout,1);  // 1 decimal

  display.setCursor(0,36);
  display.print("Duty");
  display.setCursor(65,36);
  display.print(Duty,1);    // 1 decimal

  display.display();
  display.clearDisplay();

  delay(10);
}


