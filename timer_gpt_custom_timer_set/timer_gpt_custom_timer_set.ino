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

// Inisialisasi LCD I2C (alamat bisa bervariasi, pastikan sesuai dengan LCD Anda)
LiquidCrystal_I2C lcd1(0x27, 20, 4);
LiquidCrystal_I2C lcd2(0x20, 20, 4);

// Timer duration in seconds
volatile int timerDuration[4] = {0, 0, 0, 0}; // Initial durations for timers
volatile int timers[4] = {0, 0, 0, 0}; // Initialize timers to 0
volatile bool stateTimer[4] = {true, true, true, true}; // Initial states for timers

// Temp set poin in celcius
volatile float tempSetPoinHeatPad[4] = {0, 0, 0, 0}; // Initial durations for timers
volatile bool stateThdTempHeatPad[4] = {true, true, true, true}; // Initial states for timers
volatile float tempThd = 200.0;

unsigned long previousMillis = 0; // Store the last time millis() was updated
const long interval = 1000; // Interval to update timers (1 second)

const int startButtonPin = 3; // interrupt pin mega
volatile bool startPressed = false;

const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns
/*
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
*/

char keys[ROWS][COLS] = {
  {'1', '4', '7', '*'},
  {'2', '5', '8', '0'},
  {'3', '6', '9', '#'},
  {'A', 'B', 'C', 'D'}
};

byte rowPins[COLS] = {43, 42, 41, 40};
//byte rowPins[COLS] = {41, 41, 42, 43};
byte colPins[ROWS] = {39, 38, 37, 36};
//byte colPins[ROWS] = {36, 37, 38, 39};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Deklarasi pin SSR
const int ssrPins[] = {44, 46, 31, 30};

// Deklarasi pin tombol SSR
const int ssrBtnPins[] = {33, 32, 35, 34};

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

void setupSSR() {
  // Setup pin SSR sebagai output
  for (int i = 0; i < 4; i++) {
    pinMode(ssrPins[i], OUTPUT);
    digitalWrite(ssrPins[i], LOW); // Inisialisasi SSR dalam keadaan mati
  }
}

void setupBTN() {
  // Setup pin tombol SSR sebagai input dengan pull-up
  for (int i = 0; i < 4; i++) {
    pinMode(ssrBtnPins[i], INPUT_PULLUP);
  }
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
  setupSSR();
  setupLCD1();
  setupLCD2();
  setupBTN();
  setupThermo();
  pinMode(startButtonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(startButtonPin), changeState, FALLING);
  // Request timer durations from the user
  for (int i = 0; i < 4; i++) {
    timerDuration[i] = getTimerDurationFromUser(i + 1);
    timers[i] = timerDuration[i]; // Initialize the timers with the given duration
  }
  // Request temp set poin from the user
  for (int i = 0; i < 4; i++) {
    tempSetPoinHeatPad[i] = getTempSetPoinFromUser(i + 1);
    //timers[i] = timerDuration[i]; // Initialize the timers with the given duration
  }
}

void changeState() {
  noInterrupts(); // Disable interrupts while modifying shared variables
  stateTimer[0] = true;
  timerDuration[0] = 55;
  timers[0] = 55; // Reset the timer value to 55 seconds
  interrupts(); // Re-enable interrupts
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
    
    for (int i = 0; i < 4; i++) {
      if (stateTimer[i]) {
        if (timers[i] > 0) {
          timers[i]--; // Decrement the timer
          Serial.print("Timer Line ");
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
          lcd1.print("Line ");
          lcd1.print(i + 1);
          lcd1.print(": ");
          lcd1.print(timers[i]);
          lcd1.print("         "); // Clear any leftover characters

          //update suhu heatPad ke LCD2
          lcd2.setCursor(0, i); // Assuming each timer gets its own row
          lcd2.print("HeatPad ");
          lcd2.print(i + 1);
          lcd2.print(": ");
          lcd2.print(tempHeatPad[i]);
          lcd2.print("         "); // Clear any leftover characters
          

          // Set SSR pin HIGH if timer is greater than 0
          digitalWrite(ssrPins[i], HIGH);

          // Check if the timer has reached zero
          if (timers[i] == 0) {
            stateTimer[i] = false; // Set the state timer to false
            maxState[i] = false; // Set the state max31856 to false
            Serial.print("Timer Line ");
            Serial.print(i + 1);
            Serial.println(" stop");

            // Update the LCD to indicate the timer has stopped
            lcd1.setCursor(0, i);
            lcd1.print("Line ");
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
        Serial.print("Timer Line ");
        Serial.print(i + 1);
        Serial.println(" stop");

        // Update the LCD to indicate the timer has stopped
        lcd1.setCursor(0, i);
        lcd1.print("Line ");
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
  Serial.print("Enter duration for Timer ");
  Serial.print(timerNumber);
  Serial.println(": ");
  lcd1.clear();
  lcd1.setCursor(0, 0);
  lcd1.print("Set Timer Line ");
  lcd1.setCursor(15, 0);
  lcd1.print(timerNumber); 
  lcd1.setCursor(17, 0);
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
        lcd1.print("Timer has been set");
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
            lcd1.print(" Minutes       "); // Display the digit
          }
        }
      } else if (key >= '0' && key <= '9') {
        // Append the digit to the input string
        input += key;
        Serial.print(key); // Echo the input to the serial monitor
        lcd1.setCursor(cursorPos, 1); // Move cursor to the next position
        lcd1.print(key); // Display the digit
        lcd1.setCursor(cursorPos+1, 1); // Move cursor to the next position
        lcd1.print(" Minutes"); // Display the digit
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
