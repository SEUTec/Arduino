/*********************************************************************
Rendimetre v1.2

Hardware:
Arduino UNO, NANO, Pro Mini.
Monochrome OLEDs based on SSD1306 drivers.
  128x64 size display using I2C to communicate

Coded by SEUTec
*********************************************************************/

#include <Adafruit_SSD1306.h>

// Pins Entrades Analogiques de Mesura
#define A_VIN A0   // Proto A0, PCB A2 ????????
#define A_IIN A1   // Proto A1, PCB A3 ????????
#define A_VOUT A2   // Proto A2, PCB A1 ????????
#define A_IOUT A3   // Proto A3, PCB A0 ????????

// Configuració conversor Analog-Digital
// Bits = 10;        // Bits de resolució del Conversor Analogic a Digital
// MaxLec=pow(2,Bits);   // Final d'escala que correspon a la tensió AREF.
#define MaxLec 1024

#define NLec 30        // Nombre de Lectures de corrent per promijar
int Lec=0;            // Variable per contar lectures
int Lec_Vin;
int Lec_Iin;
int Lec_Vout;
int Lec_Iout;

// Ajustarem la referencia de tensió del Conversors a un valor
// més proxim però superior als valor de tensió máxima que 
// s'esperen dels sensors.
#define AnalogREF INTERNAL
#define AREF 5.00

// Resistencies del divisor (sensor) de tensió d entrada.
float Vin=0;
#define Rdiu 9400
#define Rdid 4700
float X_Vin=(Rdiu+Rdid)/Rdid;
                  // Multiplicador per compensar el divisor de tensió
                  // d'entrada que permet mesurar tensions superiors
                  // a la tensió d'alimentació del conversor AD.

// Resistencies del divisor (sensor) de tensió de sortida.
float Vout=0;
#define Rdou 9400
#define Rdod 4700
float X_Vout=(Rdou+Rdod)/Rdod;
                  // Multiplicador per compensar el divisor de tensió
                  // d'entrada que permet mesurar tensions superiors
                  // a la tensió d'alimentació del conversor AD.

// Sensors de Corrent Hall ACS712 (4.5 a 5.5 V)
float Iin=0;
float Iout=0;
#define AREF_SC 2.5
#define Sens_SC 0.185  // Sensivilitat en Volts/Amper versió +-5A.
                  // Per lo que a 5A dona 5 * 0.185 = 0.925 Volts.
//#define Sens_SC 0.100  // V/A  versió +-20A.
                  // Per lo que a 20A dona 20 * 0.100 = 2.00 Volts.
//#define Sens_SC 0.066  // V/A  versió +-30A.
                  // Per lo que a 30A dona 30 * 0.066 = 1.98 Volts.

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


void Mira_Vin(){
  Lec = 0;
  for (int i = 0; i < NLec; i++){
    Lec = Lec + analogRead(A_VIN);
    delay(1);  // Esperar ms
  }
  Lec_Vin = Lec / NLec;
  // Convesió de Lectura Conversor a Volts.
  Vin = X_Vin * Lec_Vin * AREF / MaxLec;
}

void Mira_Iin(){
  Lec = 0;
  for (int i = 0; i < NLec; i++){
    Lec = Lec + analogRead(A_IIN);
    delay(1);  // Esperar ms
  }
  Lec_Iin = Lec / NLec;
  // Convesió de Lectura Conversor a Volts.
  Iin = (( Lec_Iin * AREF / MaxLec ) - AREF_SC) / Sens_SC;
}

void Mira_Vout(){
  Lec = 0;
  for (int i = 0; i < NLec; i++){
    Lec = Lec + analogRead(A_VOUT);
    delay(1);  // Esperar ms
  }
  Lec_Vout = Lec / NLec;
  // Convesió de Lectura Conversor a Volts.
  Vout = X_Vout * Lec_Vout * AREF / MaxLec;
}

void Mira_Iout(){
  Lec = 0;
  for (int i = 0; i < NLec; i++){
    Lec = Lec + analogRead(A_IIN);
    delay(1);  // Esperar ms
  }
  Lec_Iout = Lec / NLec;
  // Convesió de Lectura Conversor a Volts.
  Iout = (( Lec_Iout * AREF / MaxLec ) - AREF_SC) / Sens_SC;
}

void setup()   {                
  Serial.begin(9600);

  Serial.println("SEUTec RediMetre");
  Serial.println("");
  Serial.println("Vin\tIin\tVout\tIout\tPin\tPout\tEffi");

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  //display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Estes plaques inicialitcen a la adresa I2C 0x3C.
  // init done
  
  // Clear the buffer. Borra el log de Adafruit de la memoria de la pantalla
  display.clearDisplay();

  // text display tests
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("SEUTec");
  display.setTextSize(2);
  display.setCursor(0,30);
  display.println("Rendimetre");
  display.display();
  delay(3000);
  display.clearDisplay();

}



void loop() {
 
  // Lectura entrades analogiques
  Mira_Vin();
  Mira_Iin();
  Mira_Vout();
  Mira_Iout();

  float Pin = Vin * Iin;
  
  float Pout = Vout * Iout;

  float Rend = Pout / Pin;

  // Mostrar les dades pel monitor serie
  if ( Lec_Vin == 1024  ) { Serial.print(">"); }
  Serial.print(Vin);
  Serial.print("\t");
  if ( Lec_Iin == 1024  ) { Serial.print(">"); }
  Serial.print(Iin);
  Serial.print("\t");
  if ( Lec_Vout == 1024  ) { Serial.print(">"); }
  Serial.print(Vout);
  Serial.print("\t");
  if ( Lec_Iout == 1024  ) { Serial.print(">"); }
  Serial.print(Iout);
  Serial.print("\t");
  if ( Lec_Vin == 1024  || Lec_Iin == 1024) { Serial.print(">"); }
  Serial.print(Pin);
  Serial.print("\t");
  if ( Lec_Vout == 1024  || Lec_Iout == 1024) { Serial.print(">"); }
  Serial.print(Pout);
  Serial.print("\t");
  Serial.print("Rend: ");
  Serial.print(Rend);
  Serial.print("\n");

// Mostrar les dades per la Pantalla OLED.
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Vin     Iin     Pin");

  display.setCursor(0,10);
  if ( Lec_Vin == 1024  ) { display.print(">"); }
  display.print(Vin);
  display.setCursor(50,10);
  if ( Lec_Iin == 1024  ) { display.print(">"); }
  display.print(Iin);
  display.setCursor(95,10);
  if ( Lec_Vin == 1024  || Lec_Iin == 1024) { Serial.print(">"); }
  display.print(Pin);

  display.setCursor(0,25);
  display.print("Vout    Iout    Pout");

  display.setCursor(0,35);
  display.print(Vout);
  if ( Lec_Vout == 1024  ) { display.print(">"); }
  display.setCursor(50,35);
  if ( Lec_Iout == 1024  ) { display.print(">"); }
  display.print(Iout);
  display.setCursor(95,35);
  if ( Lec_Vout == 1024  || Lec_Iout == 1024) { Serial.print(">"); }
  display.print(Pout);

  display.setTextSize(2);
  display.setCursor(0,50);
  display.print("Rend: ");
  display.setCursor(70,50);
  display.print(Rend);

  display.display();
  display.clearDisplay();

  delay(1000);
}


