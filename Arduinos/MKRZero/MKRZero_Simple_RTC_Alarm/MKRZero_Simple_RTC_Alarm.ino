/*
  Simple RTC Alarm for Arduino Zero and MKR1000

  Demonstrates how to set an RTC alarm for the Arduino Zero and MKR1000

  This example code is in the public domain

  http://arduino.cc/en/Tutorial/SimpleRTCAlarm

  created by Arturo Guadalupi <a.guadalupi@arduino.cc>
  25 Sept 2015
  
  modified
  21 Oct 2015

*/
// Configura un temps inicial 0h 0min 0s
// Configura la alarma per al cap dunes hores minuts i/o segons
// Quan l'RTC arriva a l'hora de la Alarma executa la funcio 
//   especificada amb rtc.attachedInterrupt(Funcio)

#include <RTCZero.h>

/* Create an rtc object */
RTCZero rtc;

/* Change these values to set the current initial time */
const byte hours = 0;
const byte minutes = 0;
const byte seconds = 0;

/* Change these values to set the current initial date */
const byte day = 1;
const byte month = 1;
const byte year = 19;

byte Retard_Alarma_h = 0;
byte Retard_Alarma_min = 0;
byte Retard_Alarma_s = 10;
byte Alarm_hour = hours + Retard_Alarma_h;
byte Alarm_minute = minutes + Retard_Alarma_min;
byte Alarm_second = seconds + Retard_Alarma_s;

void setup(){
  Serial.begin(9600);

  rtc.begin(); // initialize RTC 24H format

  rtc.setTime(hours, minutes, seconds);
  rtc.setDate(day, month, year);

  rtc.setAlarmTime(Alarm_hour, Alarm_minute, Alarm_second);
  //rtc.enableAlarm(rtc.MATCH_OFF); // No salta la alarma
  rtc.enableAlarm(rtc.MATCH_SS);    // alarm cada cop que arriva al segon especificat
                                    // primer cop alta al Alarm_second,
                                    // y cada cop que tornarà a apareixer, a cada minut.
  //rtc.enableAlarm(rtc.MATCH_MMSS); // Salta al minut segon especificat,
                                     // y cada hora es que es torna a repetir.
  //rtc.enableAlarm(rtc.MATCH_HHMMSS); // Salta cada hora minut segon especificat,
                                      // es a dir 1 cops al día.
  
  //rtc.attachInterrupt(alarmMatch);
  rtc.attachInterrupt(alarmMatch2);
}

void loop(){
  // Print date...
  print2digits(rtc.getDay());
  Serial.print("/");
  print2digits(rtc.getMonth());
  Serial.print("/");
  print2digits(rtc.getYear());
  Serial.print(" ");

  // ...and time
  print2digits(rtc.getHours());
  Serial.print(":");
  print2digits(rtc.getMinutes());
  Serial.print(":");
  print2digits(rtc.getSeconds());

  Serial.println();

  delay(1000);
}

// Funcio a Executar quan salta quan RTC arriva al AlarmTime
void alarmMatch(){
  Serial.println("Alarm Match!");
}

// Actualitza la alarma per reactivarse
void alarmMatch2(){
  Serial.println("Alarm Match!");

  byte Hora_Actual = rtc.getHours();
  byte Minut_Actual = rtc.getMinutes();
  byte Segon_Actual = rtc.getSeconds();

  Alarm_hour = Hora_Actual + Retard_Alarma_h;
  Alarm_minute = Minut_Actual + Retard_Alarma_min;
  if (Alarm_second < 60) {
    Alarm_second = Segon_Actual + Retard_Alarma_s - 1;
  }
  else { 
    Retard_Alarma_min++;
    Retard_Alarma_s = Segon_Actual - 60 + Retard_Alarma_s - 1;
  }

  rtc.setAlarmTime( Alarm_hour, Alarm_minute, Alarm_second);
}

void print2digits(int number) {
  if (number < 10) {
    Serial.print("0"); // print a 0 before if the number is < than 10
  }
  Serial.print(number);
}
