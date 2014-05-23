
#include "Wire.h"

#include "I2Cdev.h"
#include "VCNL4000.h"


VCNL4000 vcnl;




void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    // initialize serial communication
    Serial.begin(9600);

    // initialize device
    Serial.println("Initializing I2C devices...");
    vcnl.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(vcnl.testConnection() ? "VCNL4000 connection successful" : "VCNL4000 connection failed");


}

void loop() {
  
    uint16_t light = vcnl.getAmbientLight();
    Serial.println(light);
    uint16_t prox = vcnl.getProximity();
    Serial.println(prox);
  
    delay(100);
}
