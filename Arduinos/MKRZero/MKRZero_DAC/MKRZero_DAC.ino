// Test Convertidor Digital Analogic dels Arduino Zero
//
// Utilitza la I/O Analogica A0 amb analogWrite(), 10 bits, de 0 a (2**10-1), de 0 a 1023.
// Utilitzarem la Entrada Analogica A1 per vore la sortida analogica A0
// U

#define A0_MAX 1023   // Valor digital máxim que li poderm passar, 1023 a 10 bits.
#define A0_V_MAX 3.288  // Valor en volts máxim que pot proporcionar.
                      // Avans d'arrivar al 1023, es queda a 2.25 V, a 750 aprox.
                      // Alimentar amb el uUSB Vcc es de 3.292 V.
                      // Pel que sembla la tensió máxima de sortida del DAC es de 2.25V.
                      // Tot i que els calculs els hem de fer amb una ref de 3.3 V.
                      // Si conectem la base d'un transistor amb una R de 1k només arriva a 2.11 V.
                      // Per lo que no li podrem aplicar molta carga.

#define VREF 3.288     // Tensió de referencia per les entrades analogiques

unsigned long tini=0;
unsigned long tfi=0;
unsigned int dif=0;

#define SERIAL_BAUDS 9600
//#define SERIAL_BAUDS 19200
//#define SERIAL_BAUDS 38400
//#define SERIAL_BAUDS 57600
//#define SERIAL_BAUDS 74880
//#define SERIAL_BAUDS 115200
//#define SERIAL_BAUDS 230400
//#define SERIAL_BAUDS 250000

void setup() {
  int SerialTimeOut=15;  // Temps Máxim Espera Monitor Serie en segons
  
  // initialize the serial communication:
  Serial.begin(SERIAL_BAUDS);
  while(!Serial && SerialTimeOut){
    delay(5000); // Espera uns ms la conneccio del Monitor Serie.
    SerialTimeOut--;
  }
  
  Serial.println("Test Convertidor Digital Analogic dels Arduino Zero.");

  //pinMode(A0,OUTPUT);       // Configura A0 com digital output i intefereix amb el DAC
                              // Limitant la tensió de sortida a 2.25 V aprox.
                              // impredint que arrive a Vcc de 3.3 V aprox.
  analogWriteResolution(10);  // Máxima Resolucio Sortida Analogica, 10 bits
  analogReadResolution(12);   // Máxima Resolucio Entrades Analogiques, 12 bits.

  Serial.println("Inici Test. Mesura Tensió en A0.");
  Serial.println("Ha s'augmentar fins a 3.3 Volts ????");
}

int val=0;

void loop() {

  if (val <= A0_MAX){
      Serial.println("A0: " + String(val));
      //analogWrite(A0,A0_MAX);
      //analogWrite(A0,255);
      analogWrite(A0,val);
      delay(500);
  }
  else { val = 0; }
  val += 16-1; // Incrementa valor a enviar al DAC.
  //val += 32-1; // Incrementa valor a enviar al DAC.

  // Now read A1 (connected to A0), and convert that
  // 12-bit ADC value to a voltage between 0 and 3.3.
  // Estes mesures de Tensio coincideixen amb les del multimetre
  // Per lo que referencia Analogica per defecte de 3.3 es correcta
  // tant per al ADC com par al DAC, simplement el DAC no dona més de 2.25 V.
  float voltage = analogRead(A1) * VREF / 4096.0;
  Serial.println("Volts: " + String(voltage)); // Print the voltage.
  delay(5000); // Delay en ms
  
}
