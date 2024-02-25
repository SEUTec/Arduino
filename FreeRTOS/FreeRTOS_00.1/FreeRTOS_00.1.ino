#include <Arduino_FreeRTOS.h>

// 1. Print serial monitor with 2 task
// 2. Control led an print to serial monitor at the same time, without FreeRTOS - FreeRTOS

void Task_ControlLED(void *param);
void Task_Print(void *param);

TaskHandle_t Task_Handle1;
TaskHandle_t Task_Handle2;

#define Ctrlpin 2
#define LEDpin 13

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LEDpin, OUTPUT);

  // La tasca de prioritat més elevada d'executa primer
  xTaskCreate(Task_ControlLED, "Task1", 100, NULL, 1, &Task_Handle1);
  xTaskCreate(Task_Print, "Task2", 100, NULL, 2, &Task_Handle2);
}

void loop() {
  // DO NOT put your main code here, to run repeatedly.
}

void Task_ControlLED(void *param){
  (void) param;

  while(1){
    if (digitalRead(Ctrlpin) == LOW ){
      digitalWrite(LEDpin, LOW);
    }
    else{ digitalWrite(LEDpin, HIGH); }
    vTaskDelay(1000/portTICK_PERIOD_MS);   // Print to serial monitor each 1s.
  } 
}

void Task_Print(void *param){
  (void) param;

  while(1){
    Serial.println("Task Print");
    vTaskDelay(1000/portTICK_PERIOD_MS);   // Print to serial monitor each 1s.
  }
}
