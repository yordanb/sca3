#include <SPI.h>
#include <Adafruit_MAX31856.h>

// Deklarasi pin CS untuk masing-masing modul
const int CS_PIN_1 = 45;
const int CS_PIN_2 = 47;
const int CS_PIN_3 = 48;
const int CS_PIN_4 = 49;
//const int CS_PIN_5 = 53;

// Deklarasi objek untuk masing-masing modul
Adafruit_MAX31856 thermocouple_1(CS_PIN_1);
Adafruit_MAX31856 thermocouple_2(CS_PIN_2);
Adafruit_MAX31856 thermocouple_3(CS_PIN_3);
Adafruit_MAX31856 thermocouple_4(CS_PIN_4);
//Adafruit_MAX31856 thermocouple_5(CS_PIN_5);

void setup() {
  Serial.begin(9600);

  // Inisialisasi SPI
  SPI.begin();

  // Inisialisasi modul thermocouple
  if (!thermocouple_1.begin()) {
    Serial.println("Could not initialize thermocouple 1.");
    while (1);
  }
  if (!thermocouple_2.begin()) {
    Serial.println("Could not initialize thermocouple 2.");
    while (1);
  }
  if (!thermocouple_3.begin()) {
    Serial.println("Could not initialize thermocouple 3.");
    while (1);
  }
  if (!thermocouple_4.begin()) {
    Serial.println("Could not initialize thermocouple 4.");
    while (1);
  }
//  if (!thermocouple_5.begin()) {
//    Serial.println("Could not initialize thermocouple 5.");
//    while (1);
//  }
}

void loop() {
  // Membaca suhu dari masing-masing modul
  float temp1 = thermocouple_1.readThermocoupleTemperature(); delay(50);
  float temp2 = thermocouple_2.readThermocoupleTemperature(); delay(50);
  float temp3 = thermocouple_3.readThermocoupleTemperature(); delay(50);
  float temp4 = thermocouple_4.readThermocoupleTemperature(); delay(50);
  //float temp5 = thermocouple_5.readThermocoupleTemperature(); delay(50);

  // Membulatkan suhu ke satu angka desimal
  temp1 = floor(temp1 * 10) / 10.0;
  temp2 = floor(temp2 * 10) / 10.0;
  temp3 = floor(temp3 * 10) / 10.0;
  temp4 = floor(temp4 * 10) / 10.0;
  //temp5 = floor(temp5 * 10) / 10.0;

  // Menampilkan suhu di Serial Monitor
  Serial.print("Thermocouple 1: ");
  Serial.println(temp1);
  Serial.print("Thermocouple 2: ");
  Serial.println(temp2);
  Serial.print("Thermocouple 3: ");
  Serial.println(temp3);
  Serial.print("Thermocouple 4: ");
  Serial.println(temp4);
  //Serial.print("Thermocouple 5: ");
  //Serial.println(temp5);

  delay(500);
}
