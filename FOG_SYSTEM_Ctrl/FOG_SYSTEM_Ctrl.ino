// FOG SYSTEM Control basat en l'exemple DHTtester del la llibraría DHT sensor library.
//
// by Sebastian Jardi Estadella

// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

// Sortides Digitals
// per Bomba i Valvules 1, 2, i 3.
#define BMB_pn 6
#define V1_pn 7
#define V2_pn 8
#define V3_pn 9

unsigned int T1 = 20;
unsigned int T2 = 21;
unsigned int T3 = 22;
unsigned int T4 = 23;
#define HIST 0.5    // Histeresis de OFF per Temperatura

unsigned int HM = 85;

#include "DHT.h"

#define DHTPIN 2     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 3 (on the right) of the sensor to GROUND (if your sensor has 3 pins)
// Connect pin 4 (on the right) of the sensor to GROUND and leave the pin 3 EMPTY (if your sensor has 4 pins)
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));

  dht.begin();

  pinMode(BMB_pn, OUTPUT);
  pinMode(V1_pn, OUTPUT);
  pinMode(V2_pn, OUTPUT);
  pinMode(V3_pn, OUTPUT);
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  //if (isnan(h) || isnan(t) || isnan(f)) {
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  //float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  //float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humedad: "));
  Serial.print(h);

  Serial.print(F("%  Temperatura: "));
  Serial.print(t);
  Serial.println(F("°C "));

  if (t > T1 && h < HM) { 
    digitalWrite(BMB_pn, HIGH);
    Serial.println("BMB: ON");
  }
  if (t < T1-HIST || h > HM) { 
    digitalWrite(BMB_pn, LOW); 
    Serial.println("BMB: OFF");
  }  

  if (t > T2  && h < HM) { 
    digitalWrite(V1_pn, HIGH); 
    Serial.println("V1: ON"); 
  }
  if (t < T2-HIST || h > HM) { 
    digitalWrite(V1_pn, LOW);
    Serial.println("V1: OFF"); 
  }  

  if (t > T3  && h < HM) { 
    digitalWrite(V2_pn, HIGH); 
    Serial.println("V2: ON"); 
  }
  if (t < T3-HIST || h > HM) { 
    digitalWrite(V2_pn, LOW); 
    Serial.println("V2: OFF"); 
  }  

  if (t > T4  && h < HM) { 
    digitalWrite(V3_pn, HIGH); 
    Serial.println("V3: ON"); 
  }
  if (t < T4-HIST || h > HM) { 
    digitalWrite(V3_pn, LOW); 
    Serial.println("V3: OFF"); 
  }  
}
