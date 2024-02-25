/*
 Water Conductivity Monitor
 Sketch for an Arduino gadget that measures the electrical
 conductivity of water.
*/
const float ArduinoVoltage = 5.00; // CHANGE THIS FOR 3.3v Arduinos
const float ArduinoResolution = ArduinoVoltage / 1024;
const float resistorValue = 10000.0;
int threshold = 3;
int inputPin = A0;
int ouputPin = A5;
float temp; 
int tempPin=A2;

void setup(){
  Serial.begin(9600);
  pinMode(ouputPin, OUTPUT);
  pinMode(inputPin, INPUT);
  pinMode(tempPin,INPUT);
}

void loop(){
  int analogValue=0;
  int oldAnalogValue=1000;
  float returnVoltage=0.0;
  float resistance=0.0;
  double Siemens;
  float TDS=0.0;

  temp=analogRead(tempPin);
  temp=temp*0.48828125;

  Serial.print("Temperature:");
  Serial.print(temp);
  Serial.print("'C");
  Serial.println();
  delay(500);

  while(((oldAnalogValue-analogValue)>threshold) || (oldAnalogValue<50)){
    oldAnalogValue = analogValue;
    digitalWrite( ouputPin, HIGH );
    delay(10); // allow ringing to stop, delay in ms
    analogValue = analogRead( inputPin );
    digitalWrite( ouputPin, LOW );
  }
  Serial.print("Return voltage = ");
  returnVoltage = analogValue *ArduinoResolution;
  Serial.print(returnVoltage);

  Serial.println(" volts");
  Serial.print("That works out to a resistance of ");
  resistance = ((5.00 * resistorValue) / returnVoltage) - resistorValue;
  Serial.print(resistance);
  Serial.println(" Ohms.");

  Serial.print("Which works out to a conductivity of ");
  Siemens = 1.0/(resistance/1000000);
  if(150<=Siemens & Siemens<=800)
    Siemens=Siemens+120;
  else if(801<=Siemens |  Siemens>=1900)
  Siemens=Siemens+300;
  Serial.print(Siemens);
  Serial.println(" microSiemens.");
  Serial.print("Total Dissolved Solids are on the order of ");
  TDS = 500 * (Siemens/1000);
  Serial.print(TDS);
  Serial.println(" PPM.");

  if (returnVoltage>4.9) 
    Serial.println("Are you sure this isn't metal?");
  Serial.println("");
  
  delay(2000);
}
