/* Com utilitzar FreeRTOS per programar diferents tasques
 *  Amb diferents periodes constants d'execució.
 *  
 *  Tenim d'utilitzar els semaforos per evitar que més d'una tasca utilitze el port serie.
 *  
 *  by Sebastián Jardi Estadella
 */

#include <Arduino_FreeRTOS.h>
#include <semphr.h>  // add the FreeRTOS functions for Semaphores (or Flags).

// Declare a mutex Semaphore Handle which we will use to manage the Serial Port.
// It will be used to ensure only one Task is accessing this resource at any time.
SemaphoreHandle_t xSerialSemaphore;

#define PIN0 8
#define PIN1 9
#define PIN2 10
#define PIN3 11

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  }
  Serial.println("FreeRTOS test.");
  Serial.print("Prioritat Máxima: ");
  Serial.println(configMAX_PRIORITIES - 1);

  // Semaphores are useful to stop a Task proceeding, where it should be paused to wait,
  // because it is sharing a resource, such as the Serial port.
  // Semaphores should only be used whilst the scheduler is running, but we can set it up here.
  if ( xSerialSemaphore == NULL ){  // Check to confirm that the Serial Semaphore has not already been created.
    xSerialSemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore we will use to manage the Serial Port
    if ( ( xSerialSemaphore ) != NULL )
      xSemaphoreGive( ( xSerialSemaphore ) );  // Make the Serial Port available for use, by "Giving" the Semaphore.
  }

  // Creació de tasques y asignació de prioritats.
  // BaseType_t xTaskCreate(    TaskFunction_t pvTaskCode,  // Nom funcio implementa la tasca
  //                            const char * const pcName,
  //                            configSTACK_DEPTH_TYPE usStackDepth,
  //                            void *pvParameters,
  //                            UBaseType_t uxPriority,
  //                            TaskHandle_t *pxCreatedTask);
  xTaskCreate(f_Tsc0, "Tasca 0", 128, NULL, 0, NULL);  // Menys prioritaria, Idle, Desocupado.
  xTaskCreate(f_Tsc1, "Tasca 1", 128, NULL, 1, NULL);  
  xTaskCreate(f_Tsc2, "Tasca 2", 128, NULL, 2, NULL);  
  //xTaskCreate(f_Tsc3, "Tasca 3", 128, NULL, 3, NULL);  // Més prioritaria
                                                          // Máxima es configMAX_PRIORITIES - 1 = 3

  // Configuració dels pins
  pinMode(PIN0, OUTPUT);
  pinMode(PIN1, OUTPUT);
  pinMode(PIN2, OUTPUT);
  pinMode(PIN3, OUTPUT);
}

void loop() {
  // En principi aquí no fiquem res.
  // Però podríem ficar la tasca menys prioritaria.
  // la Idle task que es mantindrà en execució fins que es desbloqueje
  // alguna de les més prioritaries, que són totes.

  // Posiblement una tasca amb prioritat 0 te més prioritat que lo que fiquem aquí,
  // que serà realement OS or Idle, que s' executa quan totes les tasques estan bloquejades,
  // per un periode de temps determinat mitjançant vTaskDelay( ticks );
}

// Tasca menys prioritaria
static void f_Tsc0(void* pvParameters){
  unsigned int Te = 2000; // Periode d'execució d'esta tasca en ms
  unsigned int tespera = 0;  // no es pot declarar dintre d'un if.
  for (;;){  // Bucle infinit
    unsigned long tini = millis();
    digitalWrite(PIN0, HIGH);

    // See if we can obtain or "Take" the Serial Semaphore.
    // If the semaphore is not available, wait 5 ticks of the Scheduler to see if it becomes free.
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      // We were able to obtain or "Take" the semaphore and can now access the shared resource.
      // We want to have the Serial Port for us alone, as it takes some time to print,
      // so we don't want it getting stolen during the middle of a conversion.
      // print out the value you read:
      Serial.println("TaskIdle");

      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }

    delay(Te/5);
    //Serial.println(Te/10);  // perquè es necesari per a que s'execute esta tasca ??
    //Serial.println("??"); // si amb això no funciona
    digitalWrite(PIN0, LOW);
    unsigned long tfi = millis();
    if (tfi > tini) {  // This number will overflow (back to zero), after 50 days aprox.
                       // Es del tipo "unsigned long". 
      tespera = Te - (tfi - tini);
    }
    delay(tespera);                    // Esta tasca no entra en periode de bloqueix
                                  // Es queda es espera 50 ms.
  }
}

static void f_Tsc1(void* pvParameters){
  unsigned int Te = 3000;  // Periode d'execució de esta tasca en ms
  unsigned int tespera = 0;
  for(;;){  // Bucle infinit, alternativa for(;;)
    unsigned long tini = millis();
    digitalWrite(PIN1, HIGH);

    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )   {
      Serial.println("Task1");
      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }
    delay(Te/10);               // Temps d'execució simulat d esta tasca.
    digitalWrite(PIN1, LOW);
    unsigned long tfi = millis();
    if ( tfi > tini){
      unsigned int tespera = Te - (tfi - tini);
    }
    vTaskDelay(tespera/portTICK_PERIOD_MS);  // Periode de bloqueix d esta tasca en ticks
                                         // Periode que es manté autorada, permetent
                                         // l execució de tasques menys prioritaries.
                                         // 1000 / portTICK_PERIOD_MS = 1 second
                                         // Esta tasca s'executa cada 100 ms
  }
}

static void f_Tsc2(void* pvParameters){
  unsigned int Te = 4000;  // Periode d'execució de esta tasca en ms
  unsigned int tespera = 0;
  for(;;){  // Bucle infinit, alternativa for(;;)
    unsigned long tini = millis();
    digitalWrite(PIN2, HIGH);

    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ) {
      Serial.println("Task2");
      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }

    delay(Te/10);
    digitalWrite(PIN2, LOW);
    unsigned long tfi = millis();
    if ( tfi > tini ) {
      unsigned int tespera = Te - (tfi - tini);
    }
    vTaskDelay(tespera/portTICK_PERIOD_MS);  // Periode de bloqueix d esta tasca en ticks
                                         // Periode que es manté autorada, permetent
                                         // l execució de tasques menys prioritaries.
                                         // 1000 / portTICK_PERIOD_MS = 1 second
                                         // Esta tasca s'executa cada 100 ms
  }
}

static void f_Tsc3(void* pvParameters){
  unsigned int Te = 5000;  // Periode d'execució d'esta tasca en ms
  unsigned int tespera = 0;
  for(;;){  // Bucle infinit
    unsigned long tini = millis();
    digitalWrite(PIN3, HIGH);

    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )  {
      Serial.println("Task3");
      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }
    delay(Te/10);     // Aquí es on s'insertaría el codi que desenvolupa la Tasca3
                      // la resta del codi es per controlar i mesurar el temps

    digitalWrite(PIN3, LOW);
    unsigned long tfi = millis();
    if (tfi > tini){
      tespera = Te - (tfi - tini);
    }
    vTaskDelay(tespera/portTICK_PERIOD_MS);  // Periode de bloqueix d esta tasca en ticks
                                         // Periode que es manté aturada, permetent
                                         // l execució de tasques menys prioritaries ??????
                                         // 1000 / portTICK_PERIOD_MS = 1 second
                                         // Esta tasca s'executa cada 100 ms
  }
}
