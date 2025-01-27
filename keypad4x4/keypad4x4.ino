#include <Keypad.h>
#include <Wire.h>              // Memanggil library wire.h
#include <LiquidCrystal_I2C.h> // Memanggil library LCD dengan I2C
LiquidCrystal_I2C lcd(0x20, 20, 4); // Alamat I2C(0x27) LCD dan Jenis LCD (20x4)
const byte ROWS = 4; //4 baris
const byte COLS = 4; //4 kolom
char keys[ROWS][COLS] = {
  {'7','8','9','A'},
  {'4','5','6','B'},
  {'1','2','3','C'},
  {'*','0','#','D'}
};
byte colPins[COLS] = {27, 26, 25, 24}; // pin 2,3,4,5 untuk pin kolom keypad (lihat gambar)
byte rowPins[ROWS] = {31, 30, 29, 28}; // pin 6,7,8,9 untuk pin baris keypad (lihat gambar)
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
char stringAngka[17];
int indexKeypad = 0;
void setup(){
  Serial.begin(9600);
  lcd.init();                     // instruksi untuk memulai LCD
  lcd.begin(20,4);                // instruksi untuk menggunakan lcd 20x4
  lcd.backlight();                // instruksi untuk mengaktifkan lampu latar LCD
  lcd.setCursor(4,0);             // karakter tampil pada kolom 5 baris 1 LCD 1602
  lcd.print("Tutorial");          // karakter tampil pada kolom 0 baris 2 LCD 1602
  lcd.setCursor(0,1);             //karakter tampil pada kolom 5 baris 1 LCD 1602
  lcd.print("Keypad 4x4 & LCD");  //karakter tampil pada kolom 0 baris 2 LCD 1602
  delay(2000);
  lcd.clear();
}
void loop(){
   lcd.setCursor(4,0);       //karakter tampil pada kolom 5 baris 1 LCD 1602
   lcd.print("Keypad LCD");  //karakter tampil pada kolom 0 baris 2 LCD 1602
   lcd.setCursor(0,1);       //karakter tampil pada kolom 5 baris 1 LCD 1602
   lcd.print("Tombol keypad: ");  //karakter tampil pada kolom 0 baris 2 LCD 1602
   
   char key = keypad.getKey();
   if (key) {
    Serial.println(key);
    switch (key)
    {
      case '0':                    // Instruksi untuk menampilkan karakter pada LCD dan Serial monitor
      case '1':                   
      case '2':                     
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case '#':
        if (!indexKeypad)
        {
          lcd.clear();
        }
        stringAngka[indexKeypad++] = key;
        lcd.setCursor(15,1);
        lcd.println(key);
        break;
      case '*':                     //Tombol * Keypad digunakan sebagai reset
        lcd.clear();
        indexKeypad = 0;
        break;
    }
 }
}
