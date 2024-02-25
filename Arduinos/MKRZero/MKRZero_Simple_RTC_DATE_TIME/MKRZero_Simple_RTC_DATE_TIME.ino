/*
  Simple RTC for Arduino Zero and MKR1000

  Demonstrates the use of the RTC library for the Arduino Zero and MKR1000

  This example code is in the public domain

  http://arduino.cc/en/Tutorial/SimpleRTC

  created by Arturo Guadalupi <a.guadalupi@arduino.cc>
  15 Jun 2015
  modified
  18 Feb 2016
  modified by Andrea Richetta <a.richetta@arduino.cc>
  24 Aug 2016
*/

#include <RTCZero.h>

/* Create an rtc object */
RTCZero rtc;

/* Change these values to set the current initial time */
int Hour = 0;
int Min = 0;
int Sec = 0;

/* Change these values to set the current initial date */
int Day = 0;
int Month = 0;
int Year = 0;

void setup()
{
  Serial.begin(9600);

  while(!Serial);

  Serial.println("Battery Cell Logger:"); 
  Serial.print("Compile Date:");
  Serial.println(__DATE__);   // "Jan 24 2019"

  // Obtenir data de compilacio
  char s_month[5];
  static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
  sscanf(__DATE__, "%s %d %d", s_month, &Day, &Year);
  Month = (strstr(month_names, s_month)-month_names)/3+1;
/*  Serial.print("Comp_Month: ");
  Serial.println(Month);
  Serial.print("Comp_Day: ");
  Serial.println(Day);
  Serial.print("Comp_Year: ");
  Serial.println(Year-2000);*/

  Serial.print("Compile Time:");
  Serial.println(__TIME__);   // "20:44:27"

  // Obtenir hora de compilacio
  sscanf(__TIME__, "%2d:%2d:%2d", &Hour, &Min, &Sec);
  /*Serial.print("Comp_hour: ");
  Serial.println(Hour);
  Serial.print("Comp_min: ");
  Serial.println(Min);
  Serial.print("Comp_sec: ");
  Serial.println(Sec);*/

  rtc.begin(); // initialize RTC

  // Set the time
  rtc.setHours(Hour);
  rtc.setMinutes(Min);
  rtc.setSeconds(Sec);

  // Set the date
  rtc.setDay(Day);
  rtc.setMonth(Month);
  rtc.setYear(Year-2000);

  // you can use also
  //rtc.setTime(hours, minutes, seconds);
  //rtc.setDate(day, month, year);
}

void loop()
{
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

void print2digits(int number) {
  if (number < 10) {
    Serial.print("0"); // print a 0 before if the number is < than 10
  }
  Serial.print(number);
}
