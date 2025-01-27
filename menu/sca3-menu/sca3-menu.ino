#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// Inisialisasi LCD I2C (alamat bisa bervariasi, pastikan sesuai dengan LCD Anda)
LiquidCrystal_I2C lcd(0x20, 20, 4);

// Konfigurasi Keypad
const byte ROWS = 4; // 4 baris
const byte COLS = 4; // 4 kolom

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {27, 26, 25, 24}; // Hubungkan ke pin baris keypad
byte colPins[COLS] = {31, 30, 29, 28}; // Hubungkan ke pin kolom keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Menu Items
String menuItems[] = {"Item 1", "Item 2", "Item 3", "Item 4"};
int menuLength = sizeof(menuItems) / sizeof(menuItems[0]);
int currentMenuIndex = 0;

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Menu System");
  delay(2000);
  lcd.clear();
  displayMenu();
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    handleKeypress(key);
  }
}

void displayMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Menu:");
  for (int i = 0; i < menuLength; i++) {
    lcd.setCursor(0, i + 1);
    if (i == currentMenuIndex) {
      lcd.print("> "); // Menunjukkan pilihan saat ini
    } else {
      lcd.print("  ");
    }
    lcd.print(menuItems[i]);
  }
}

void handleKeypress(char key) {
  switch (key) {
    case '2': // Tombol '2' untuk naik
      currentMenuIndex--;
      if (currentMenuIndex < 0) {
        currentMenuIndex = menuLength - 1;
      }
      displayMenu();
      break;
    case '8': // Tombol '8' untuk turun
      currentMenuIndex++;
      if (currentMenuIndex >= menuLength) {
        currentMenuIndex = 0;
      }
      displayMenu();
      break;
    case '5': // Tombol '5' untuk memilih
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Selected: ");
      lcd.setCursor(0, 1);
      lcd.print(menuItems[currentMenuIndex]);
      delay(2000);
      displayMenu();
      break;
    default:
      break;
  }
}
