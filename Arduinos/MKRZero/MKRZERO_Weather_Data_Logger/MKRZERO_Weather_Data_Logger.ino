/*
  Scheduled Datalogger

  Demonstrates how to log temperature and humidity sensor
  reading and time using a MKRZero, then you can plot the
  graph of the temperature and humidity in a romm during 
  the whole day!

  This example code is in the public domain

  Parts required:
  1 DHT22 temperature and humidity sensor
  1 MKR protoshield

  created by Arturo Guadalupi <a.guadalupi@arduino.cc>
  3 Nov 2016
*/
#include <SD.h>
#include <RTCZero.h>
#include <DHT.h>

#define DHTPIN 7
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

const int chipSelect = SS1;
unsigned long previousTime;

int loadDataCheck;  //Checks if data needs to be loaded 

RTCZero rtc;

/* Change these values to set the current initial time */
const byte seconds = 50;
const byte minutes = 44;
const byte hours = 17;

/* Change these values to set the current initial date */
const byte day = 1;
const byte month = 9;
const byte year = 16;

// the setup function runs once when you press reset
// or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);    
  
  Serial.println("DataLogger Example:"); 
  
   // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
    
  }  
  Serial.println("card initialized.");

  //When power is supplied to the DHT22 sensor,
  //don't send any instruction to the sensor
  //within one second to pass unstable status
  delay(1000);
  Serial.println("Initializing DHT");
  dht.begin();
  
  Serial.println("Initializing RTC");
  rtc.begin();
  rtc.setTime(hours, minutes, seconds);
  rtc.setDate(day, month, year);

  rtc.setAlarmTime(0, 0, 0);
  rtc.enableAlarm(rtc.MATCH_SS); //alarm attached every minute
                                 //salta cada cop que els segons es 0.

  rtc.attachInterrupt(dataCheck);

  loadDataCheck=0;

  previousTime=millis();
  
  Serial.println("System ready...");  
}

// the loop function runs over and over again forever
void loop() {
  unsigned long currentTime=millis();
  if ((currentTime-previousTime)>5000)
  {
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on 
    delay(100);                       // wait for a bit
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED off 
    previousTime=millis();
  }                       
  if (loadDataCheck) logData();  
}

void dataCheck(){
  loadDataCheck=1;
}

void logData(void) {     
  float humidity = dht.readHumidity();  
  float temperature = dht.readTemperature();
  
  String dataString = "";
  dataString += "Temperature: " + String(temperature) + " C" + "\t" + "Humidity: " + String(humidity) + "%\t" + "Time: " + getTime();  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the Serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
  loadDataCheck=0;
}

String getTime(void) {
  String returnString = "";

  if (rtc.getHours() < 10)
    returnString += "0" + String(rtc.getHours());
  else
    returnString += String(rtc.getHours());

  returnString += ":";

  if (rtc.getMinutes() < 10)
    returnString += "0" + String(rtc.getMinutes());
  else
    returnString += String(rtc.getMinutes());

  returnString += ":";

  if (rtc.getSeconds() < 10)
    returnString += "0" + String(rtc.getSeconds());
  else
    returnString += String(rtc.getSeconds());

  return returnString;
}
