// Inversor amb driver de motor.
// 
// Per monitoritzar. Obrir el Monitor Serie i configurarlo
// Autoscroll, Nueva Línea i xxxx baudio, que ha de coincidir
// amb la configurada amb Serial.begin(xxx);
// Valors típics: 9600, 19200 Max Arduino UNO (Funduino).
// Altre valors típics: 38400, 57600, 74880, 115200, 230400, 250000

#define ADJUSTABLE        // Descomentar per Poder ajustar DutyMax, Freq_Seno y Freq_PWM.
#define SENO_PWM          // Descomentar per PWM senoidal
#define SERIAL_MONITOR    // Descomentar per Enviar dades pel Monitor Serie

#define MOTOR_1_ACTIVE
//#define MOTOR_2_ACTIVE

#include <TimerOne.h>
#include <LiquidCrystal.h>

/*
LiquidCrystal(rs, enable, d4, d5, d6, d7)
LiquidCrystal(rs, rw, enable, d4, d5, d6, d7)
LiquidCrystal(rs, enable, d0, d1, d2, d3, d4, d5, d6, d7)
LiquidCrystal(rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7)
*/
LiquidCrystal lcd(8, 2, 4, 5, 6, 7);           // select the pins used on the LCD panel
#define BACK_LIGHT 3   // BackLight Pin

int LCD_Updt = 0;     // Per millorar visibilitat de les dades, reduim freq actualitzacio.
#define LCD_UPDATE_CCLS 20

// Pins Driver Motor Shield
//#define MC33926_SHLD
//#define MC33926
#define HIGH_POWER_36v9

#ifdef HIGH_POWER_36v9
  #define M1_PWM 9 // No podem utilitzar qualsevol pin PWM.
                   // Tenim d'utilitzar un dels pins del Timer1.
                   // Per Timer1 hem d utilitzar el pin 9 o 10.
  #define M1_DIR 10
#endif

#ifdef MC33926_SHLD
  #define M1_PWM 9 // No podem utilitzar qualsevol pin PWM.
                   // Tenim d'utilitzar un dels pins del Timer1.
                   // Per Timer1 hem d utilitzar el pin 9 o 10.
  #define M2_PWM 10                  
  #define M1_DIR 7
  #define M2_DIR 8
  #define _nD2 4    // Enable Drv
#endif

// Entrades analogiques per variar PWM_Max, Freq_PWM i Freq_senoidal
#define PWM_PIN A2
#define FREQ_SENO_PIN A3

#define FREQ_SEN_MIN 50   // Frequencia Senoidal Minima
#define FREQ_SEN_MAX 400 // Frequencia Senoidal Maxima 400 Hz amb Duty Max 100%
                          // amb Dutys Inferior podem augmentar més la freq fins a 720 Hz
                          // encara que fiquem 900, la mesurada no passa de 720 Hz.
                          // 720 x 32 = 23 kHz de PWM que ha es prou.

#define SERIAL_BAUD 19200

int DutyMax = 0;
volatile int Duty = 0;   // 100 per obtenir un senyl quadrada de la freq especificada
int k = 1000;
int ms = 1000;     // *1e3 per convertit s en ms.
long us = 1000000; // *1e6 per convertir s en us.

unsigned int Freq = 75; // Frequencia de 1 Hz, fins 65535 kHz per ser Freq unsigned int
                        // Però més que suficient per conmutar MOSFETs de Potencia.

float T=1/(float)Freq; // Periode Senoide en segons
                      // Nombre Divisions senyal senoidal
                      // Tot i que esta freq baixa al augmentar la complexitat del programa
                      // La Freq PWM mínima tindria de ser superior a 1500 Hz
                      // deixa d'actualitzarse la pantalla, bueno tarda massa.
                      // Per un filtro LC amb L=5mH i C=4u7F
//#define DIV_SENO 32   // Per Obtindre ones senoidals de 400 Hz DIV_SENO 36. FREQ_PWM = 400 * 32 = 12800
int DIV_SENO = 50;   // Valor InerMig.
//#define DIV_SENO 100   // Per Senoides de 50 Hz DIV_SENO 100. FREQ_PWM = 50 * 100 = 5000

float Sen_Wv_100[100];
float Sen_Wv_50[50];
float Sen_Wv_32[32];

float dt=T/DIV_SENO;  // Periode actualizació Duty per crear senoide
float Freq_PWM=1/dt;

int Sen_Ind=0;

void setup(){

   lcd.begin(16, 2);               // start the library
   lcd.print("SEUTec. sPWM.");  // print a simple message on the LCD

   pinMode(BACK_LIGHT, OUTPUT);    // Backligth Control
   analogWrite(BACK_LIGHT, 180);   // PWM, 0 = Apagada, 255 = ON, Valor máxim ajustad amb el potenciometre

#ifdef SERIAL_MONITOR
  Serial.begin(SERIAL_BAUD);
  delay(2000);        // Esperar opertura monitor serie 5000 ms = 5s.
  //while(!Serial);   // Esperar opertura del monitor serie.
  // Wait untilSerial is ready - Leonardo
#endif

// Pins Motor Shield
#ifdef MOTOR_1_ACTIVE
  pinMode(M1_PWM, OUTPUT);
  pinMode(M1_DIR, OUTPUT);
#endif
#ifdef MC33926_SHLD
 #ifdef MOTOR_2_ACTIVE
  pinMode(M2_PWM, OUTPUT);
  pinMode(M2_DIR, OUTPUT);
 #endif
#endif

// Inicialitzacio Timer1.
  Timer1.initialize(dt*us);
  Timer1.attachInterrupt(callback_50);

  // Inicialitzar el driver del motor.
#ifdef MC33926_SHLD
  pinMode(_nD2,OUTPUT);
  digitalWrite(_nD2,HIGH); // default to on
#endif

  // Crear LookUp Table Senoidal per 100 divisions
  for (int i = 0; i < 100; i++){
    Sen_Wv_100[i]=sin(2*PI/100*i);
    //Serial.print(Sen_Wv_100[i]);
    //Serial.print(" ");
  }

  // Crear LookUp Table Senoidal per 50 divisions
  for (int i = 0; i < 50; i++){
    Sen_Wv_50[i]=sin(2*PI/50*i);
    //Serial.print(Sen_Wv_50[i]);
    //Serial.print(" ");
  }

  // Crear LookUp Table Senoidal per 50 divisions
  for (int i = 0; i < 32; i++){
    Sen_Wv_32[i]=sin(2*PI/32*i);
    //Serial.print(Sen_Wv_32[i]);
    //Serial.print(" ");
  }


#ifdef SERIAL_MONITOR
  Serial.println("");
  Serial.println("SEUTec. Inversor Senoidal.");
  Serial.println("");
  Serial.print("Freq [Hz]: ");
  Serial.println(Freq);
  Serial.print("Periode [s]: ");
  Serial.println(T);
  Serial.print("dt [us]: ");
  Serial.println(dt*us);

  // Parametres Filtro LC
  float L = 5e-3;     // Inductancia en H.
  float C = 4.7e-6;   // Capacitancia en F.
  float w0 = 1/sqrt(L*C); // Freq. Resonancia en Radiants
  float f0 = w0/2/PI;             // Freq. Resonancia en Hz.

  // Per frequencies d'ona senoidal superiors a f0/3 obtindrem una ona senoidal
  // deformada degut a la resonancia del filtro LC, per lo que pot ser necessari
  // reduir la Inductancia i o Capacitancia del filtre evitar aquestar deformació.
  // El problema es que el filtro que va be a uns 50 - 200 Hz
  // no va be a freq superiors, per lo que si volem un freq variable tindrem de
  // ajustar el filtre amb la frequencia.

  Serial.print("Parametres Filtre LC.");
  Serial.print(" L [mH]: "); Serial.print(L*1000);
  Serial.print(" C [uF]: "); Serial.print(C*1000000);
  Serial.print(" w0: " ); Serial.print(w0);
  Serial.print(" f0:"); Serial.print(f0);
  Serial.println("");
  
  Serial.flush();
#endif
}

void loop(){

#ifdef ADJUSTABLE
// Reconfigurar Sortida segons Entrades Analogiques
// Adjust de DuttyMax amb Entrada Analogica
    DutyMax=analogRead(PWM_PIN);

// Adjust de Freq Senoidal amb Entrada Analogica
    Freq = (float)analogRead(FREQ_SENO_PIN)/1023*(FREQ_SEN_MAX-FREQ_SEN_MIN)+FREQ_SEN_MIN;
    // if (Freq > FREQ_SEN_MAX) { Freq = FREQ_SEN_MAX; }  // NO CAL ?

    if (Freq > 250){  // Freq es menor a 250 Hz.
    	DIV_SENO = 32;
      Timer1.attachInterrupt(callback_32);
    	Serial.print("DIV_SENO=32");
    }
    else if (Freq > 150) {  // Freq es menor a 250 y > 150 Hz
    	DIV_SENO = 50;
      Timer1.attachInterrupt(callback_50);
    	Serial.print("DIV_SENO=50");
    }
    else {    // Freq es menor a 150 Hz
    	DIV_SENO = 100;
      Timer1.attachInterrupt(callback_100);
    	Serial.print("DIV_SENO=100");
    }
                   // Freq PWM proporcional a la Freq Senoidal
    T=1/(float)Freq; // Periode Senoide en segons
    dt=T/DIV_SENO;  // Periode actualizació Duty per crear senoide
    Freq_PWM = 1/dt;
   
#endif

#ifdef SERIAL_MONITOR
    //Serial.print("t ");
    //Serial.print(t);
    //Serial.print(" Duty Max [%]: ");
    //Serial.print(DutyMax * 100.0 / 1023.0);
    //Serial.print(" Duty [%]: ");
    //Serial.print(Duty);
    Serial.print(" F.Seno [Hz]: ");
    Serial.print(Freq);
    Serial.print(" dt [ms]: ");
    Serial.print(dt*1000);
    Serial.print(" F.PWM [Hz]: ");
    Serial.print(Freq_PWM);
    Serial.println();
#endif

  if ( LCD_Updt > LCD_UPDATE_CCLS){ // Actualitzar pantalla cada tants loops

   lcd.setCursor(4,1);  // Borrar ultim decimal, per si Freq es <100, 2 digits.
   lcd.print(" ");
   lcd.setCursor(0,1);
   lcd.print("F " + String(Freq));

   lcd.setCursor(15,1);   // Borrar ultim caracter del numero
   lcd.print(" ");
   lcd.setCursor(7,1);
   lcd.print("PWM " + String((int)Freq_PWM));

   LCD_Updt=0;
  }
  else {
    LCD_Updt++;
  }
}


void callback_100(){
  Sen_Ind++;

  if (Sen_Ind > 100){
    Timer1.setPeriod(dt*us);
    Sen_Ind = 0;
  }

//  Duty = DutyMax * sin(w*t);    // Amb table el temps d'execució es mes curt i més estable
  Duty = DutyMax * Sen_Wv_100[Sen_Ind];
  if (Duty > 0){
    #ifdef MOTOR_1_ACTIVE
      digitalWrite(M1_DIR, HIGH);
    #endif
    #ifdef MOTOR_2_ACTIVE
      digitalWrite(M2_DIR, HIGH);
    #endif
  }
  else{
    Duty = -Duty;   // Duty sempre ha de ser positu
    #ifdef MOTOR_1_ACTIVE
      digitalWrite(M1_DIR, LOW);
    #endif
    #ifdef MOTOR_2_ACTIVE
      digitalWrite(M2_DIR, LOW);
    #endif
  }

  #ifdef MOTOR_1_ACTIVE
    Timer1.pwm(M1_PWM, Duty);
  #endif
  #ifdef MOTOR_2_ACTIVE
    Timer1.pwm(M2_PWM, Duty);
  #endif
}

void callback_50(){
  Sen_Ind++;

  if (Sen_Ind > 50){
    Timer1.setPeriod(dt*us);
    Sen_Ind = 0;
  }

//  Duty = DutyMax * sin(w*t);    // Amb table el temps d'execució es mes curt i més estable
  Duty = DutyMax * Sen_Wv_50[Sen_Ind];
  if (Duty > 0){
    #ifdef MOTOR_1_ACTIVE
      digitalWrite(M1_DIR, HIGH);
    #endif
    #ifdef MOTOR_2_ACTIVE
      digitalWrite(M2_DIR, HIGH);
    #endif
  }
  else{
    Duty = -Duty;   // Duty sempre ha de ser positu
    #ifdef MOTOR_1_ACTIVE
      digitalWrite(M1_DIR, LOW);
    #endif
    #ifdef MOTOR_2_ACTIVE
      digitalWrite(M2_DIR, LOW);
    #endif
  }

  #ifdef MOTOR_1_ACTIVE
    Timer1.pwm(M1_PWM, Duty);  // Convertim Duty de 0 a 100 en 0 a 1023.
  #endif
  #ifdef MOTOR_2_ACTIVE
    Timer1.pwm(M2_PWM, Duty);  // Convertim Duty de 0 a 100 en 0 a 1023.
  #endif
}

void callback_32(){
  Sen_Ind++;

  if (Sen_Ind > 32){
    Timer1.setPeriod(dt*us);
    Sen_Ind = 0;
  }

//  Duty = DutyMax * sin(w*t);    // Amb table el temps d'execució es mes curt i més estable
  Duty = DutyMax * Sen_Wv_32[Sen_Ind];
  if (Duty > 0){
    #ifdef MOTOR_1_ACTIVE
      digitalWrite(M1_DIR, HIGH);
    #endif
    #ifdef MOTOR_2_ACTIVE
      digitalWrite(M2_DIR, HIGH);
    #endif
  }
  else{
    Duty = -Duty;   // Duty sempre ha de ser positu
    #ifdef MOTOR_1_ACTIVE
      digitalWrite(M1_DIR, LOW);
    #endif
    #ifdef MOTOR_2_ACTIVE
      digitalWrite(M2_DIR, LOW);
    #endif
  }

  #ifdef MOTOR_1_ACTIVE
    Timer1.pwm(M1_PWM, Duty);  // Convertim Duty de 0 a 100 en 0 a 1023.
  #endif
  #ifdef MOTOR_2_ACTIVE
    Timer1.pwm(M2_PWM, Duty);  // Convertim Duty de 0 a 100 en 0 a 1023.
  #endif
}
