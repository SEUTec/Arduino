/*
 * Tasques de duració variable amb periode d'execució constant.
 * 
 */

#define TEMPS_ADQ 0   // Per a mesurar (o no) el temps d'adquisició de dades.
#define HWM 0         // Per realitzar (o no) el Test High Water Mark, Stack Size.
#define HWM2 0        // Per realitzar (o no) el Test amb 2 High Water Mark, per
                      // determinar l'espai que ocupa el 1 Test HWM.

#include <Arduino_FreeRTOS.h>

// define two tasks for Blink & AnalogRead
void TaskBlink( void *pvParameters );
void TaskAnalogRead( void *pvParameters );

TaskHandle_t TaskHandle_Blink;
TaskHandle_t TaskHandle_AnalogRead;

// the setup function runs once when you press reset or power the board
void setup() {
  
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  }

  // Now set up two tasks to run independently.
  xTaskCreate(
    TaskBlink
    ,  "Blink"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL  // NULL si no pasem parametres a la funcio Tasca
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  &TaskHandle_Blink );

  xTaskCreate(
    TaskAnalogRead
    ,  "AnalogRead"
    ,  200  // Stack size
    ,  NULL
    ,  3  // Priority
    ,  &TaskHandle_AnalogRead );

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop(){
  // Empty. Things are done in Tasks.
}

/*---------------------- Tasks ---------------------*/

// Testeix HighWaterMark
// Per determinar la quantitat de memoria lliure quan s'executa
// una determinada tasca, 
// quan cambia la quantitat de memoria lliure envia el dato al port serie
void TestHwm(char * taskName){

  static int stack_hwm, stack_hwm_temp;

  stack_hwm_temp = uxTaskGetStackHighWaterMark(nullptr);
  if ( !stack_hwm || (stack_hwm_temp < stack_hwm) ) {
    stack_hwm = stack_hwm_temp;
    Serial.println(String(taskName) + " tiene stack hwm " + String(stack_hwm)); 
    // Restant l'stack size definit al crear la tasca, dels que ens sobren,
    // obtindrem el valor que tindrem de definir, per ajustar-nos millor
    // al valor que necesitem.
    // Al valor definit li tenim de restar un valor inferior dels que ens
    // sobre per a que no en sobrin tants, pero millo que en sobrin.
    // Si no en sobren o en falten tindrem problemes de falta de memoria,
    // i l'ESP32 donará error y RESETEIX continuo.
  }
}

/* Copia exacta de la funcio anterior per poder saber quanta memoria ocupa */
void TestHwm_Copi(char * taskName){

  static int stack_hwm, stack_hwm_temp;

  stack_hwm_temp = uxTaskGetStackHighWaterMark(nullptr);
  if ( !stack_hwm || (stack_hwm_temp < stack_hwm) ) {
    stack_hwm = stack_hwm_temp;
    Serial.println(String(taskName) + " tiene stack hwm " + String(stack_hwm)); 
    // Restant l'stack size definit al crear la tasca, dels que ens sobren,
    // obtindrem el valor que tindrem de definir, per ajustar-nos millor
    // al valor que necesitem.
    // Al valor definit li tenim de restar un valor inferior dels que ens
    // sobre per a que no en sobrin tants, pero millo que en sobrin.
    // Si no en sobren o en falten tindrem problemes de falta de memoria,
    // i l'ESP32 donará error y RESETEIX continuo.
  }
}

void TaskBlink(void *pvParameters) {

  (void) pvParameters;

  // initialize digital LED_BUILTIN on pin 13 as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Blink PWM setup
  int PWM_Duty = 50;   // Duty Cycle en %, 0 a 100
  unsigned int PWM_Prd = 5000;  // Periode PWM en ms

  TickType_t xLastWakeTime;
  TickType_t xPeriod = pdMS_TO_TICKS(PWM_Prd);  // Periode de repeticio tasca en ticks

/* Pointer to a variable that holds the time at which the task was last unblocked. 
 * The variable must be initialised with the current time prior to its first use. 
 * Following this the variable is automatically updated within vTaskDelayUntil(). 
 */
  xLastWakeTime = xTaskGetTickCount();

  for (;;){ // A Task shall never return or exit.
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    vTaskDelay(xPeriod * PWM_Duty / 100);
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    vTaskDelayUntil(&xLastWakeTime, xPeriod);
/*
 * The cycle time period, xPeriod. The task will be unblocked at time 
 * (*pxPreviousWakeTime + xTimeIncrement). Calling vTaskDelayUntil with the same xTimeIncrement 
 * parameter value will cause the task to execute with a fixed interval period.
 */

  }
}

void TaskAnalogRead(void *pvParameters){  // This is a task.

  (void) pvParameters;

  pinMode(A0, INPUT);  // En principi no es necessari,
                       // si avans no l'hem configurat com a sortida digital 
                       
  // Mostres Analògiques
  #define NumMostres 5
  int Mostres[NumMostres];
  int Mostres_Ndx=0;
  unsigned int Mostres_Prd = 500;  // Periode de Mostreix en ms, ha de ser superior al temps
                                  // mínim de lectura d'una dada, i deixar temps executar
                                  // les altres tasques.

  TickType_t xLastWakeTime;
  TickType_t xPeriod = pdMS_TO_TICKS(Mostres_Prd);  // Periode de repeticio tasca en ticks

/* Pointer to a variable that holds the time at which the task was last unblocked. 
 * The variable must be initialised with the current time prior to its first use. 
 * Following this the variable is automatically updated within vTaskDelayUntil(). 
 */
  xLastWakeTime = xTaskGetTickCount();
  
/*
  AnalogReadSerial
  Reads an analog input on pin 0, prints the result to the serial monitor.
  Graphical representation is available using serial plotter (Tools > Serial Plotter menu)
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.
*/
  for (;;) {
    // read the input on analog pin 0:

#if TEMPS_ADQ
    int tini = micros();  // TEST temps
#endif

    Mostres[Mostres_Ndx] = analogRead(A0);
    Mostres_Ndx++;

#if TEMPS_ADQ
    int tfi = micros();
    int t = tfi - tini;
    Serial.println("Temps Adquisició: " + String(t));
#endif

    // print out the values you read:
    if (Mostres_Ndx == NumMostres-1) {
      Mostres_Ndx = 0;
      for (int i = 0; i < NumMostres; i++){
        Serial.println(Mostres[i]);  // No cal semafor,
                                     // si només esta tasca utilitza el port serie.
      }
      Serial.println(" ");
    }
    vTaskDelayUntil(&xLastWakeTime, xPeriod);
    
/*
 * The cycle time period, xPeriod. The task will be unblocked at time 
 * (*pxPreviousWakeTime + xTimeIncrement). Calling vTaskDelayUntil with the same xTimeIncrement 
 * parameter value will cause the task to execute with a fixed interval period.
 */ 
#if HWM2
    TestHwm_Copi("TaskAnalogRead");   // Passem la etiqueta de la Tasca.
#endif
#if HWM    
    TestHwm("TaskAnalogRead");   // Passem la etiqueta de la Tasca.
                           // Ens ajuda a determinar el StackSize.
                           // Un cop testejat, podem comentar esta 
                           // funcio de TestHwm, i treureli uns  bytes
                           // al Stack Size. 

                           // StackSize 128 bytes
                           // Amb 1 Hwm sobra 26 bytes
                           // Amb 2 Hwm sobra 5 bytes
                           // Hwm StackSize = 26 - 5 =  21 bytes
                           // StackSize es pot ajustar
                           // StackSize - Sobra amb 1 Hwm - Hwm StackSize
                           // 128 - 26 - 21 = 81 => 90 o 100 bytes
                           // sempre que no afegim molt the codi ni les funcions de TestHwm.
#endif   
    }
}
