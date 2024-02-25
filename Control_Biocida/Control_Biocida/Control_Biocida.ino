 /*
 * CONTROL BOMBA DOSIFICADORA DE BIOCIDA TORTOSA ENERGIA
 * 
 * Tenim 2 bombes que fiquen aigua al depòsit de Aigua de Bruta
 *   1 es una bomba del pou que introdueix ____________ L/h
 *   2 es l'altra bomba del pou que introdueix ____________ L/h
 * Volem dosificar el Biocida corresponent a la quantitat d'aigua que entra al tanc de Aigua Bruta.
 * Llavors quan funciona 
 *   bomba 1 tenim de dosificar _________ L/h de Biocida enviant _______ mA a la bomba dosificadora.
 *   bomba 2 tenim de dosificar _________ L/h de Biocida enviant _______ mA a la bomba dosificadora.
 *   Quan funciones les dos bombes tindrem de dosificar la suma de les dos dosificacions ????
 *   que correspòn a __________ mA a la señal de control de la bomba dosificadora.
 * 
 */

// Pantalla LCD I2C 20 Caracters x 4 Linies
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <Adafruit_ADS1X15.h>  // ADC 16 bits, Entrada Sensor Nivell
#include <Adafruit_MCP4725.h>  // DAC Sortida Bomba Dosificadora

// Entrada Analògica amb ADC
Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
bool Fallo_ADS1115 = LOW;

int16_t adc0;

float sns_Nivell_Volts;
float sns_Nivell_mA;
float sns_Nivell_100;
float sns_Nivell_MM;
float sns_Nivell_Mtrs;
float sns_Nivell_Ltrs;

// Pantalla LCD per bus I2C
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// Real Time Clock
RTC_DS3231 rtc;
DateTime date;

Adafruit_MCP4725 dac;
#define DAC_MAX 4095  // per ser de 12 bits, 2**12=4096
int DAC_Data;
bool Fallo_DAC = LOW;

// Temporitzador Funcionament Bomba Dosificadora
int EstatBomba = LOW;  // Meorització estat Bomba 
// Sortida Digital Control Bomba Dosificadora
#define BMB_DOSIFICA 5
//#define BMB_DOSIFICA LED_BUILTIN
float BOMBA_DOSI = 20.0;  // 0..100 % Dosificació bomba quan està en marxa.
float Bomba_Dosi = 0;     // Valor que enviarem al DAC = BOMBA_DOSI, quan funciona una Bomba de Pou.
                          // o BOMBA_DOSI * 2, quan funcionen les dos bombes del pou.

// El valor de millis() el guardarem en una variable tipo "unsigned long"
// Un unsgined int o int, es derbordaría massa ràpid
unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long currentMillis = 0;
unsigned long dt = 0;

 // Periode total en Segons, valor màxim = 2**32 / 1000 = 4294967295 / 1000 = 4.294.967.
unsigned long T_s = 60;        // Configuració definitiva.
//unsigned int T_s = 10;        // Configuració de Test.
unsigned long T_ms = T_s * 1000;      // Periode total en miliSegons, valor màxim 2**32 = 4.294.967.295.
//unsigned long T_ms = 2000;      // Periode total en miliSegons, valor màxim 2**32 = 4.294.967.295.
float Duty = 50;                      // Duty Cycle 0 a 100, tipo float per fer la divisió correctament.
unsigned long Ton_ms = T_ms * Duty / 100;       // Periode en ON.
unsigned long Toff_ms = T_ms - Ton_ms;          // Periode en OFF.

// Entrades Digitals
#define BMB1 2
#define BMB2 3

#define Puls_Frontal 6      // Pulsador Frontal
#define Tnk_Foradad 4       // sensor tanc foradat

// Sortides Digitals LEDSs Indicadors estat de les Bombes.
#define led_ERCROS 10     // Dosi Polialco (anteriorment ledverde1)
#define led_BMB1 11     // Dosi Bba 1 AB (anteriorment ledverde2)
#define led_BMB2 12     // Dosi Bba 2 AB (anteriorment ledverde3)

// Sortides Digitals LEDs Roijos Indicadors Dosificació.
//#define LED_DOSI_ALTA 7
//#define LED_DOSI_BAIXA 8
#define LED_DOSIFIACIO 7
#define LED_PA_LO_QUE_CONVINGUE 8
#define LED_TANC_FORADAD 9

// Entrades Analògiques
// Bits=10;    // Bits del conversor AD.
// Max_Lec = pow(2,Bits);   // Final d'escala en Binari
#define Max_Lec 1023

// Entrada Analògica del Potenciòmetre de temps funcionament bomba
#define POT_TMPS_BMB A0  // Potenciòmetre regulació temp de funcionament Bomba Dosificadora
int lecTmpsBomba;
#define MIN_POT_TMPS_BOMB 15 // Valor mínim dosificació amb el potenciòmetra tope a l'esquerra
#define MAX_POT_TMPS_BOMB 30 // Valor màxim dosificació amb el potenciòmetre tope a la dreta

// Entrada Analògica del Potenciòmetre de regulació bomba
#define POT_POT_BMB A1  // Potenciòmetre regulació Potencia Bomba Dosificadora
int lecPotBomba;
#define MIN_POT_POT_BOMB 20 // Valor mínim dosificació amb el potenciòmetra tope a l'esquerra
#define MAX_POT_POT_BOMB 40 // Valor màxim dosificació amb el potenciòmetre tope a la dreta

#define TEMPS_TEST_s 15    // Temps del Test inicial de LEDS, que mostra esta inicializació correcte o no
                           // del DAC, ADC, RTC, etc.. 

bool Fallo_RTC = LOW;

// El valor de millis() el guardarem en una variable tipo "unsigned long"
// Un unsgined int o int, es derbordaría massa ràpid
unsigned long previousMillis_Pantalla = 0;        // will store last time LED was updated
unsigned long currentMillis_Pantalla = 0;
unsigned long dt_Pantalla = 0;

int EstatPantalla = 0;
int T_temps = 10*1000;  // Temps mostra pantalla Temps en ms, 0.
int T_nivell = 10*1000; // Temps mostra pantalla Nivell en ms, 1.

unsigned int previousMillis_Act_Pantalla = 0;
unsigned int currentMillis_Act_Pantalla = 0;
#define T_ACT_PANTALLA 1000  // Periode d'actualització dades de la Pantalla.
                             // Com més gran menys parpadeig

unsigned int dt_Act_Pantalla = 0;


void setup() {
  // Entrades Digitals
  pinMode(Puls_Frontal, INPUT);
  pinMode(BMB1, INPUT);             // Contacte NO a 5V. 
  pinMode(BMB2, INPUT);
//  pinMode(BMB1, INPUT_PULLUP);    // Contacte NC a massa.
//  pinMode(BMB2, INPUT_PULLUP);
  pinMode(Tnk_Foradad, INPUT);

  // Sortides Digitals Leds Indicadors
  // Leds Verds Indicadors
  pinMode(led_ERCROS, OUTPUT); 
  pinMode(led_BMB1, OUTPUT);
  pinMode(led_BMB2, OUTPUT);
  // Leds Roijos d'Alarma
  pinMode(LED_DOSIFIACIO, OUTPUT); 
  pinMode(LED_PA_LO_QUE_CONVINGUE, OUTPUT);
  pinMode(LED_TANC_FORADAD, OUTPUT);
  // Bomba Dosificadora
  pinMode(BMB_DOSIFICA, OUTPUT);

  if (!ads.begin()) {
    Fallo_ADS1115 = HIGH; 
  }
  else{
    // Configuració del ADS1115
    ads.setGain(GAIN_TWOTHIRDS);
  }
  if (!rtc.begin()) {
    Fallo_RTC = HIGH; 
  }
  // Sortida Analògica DAC, amb convesor a 4-20mA.
  // Per Adafruit MCP4725A1 0x62 per defecte, pin ADDR a GND o CO, o 0x63 si pin ADDR a VCC)
//  if (!dac.begin(0x60)) {  // Par al MCP 4725 de Sparkfun tal com va de serie
  if (!dac.begin(0x62)) {  // Per al MCP 4725 de Adafruit 
    Fallo_DAC = HIGH;
  }
  //float Iini = 4.0;   // A la señal de control de bomba li tindria d'arrivar <= 4 mA, per aturada.      
  //DAC_Data = DAC_MAX / ICTRL_MAX * Iini;  // Càlcul valor a enviar al DAC per obtenir el Iini mA que volem          
                                          // com a valor inicial de senyal de control a bomba dosificadora.
  //DAC_Data = 409;  // 0 -> 0, 409 -> Vcc / 10, 4096 -> Vcc, 
  // DAC_Data = 4096 * 10.0 / 100 = 409.6
  //DAC_Data = int(DAC_MAX * BOMBA_DOSI / 100);  // Duty te de ser un float, per que face be la divició correctament.
  //dac.setVoltage(DAC_Data, false);             // false, per no guardar a la EEPROM.
  dac.setVoltage(0, false);             // Només arrancar tenim de ficar 0V, 0 mA, o la bomba se ficará a 20 mA.

  // Ajustar Data i Hora del RTC, a la data y l'hora d'este PC al compitar este programa.
  // Descomentar línea seguent, compolar, carregar al arduino, tornar a comentar.
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

  lcd.init();                      // initialize the lcd 
  lcd.backlight();           // Activa la Backight de la pantalla
//  lcd.noBacklight();      // Desactiva la Backlight de la pantalla
  lcd_test();
  leds_test();  // Activa tots els leds indicadors durant uns segons
  
  digitalWrite(led_ERCROS, HIGH);   // Se suposa que sempre hi ha caudal, o no ?
}

void loop() {
  // LEDs Indicador Estat Bombes
  digitalWrite(led_BMB1, digitalRead(BMB1));        // Activa LED Indicador Funcionament BOMBA POU 1, si cal.
  digitalWrite(led_BMB2, digitalRead(BMB2));        // Activa LED Indicador Funcionament BOMBA POU 2, si cal.
  digitalWrite(LED_TANC_FORADAD, digitalRead(Tnk_Foradad));  // Ilumina LED inficador Tanc Foradat, si cal.

  // Lectura del Nivell del Liquid que hi ha al Tanc
  // definida al lectura_nivell.ino, aquí hi ha algo que reseteja el Nano
  // Avans d'intentar accedir al ADC, hem de fer una ads.begin
  // Si no es pot accedir al ADC, per que no està connectad, se penja i no mostra res a la pantalla.
  if (!Fallo_ADS1115){
    lec_Nivell();
  }

  // Càlcul Duty per una bomba de pou en funcionament, segons posició potenciòmetre de temps
  // Dosifiació Bomba segons Potenciòmetre
  lecTmpsBomba = analogRead(POT_TMPS_BMB);
  // Potenciòmetre permet regular la potencia de MIN_POT_POT_BOMB a MAX_POT_POT_BOMB
  // float fmap(float x, float in_min, float in_max, float out_min, float out_max)
  float BOMBA_TEMPS = fmap(lecTmpsBomba, 0, Max_Lec, MIN_POT_TMPS_BOMB, MAX_POT_TMPS_BOMB);
  
  // Calcul Duty, Duty hade ser float per a que face la divisió correctament.
  if (!digitalRead(BMB1) && !digitalRead(BMB2)){  // No Funciona cap Bomba del Pou.
    Duty = 0.0;
    Bomba_Dosi = BOMBA_DOSI;
  }
  if (digitalRead(BMB1) && !digitalRead(BMB2)){  // Funciona només la Bomba del Pou 1.
    Duty = BOMBA_TEMPS;
    Bomba_Dosi = BOMBA_DOSI;
  }
  if (!digitalRead(BMB1) && digitalRead(BMB2)){  // Funcionen les 2 Bombes del Pou .
    Duty = BOMBA_TEMPS;                                       // En principi ha de ser igual que l'anterior.
    Bomba_Dosi = BOMBA_DOSI;
  }
  if (digitalRead(BMB1) && digitalRead(BMB2)){  // No funciona cap bomba del pou.
    //Duty = BOMBA_TEMPS * 2;       // Quan funciones les 2 bombes, doblem el temps o la dosificació
    Duty = BOMBA_TEMPS;             // Quan funciones les 2 bombes, mantenim el temps
    Bomba_Dosi = BOMBA_DOSI * 2;    // y doblem les rpms de la bomba.
  }

  Ton_ms = T_ms * Duty / 100;          // Periode en ON.
  Toff_ms = T_ms - Ton_ms;

  // Control temps bomba dosificadora amb una sortida digital
  currentMillis = millis();
  dt = currentMillis - previousMillis;

  // Dosifiació Bomba segons Potenciòmetre
//  lecPotBomba = analogRead(POT_POT_BMB);
  // Potenciòmetre permet regular la potencia de MIN_POT_POT_BOMB a MAX_POT_POT_BOMB
  // float fmap(float x, float in_min, float in_max, float out_min, float out_max)
//  BOMBA_DOSI = fmap(lecPotBomba, 0, Max_Lec, MIN_POT_POT_BOMB, MAX_POT_POT_BOMB);

  if (dt >= Ton_ms && EstatBomba) {
    previousMillis = currentMillis;
    EstatBomba = LOW;
    DAC_Data = int(DAC_MAX * 0.0 / 100);  // Duty te de ser un float, per que face be la divició correctament.
    dac.setVoltage(DAC_Data, false);             // false, per no guardar a la EEPROM.
  }
  else if (dt >= Toff_ms && !EstatBomba) {
    previousMillis = currentMillis;
    EstatBomba = HIGH;
    DAC_Data = int(DAC_MAX * Bomba_Dosi / 100);  // Duty te de ser un float, per que face be la divició correctament.
    dac.setVoltage(DAC_Data, false);             // false, per no guardar a la EEPROM.
  }
  digitalWrite(BMB_DOSIFICA, EstatBomba); // Activa Bomba Dosificadora segons Duty
  digitalWrite(LED_DOSIFIACIO, EstatBomba); // LED Reoig Superior per inficar quan la bomba dosifica.
  digitalWrite(LED_BUILTIN, EstatBomba);  // Activa el LED del Nano al Mateix temps
                                          // Alguns del LEDs Indicadors ???

  // Control Temporitzat de les dades a viasualitza en la pantalla.
  currentMillis_Pantalla = millis();
  dt_Pantalla = currentMillis_Pantalla - previousMillis_Pantalla;
  
  if (dt_Pantalla >= T_nivell && EstatPantalla==1) {  // Quan acaba temps pantalla Nivell,1, mostra Temps,0.
    // save the last time you switched the OUTPUT
    previousMillis_Pantalla = currentMillis_Pantalla;
    EstatPantalla = 0;
//    lcd_temps();
  }
  else if (dt_Pantalla >= T_temps && EstatPantalla==0) {  // Quan acaba temps pantalla Temps,0, mostra Nivell,1.
    // save the last time you switched the OUTPUT
    previousMillis_Pantalla = currentMillis_Pantalla;
    EstatPantalla = 1;
//    lcd_nivell();
  }

  currentMillis_Act_Pantalla = millis();
  dt_Act_Pantalla = currentMillis_Act_Pantalla - previousMillis_Act_Pantalla;

  if (dt_Act_Pantalla > T_ACT_PANTALLA){
    previousMillis_Act_Pantalla = currentMillis_Act_Pantalla;
    switch (EstatPantalla){
      case 0:
        lcd_dosi();
        break;

      case 1:
        lcd_nivell();
        break;
  
      default:
        lcd_main();
        break;
    }
  }
}
