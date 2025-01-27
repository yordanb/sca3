#include <Adafruit_MAX31856.h>
#include <SPI.h>

Adafruit_MAX31856 tc1 = Adafruit_MAX31856(10); // CS pin connected to pin 10
Adafruit_MAX31856 tc2 = Adafruit_MAX31856(9); // CS pin connected to pin 9
Adafruit_MAX31856 tc3 = Adafruit_MAX31856(8); // CS pin connected to pin 8
Adafruit_MAX31856 tc4 = Adafruit_MAX31856(7); // CS pin connected to pin 7

int period = 1000;
unsigned long time_now = 0;

void setupSerial(){
  Serial.begin(115200);
}

void setupThermo(){
  tc1.begin();
  tc1.setThermocoupleType(MAX31856_TCTYPE_J);
  tc2.begin();
  tc2.setThermocoupleType(MAX31856_TCTYPE_J);
  tc3.begin();
  tc3.setThermocoupleType(MAX31856_TCTYPE_J);
  tc4.begin();
  tc4.setThermocoupleType(MAX31856_TCTYPE_J);
  delay(500);
}

void setup(){
  setupSerial();
  setupThermo();
}

void bacaSuhu() {
  Serial.println(tc1.readThermocoupleTemperature());
  delay(1000);
  Serial.print("\t");
  Serial.print(tc2.readThermocoupleTemperature());
  delay(1000);
  Serial.print("\t");
  Serial.print(tc3.readThermocoupleTemperature());
  delay(1000);
  Serial.print("\t");
  Serial.print(tc4.readThermocoupleTemperature());
  delay(1000);
  Serial.println("\t");
  //delay(50);
}

void loop(){
  time_now = millis(); 
  while(millis() < time_now + period){
      bacaSuhu();
  }
  
}
