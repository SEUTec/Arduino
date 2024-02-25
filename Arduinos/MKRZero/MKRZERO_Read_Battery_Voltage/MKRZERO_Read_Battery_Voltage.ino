/*

This code is based on the readAnalogVoltage tutorial. For a deeper explanation about the code in this tutorial please refer to it.
  
  ReadBatteryVoltage
  Reads the analog input connected to the battery output on a MKRZero or MKR1000, converts it to voltage, and prints the result to the serial monitor.
  Graphical representation is available using serial plotter (Tools > Serial Plotter menu)

  This example code is in the public domain.
*/

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(ADC_BATTERY);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 4.3V):
  float voltage = sensorValue * (4.3 / 1023.0);
  // print out the value you read:
  Serial.print(voltage);
  Serial.println("V");
}