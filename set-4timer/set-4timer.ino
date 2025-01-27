#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// Inisialisasi LCD I2C
LiquidCrystal_I2C lcd(0x20, 20, 4);

// Konfigurasi Keypad
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'7','8','9','A'},
  {'4','5','6','B'},
  {'1','2','3','C'},
  {'*','0','#','D'}
};
byte colPins[COLS] = {27, 26, 25, 24}; // pin 2,3,4,5 untuk pin kolom keypad (lihat gambar)
byte rowPins[ROWS] = {31, 30, 29, 28}; // pin 6,7,8,9 untuk pin baris keypad (lihat gambar)
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const int startButtonPin = 3; //interupt pin mega
volatile bool startPressed = false;

// Timer settings
unsigned int timers[4] = {0, 0, 0, 0}; // in seconds

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(startButtonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(startButtonPin), startTimer, FALLING);
  lcd.setCursor(0, 0);
  lcd.print("Set 4 Timers:");
  delay(2000);
  setTimers();
}

void loop() {
  if (startPressed) {
    startPressed = false;
    Serial.println("tommbol start aktif");
    runTimers();
  }
  else{
    //Serial.println("tommbol start tidak aktif");
  }
}

void setTimers() {
  for (int i = 0; i < 4; i++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Set Timer ");
    lcd.print(i + 1);
    lcd.print(": ");
    timers[i] = inputTime();
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press Start Btn");
}

unsigned int inputTime() {
  String input = "";
  char key;
  while (true) {
    key = keypad.getKey();
    if (key) {
      if (key == '#') {
        return input.toInt(); // Convert input to integer
      } else if (key >= '0' && key <= '9') {
        input += key;
        lcd.setCursor(input.length(), 1);
        lcd.print(key);
      }
    }
  }
}

void startTimer() {
  delay(50); // Debouncing delay
  startPressed = true;
}

void runTimers() {
  for (int i = 0; i < 4; i++) {
    unsigned int remainingTime = timers[i];
    while (remainingTime > 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Timer ");
      lcd.print(i + 1);
      lcd.print(": ");
      lcd.print(remainingTime);
      lcd.print(" sec");
      delay(1000); // Wait for one second
      remainingTime--;
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Timer "); Serial.print("Timer ");
    lcd.print(i + 1); Serial.print(i+1);
    lcd.print(" done!"); Serial.println(" done!");
    delay(2000); // Display done message
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("All Timers Done!");
}
