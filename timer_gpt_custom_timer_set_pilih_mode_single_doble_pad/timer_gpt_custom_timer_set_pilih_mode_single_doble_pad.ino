/*
 * SCA3 Sofware update 
 * 21 Jun 2024 : new algoritma menu dan timer
 * 28 Jun 2024 : redesign pin menggunakan ardu-mega-pro
 * 29 Jun 2024 : tambah program max31856
 * 
 */

#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Adafruit_MAX31856.h>
#include "RTClib.h"

//inisiasi RTC
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Inisialisasi LCD I2C (alamat bisa bervariasi, pastikan sesuai dengan LCD Anda)
LiquidCrystal_I2C lcd1(0x27, 20, 4);
LiquidCrystal_I2C lcd2(0x25, 20, 4);

// Timer duration in seconds
volatile int timerDuration[4] = {0, 0, 0, 0}; // Initial durations for timers
volatile int timers[4] = {0, 0, 0, 0}; // Initialize timers to 0
volatile bool stateTimer[4] = {true, true, true, true}; // Initial states for timers

// Temp set poin in celcius
volatile float tempSetPoinHeatPad[4] = {0, 0, 0, 0}; // Initial durations for timers
volatile bool stateThdTempHeatPad[4] = {true, true, true, true}; // Initial states for timers
volatile float tempThd = 200.0;

// Deklarasi variabel untuk mode
bool doublePadMode = false;

unsigned long previousMillis = 0; // Store the last time millis() was updated
const long interval = 1000; // Interval to update timers (1 second)

const int startButtonPin = 3; // interrupt pin mega
volatile bool startPressed = false;

const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns

char keys[ROWS][COLS] = {
  {'1', '4', '7', '*'},
  {'2', '5', '8', '0'},
  {'3', '6', '9', '#'},
  {'A', 'B', 'C', 'D'}
};

byte rowPins[COLS] = {43, 42, 41, 40};
byte colPins[ROWS] = {39, 38, 37, 36};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Deklarasi pin SSR
const int ssrPins[] = {44, 46, 31, 30};

// Deklarasi pin tombol SSR
const int ssrBtnPins[] = {33, 32, 35, 34};

//Deklarasi pin Relay
const int relayPin[] = {26, 27, 28, 29};

//Deklarasi sinyal HM Counter
const int sinyalHMPin = 25;

// Variabel untuk threshold temperatur
int tempPad1 = 200;
int tempPad2 = 200;
int tempPad3 = 200;
int tempPad4 = 200;

// Setpoint temperatur
int setPoinTemp = 150;

// Array untuk menyimpan objek max31856
Adafruit_MAX31856 thermo[4] = {
  Adafruit_MAX31856(45),
  Adafruit_MAX31856(47),
  Adafruit_MAX31856(48),
  Adafruit_MAX31856(49)
};

// Array untuk menyimpan nilai CS pin
const int maxCSPin[4] = {45, 47, 48, 49};

// Array untuk menentukan status aktif sensor
bool maxState[4] = {true, true, true, true};

// Array untuk menyimpan suhu yang dibaca dari setiap sensor
float tempHeatPad[4] = {0.0, 0.0, 0.0, 0.0};

// Inisialisasi modul thermocouple
void setupThermo(){
//  for (int i = 0; i < 4; i++) {
//    thermocouples[i].begin();
//  }
  if (!thermo[0].begin()) {
    Serial.println("Could not initialize thermocouple 1.");
    while (1);
  }
  if (!thermo[1].begin()) {
    Serial.println("Could not initialize thermocouple 2.");
    while (1);
  }
  if (!thermo[2].begin()) {
    Serial.println("Could not initialize thermocouple 3.");
    while (1);
  }
  if (!thermo[3].begin()) {
    Serial.println("Could not initialize thermocouple 4.");
    while (1);
  }
}

void setupRTC(){
#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
}

void setupSSR() {
  // Setup pin SSR sebagai output
  for (int i = 0; i < 4; i++) {
    pinMode(ssrPins[i], OUTPUT);
    digitalWrite(ssrPins[i], LOW); // Inisialisasi SSR dalam keadaan mati
  }
}

void setupRelay() {
  // Setup pin SSR sebagai output
  for (int i = 0; i < 4; i++) {
    pinMode(relayPin[i], OUTPUT);
    digitalWrite(relayPin[i], LOW); // Inisialisasi SSR dalam keadaan mati
    delay(500);
    digitalWrite(relayPin[i], HIGH);
    //delay(500);
    //digitalWrite(relayPin[i], LOW);
  }
}

void setupBTN() {
  // Setup pin tombol SSR sebagai input dengan pull-up
  for (int i = 0; i < 4; i++) {
    pinMode(ssrBtnPins[i], INPUT_PULLUP);
  }
}

void changeState() {
  noInterrupts(); // Disable interrupts while modifying shared variables
  stateTimer[0] = true;
  timerDuration[0] = 55;
  timers[0] = 55; // Reset the timer value to 55 seconds
  interrupts(); // Re-enable interrupts
}

void setupLCD1() {
  lcd1.init();
  lcd1.backlight();
  lcd1.setCursor(5, 0);
  lcd1.print("Welcome To");
  delay(200);
  lcd1.setCursor(4, 1);
  lcd1.print("Smart Curing");
  delay(200);
  lcd1.setCursor(3, 2);
  lcd1.print("Assistance V.3");
  delay(200);
  lcd1.setCursor(1, 3);
  lcd1.print("Dept Tyre Pama-Kide");
  delay(1000);
}

void setupLCD2() {
  lcd2.init();
  lcd2.backlight();
  lcd2.setCursor(5, 0);
  lcd2.print("Welcome To");
  delay(200);
  lcd2.setCursor(4, 1);
  lcd2.print("Smart Curing");
  delay(200);
  lcd2.setCursor(3, 2);
  lcd2.print("Assistance V.3");
  delay(200);
  lcd2.setCursor(1, 3);
  lcd2.print("Dept Tyre Pama-Kide");
  delay(1000);
}

void setup() {
  Serial.begin(9600); // Start the serial communication
  SPI.begin();
  setupRTC();
  setupRelay();
  setupSSR();
  setupLCD1();
  setupLCD2();
  setupBTN();
  setupThermo();
  pinMode(startButtonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(startButtonPin), changeState, FALLING);
  selectOperation();
   
}

void calculateTimerSinglePad(){
  // Calculate timer durations from the user
  for (int i = 0; i < 4; i++) {
    timerDuration[i] = getTimerDurationFromUser(i + 1);
    timers[i] = timerDuration[i] * 300; // Initialize the timers with the given deept * 300 second || 1 mm = 300 second
  }
  // Request temp set poin from the user
  for (int i = 0; i < 4; i++) {
    tempSetPoinHeatPad[i] = getTempSetPoinFromUser(i + 1);
    //timers[i] = timerDuration[i]; // Initialize the timers with the given duration
  }
}

void calculateTimerDoublePad(){
  // Calculate timer durations from the user
  for (int i = 0; i < 2; i++) {
    timerDuration[i] = getTimerDurationFromUser(i + 1);
    timers[i] = timerDuration[i] * 300; // Initialize the timers with the given deept * 300 second || 1 mm = 300 second
  }
  // Request temp set poin from the user
  for (int i = 0; i < 2; i++) {
    tempSetPoinHeatPad[i] = getTempSetPoinFromUser(i + 1);
    //timers[i] = timerDuration[i]; // Initialize the timers with the given duration
  }
  timers[3]= 0;
  timers[4]= 0;
  tempSetPoinHeatPad[3]=0;
  tempSetPoinHeatPad[4]=0;
}

// Function to select mode (single or double pad)
void selectOperation() {
  lcd1.clear();
  lcd2.clear();
  lcd1.setCursor(0, 0);
  lcd1.print("    Select Mode:    ");
  lcd1.setCursor(0, 1);
  lcd1.print("    1: Test Unit    ");
  lcd1.setCursor(0, 2);
  lcd1.print(" 2: Curing Process ");
  
  while (true) {
    char key = keypad.getKey();
    if (key == '1') {
      doublePadMode = false; // Single pad mode
      lcd1.clear();
      lcd1.setCursor(0, 0);
      lcd1.print("       Mode :       ");
      lcd1.setCursor(0, 1);
      lcd1.print("     Test Unit      ");  
      lcd1.setCursor(0, 2);
      lcd1.print("      Selected      ");
      delay(1000);
      calculateTimerSinglePad();
      break;
    } else if (key == '2') {
      doublePadMode = true; // Double pad mode
      lcd1.clear();
      lcd1.print("       Mode :       ");
      lcd1.setCursor(0, 1);
      lcd1.print("Curing Process");  
      lcd1.setCursor(0, 2);
      lcd1.print("      Selected      "); 
      selectOperationMode(); 
      break;
    }
  }
}

// Function to select mode (single or double pad)
void selectOperationMode() {
  lcd1.clear();
  lcd1.setCursor(0, 0);
  lcd1.print("    Select Mode:    ");
  lcd1.setCursor(0, 1);
  lcd1.print("   1: Single Pad    ");
  lcd1.setCursor(0, 2);
  lcd1.print("   2: Double Pad    ");
  
  
  while (true) {
    char key = keypad.getKey();
    if (key == '1') {
      doublePadMode = false; // Single pad mode
      lcd1.clear();
      lcd1.setCursor(0, 0);
      lcd1.print("       Mode :       ");
      lcd1.setCursor(0, 1);
      lcd1.print("     Single Pad     ");  
      lcd1.setCursor(0, 2);
      lcd1.print("      Selected      ");
      delay(1000);
      calculateTimerSinglePad();
      break;
    } else if (key == '2') {
      doublePadMode = true; // Double pad mode
      lcd1.clear();
      lcd1.print("       Mode :       ");
      lcd1.setCursor(0, 1);
      lcd1.print("     Double Pad     ");  
      lcd1.setCursor(0, 2);
      lcd1.print("      Selected      "); 
      calculateTimerDoublePad();
      break;
    }
  }
}

void tampilWaktu(){
  // Tampilkan tanggal dan waktu ke LCD
  DateTime now = rtc.now();
  int jam = now.hour();
  int mnt = now.minute();
  int dtk = now.second();
  
  lcd2.setCursor(0, 0);
  lcd2.print("Date: ");
  lcd2.print(now.year(), DEC);
  lcd2.print('/');
  lcd2.print(now.month(), DEC);
  lcd2.print('/');
  lcd2.print(now.day(), DEC);

  lcd2.setCursor(0, 1);
  lcd2.print("Day: ");
  lcd2.print(daysOfTheWeek[now.dayOfTheWeek()]);

  lcd2.setCursor(0, 2);
  lcd2.print("Time: ");
  if(jam < 10){
    lcd2.print('0');
    lcd2.print(jam);
  }
  else{
    lcd2.print(jam);
  }
  lcd2.print(':');
  if(mnt < 10){
    lcd2.print('0');
    lcd2.print(mnt);
  }
  else{
    lcd2.print(mnt);
  }
  lcd2.print(':');
  if(dtk < 10){
    lcd2.print('0');
    lcd2.print(dtk);
  }
  else{
    lcd2.print(dtk);
  }
  lcd2.setCursor(0, 3);
  lcd2.print("Temp: ");
  lcd2.print(rtc.getTemperature());
  lcd2.print(" C");
}


void loop() {
  
  unsigned long currentMillis = millis();
  
  // Check if all timers are stopped
  bool allTimersStopped = true;
  for (int i = 0; i < 4; i++) {
    if (stateTimer[i]) {
      allTimersStopped = false;
      break;
    }
  }

  if (allTimersStopped) {
    lcd1.clear();
    Serial.println("All timers have stopped. Halting the program.");
    lcd1.setCursor(6, 0);
    lcd1.print("All job");
    lcd1.setCursor(1, 1);
    lcd1.print("Has been completed");
    lcd1.setCursor(4, 3);
    lcd1.print("See you next");
    
    while (true) {
      // Infinite loop to halt the program
    }
  }

  // Check if it's time to update the timers
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // Save the last time we updated the timers
    tampilWaktu();
    
    for (int i = 0; i < 4; i++) {
      if (stateTimer[i]) {
        if (timers[i] > 0) {
          timers[i]--; // Decrement the timer
          Serial.print("Timer Pad ");
          Serial.print(i + 1);
          Serial.print(": ");
          Serial.println(timers[i]);
          
          //baca thermocouple yang timernya sedang aktif
          if (maxState[i]) {
            tempHeatPad[i] = thermo[i].readThermocoupleTemperature();
            delay(50);
            // Membulatkan suhu ke satu angka desimal
            tempHeatPad[i] = floor(tempHeatPad[i] * 10) / 10.0;
          }
 
          // Update sisa timer ke LCD1
          lcd1.setCursor(0, i); // Assuming each timer gets its own row
          lcd1.print("Pad ");
          lcd1.print(i + 1);
          lcd1.print(": ");
          // Convert the timer value to minutes with one decimal place
          float minutes = timers[i] / 60.0;
          char buffer[6]; // Buffer to hold the string representation
          dtostrf(minutes, 4, 1, buffer); // Convert float to string with 1 decimal place
          lcd1.print(buffer);
          lcd1.print(" Mnts  "); // Clear any leftover characters
/*
          //update suhu heatPad ke LCD2
          lcd2.setCursor(0, i); // Assuming each timer gets its own row
          lcd2.print("T-Pad ");
          lcd2.print(i + 1);
          lcd2.print(": ");
          //lcd2.print(tempHeatPad[i]);
          lcd2.print(rtc.getTemperature());
          lcd2.print(" C"); // Clear any leftover characters
*/          

          // Set SSR pin HIGH if timer is greater than 0
          digitalWrite(ssrPins[i], HIGH);

          // Check if the timer has reached zero
          if (timers[i] == 0) {
            stateTimer[i] = false; // Set the state timer to false
            maxState[i] = false; // Set the state max31856 to false
            Serial.print("Timer Pad ");
            Serial.print(i + 1);
            Serial.println(" stop");

            // Update the LCD to indicate the timer has stopped
            lcd1.setCursor(0, i);
            lcd1.print("Pad ");
            lcd1.print(i + 1);
            lcd1.print(": stop");
            lcd1.print("        "); // Clear any leftover characters

            // Set SSR pin LOW when timer stops
            digitalWrite(ssrPins[i], LOW);
          }
        } else {
          // Set SSR pin LOW if timer is 0
          digitalWrite(ssrPins[i], LOW);
        }
      } else {
        timers[i] = 0; // Ensure the timer is set to 0
        Serial.print("Timer Pad ");
        Serial.print(i + 1);
        Serial.println(" stop");

        // Update the LCD to indicate the timer has stopped
        lcd1.setCursor(0, i);
        lcd1.print("Pad ");
        lcd1.print(i + 1);
        lcd1.print(": stop");
        lcd1.print("        "); // Clear any leftover characters

        // Set SSR pin LOW when timer stops
        digitalWrite(ssrPins[i], LOW);
      }
    }
  }
}

int getTimerDurationFromUser(int timerNumber) {
  Serial.print("Enter depth value ");
  Serial.print(timerNumber);
  Serial.println(": ");
  lcd1.clear();
  lcd1.setCursor(0, 0);
  lcd1.print("Set depth ");
  lcd1.setCursor(10, 0);
  lcd1.print(timerNumber); 
  lcd1.setCursor(12, 0);
  lcd1.print(": ");
  
  String input = "";
  int cursorPos = 0; // Start after "Line X: "
  
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '#') {
        // User finished entering the number
        lcd1.clear();
        lcd1.setCursor(0, 1);
        lcd1.print("       Depth        ");
        lcd1.setCursor(0, 2);
        lcd1.print("    Has been set    ");
        delay(550);
        break;
      } else if (key == '*') {
        // User pressed '*', delete the last character
        if (input.length() > 0) {
          input.remove(input.length() - 1); // Remove last character
          cursorPos--; // Move cursor back
          lcd1.setCursor(cursorPos, 1);
          lcd1.print(" "); // Clear the character on LCD
          lcd1.setCursor(cursorPos, 1); // Move cursor back
          if(cursorPos == 0) {
            lcd1.setCursor(cursorPos, 1);
            lcd1.print("                    "); // Clear the character on LCD
          } else {
            lcd1.setCursor(cursorPos+1, 1); // Move cursor to the next position
            lcd1.print(" Millimeter      "); // Display the digit
          }
        }
      } else if (key >= '0' && key <= '9') {
        // Append the digit to the input string
        input += key;
        Serial.print(key); // Echo the input to the serial monitor
        lcd1.setCursor(cursorPos, 1); // Move cursor to the next position
        lcd1.print(key); // Display the digit
        lcd1.setCursor(cursorPos+1, 1); // Move cursor to the next position
        lcd1.print(" Millimeter"); // Display the digit
        cursorPos++; // Move to the next cursor position
      }
    }
  }
  
  Serial.println();
  return input.toInt(); // Convert the input string to an integer
}

float getTempSetPoinFromUser(int heatPadNumber) {
  Serial.print("Enter set temp for Heat Pad ");
  Serial.print(heatPadNumber);
  Serial.println(": ");
  lcd1.clear();
  lcd1.setCursor(0, 0);
  lcd1.print("Set Temp Pad ");
  lcd1.setCursor(13, 0);
  lcd1.print(heatPadNumber); 
  lcd1.setCursor(14, 0);
  lcd1.print(": ");
  
  String input = "";
  int cursorPos = 0; // Start after "Line X: "
  
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '#') {
        // User finished entering the number
        lcd1.clear();
        lcd1.setCursor(1, 1);
        lcd1.print("Temp has been set");
        delay(250);
        break;
      } else if (key == '*') {
        // User pressed '*', delete the last character
        if (input.length() > 0) {
          input.remove(input.length() - 1); // Remove last character
          cursorPos--; // Move cursor back
          lcd1.setCursor(cursorPos, 1);
          lcd1.print(" "); // Clear the character on LCD
          lcd1.setCursor(cursorPos, 1); // Move cursor back
          if(cursorPos == 0) {
            lcd1.setCursor(cursorPos, 1);
            lcd1.print("                    "); // Clear the character on LCD
          } else {
            lcd1.setCursor(cursorPos+1, 1); // Move cursor to the next position
            lcd1.print(" Celsius          "); // Display the digit
          }
        }
      } else if ((key >= '0' && key <= '9') || key == 'D') {
        // Append the digit to the input string
        if(key == 'D'){
          key = '.';
        }
        input += key;
        Serial.print(key); // Echo the input to the serial monitor
        lcd1.setCursor(cursorPos, 1); // Move cursor to the next position
        lcd1.print(key); // Display the digit
        lcd1.setCursor(cursorPos+1, 1); // Move cursor to the next position
        lcd1.print(" Celsius"); // Display the digit
        cursorPos++; // Move to the next cursor position
      }
    }
  }
  
  Serial.println();
  return input.toFloat(); // Convert the input string to a float
}
