#include <Wire.h>
#include <I2Cdev.h>
#include <MPL115A2.h>

MPL115A2 mpl;
boolean connection=false;

void setup() {
  Wire.begin();
  mpl.initialize();
  
  connection = mpl.testConnection();
  Serial.println(  mpl.testConnection()  ? "Connection OK" : "Connection FAILED");
}

void loop() {
  if (!connection)
    return;
//  delay(10);
  Serial.print("pressure (kPA): ");  
  Serial.print(mpl.getPressure());
  Serial.print("\t");
  Serial.print("temperature (°C): "); 
  Serial.println(mpl.getTemperature());
}
