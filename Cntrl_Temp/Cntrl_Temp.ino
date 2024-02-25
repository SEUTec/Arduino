//Control de Temperatura amb Relés

/*thermistor parameters:
 * RT0: 10 000 Ω
 * B: 3977 K +- 0.75%
 * T0:  25 C
 * +- 5%
 */

//These values are in the datasheet default from this example
//#define RT0 10000   // Ω
//#define B 3977      // K
//--------------------------------------

//These values are in the datasheet E3D HotEnd Thermistor
#define RT0 100000   // Ω
#define B 4725      // K
//--------------------------------------

//These values are in the datasheet Air Fryer Thermistor ??
//#define RT0 200000   // Ω
//#define B 3475      // K
//--------------------------------------

#define VCC 5    //Supply voltage
//#define VCC 3.3    //Supply voltage per Duet y alguns Arduinos

#define R 10000  //R=10KΩ, defecte en este exemple.
//#define R 4700  //R=4K7Ω, com algunes Duet 0.6
//#define R 4700  //R=4K7Ω, com algunes Duet 0.8.5

//Variables Temperatura
float RT, VR, ln, TX, T0, VRT;

// Variables Sortides Reles
const int HotPin = 13;  // Per coincidir amb el LED del Arduino Nano
const int FanPin = 12;  // Per ser la del costat del 13.

int HotState = HIGH;  // Els relés están actius en LOW
int FanState = HIGH;

// Control de Temperatura
int Temp_Consigna = 60;
int Temp_Histeresis = 0;  // Valors de Conmutació respecte de la Temp. Consigna
                          // No cal histeresis, ja que temperatura continua augmentant
                          // per inercia quan s'atura el calefactor.

void setup() {
  // Port serie per enviar les dades al PC
  Serial.begin(9600);

  // Thermistor
  T0 = 25 + 273.15;                 //Temperature T0 from datasheet, conversion from Celsius to kelvin

  // Relés de Sortida
  pinMode(HotPin, OUTPUT);
  pinMode(FanPin, OUTPUT);
}

void loop() {
  VRT = analogRead(A0);              //Acquisition analog value of VRT
  VRT = (5.00 / 1023.00) * VRT;      //Conversion to voltage
  VR = VCC - VRT;
  RT = VRT / (VR / R);               //Resistance of RT

  ln = log(RT / RT0);
  TX = (1 / ((ln / B) + (1 / T0))); //Temperature from thermistor

  TX = TX - 273.15;                 //Conversion to Celsius

  Serial.print("Temp:");
  Serial.print("\t");
  Serial.print(TX);
  Serial.print(" / ");
  Serial.print(Temp_Consigna);
  Serial.print(" ºC");

//  Serial.print(TX + 273.15);        //Conversion to Kelvin
//  Serial.print("K\t");
//  Serial.print((TX * 1.8) + 32);    //Conversion to Fahrenheit
//  Serial.print("F");

  if( TX > Temp_Consigna + Temp_Histeresis){
    HotState = HIGH; // Active at LOW state
  }
  if( TX < Temp_Consigna - Temp_Histeresis){
    HotState = LOW;
  }
  
  FanState = LOW;
  
  digitalWrite(HotPin, HotState);
  digitalWrite(FanPin, FanState);
  
  Serial.print("\tHotState: ");
  Serial.print(HotState);
  //Serial.print("\tFanState: ");
  //Serial.print(FanState);
  Serial.println("");

  delay(500);
}




