const unsigned long timer0Duration = 10000; // 10 detik
const unsigned long timer1Duration = 20000; // 20 detik
const unsigned long timer2Duration = 0000; // 15 detik
const unsigned long timer3Duration = 21000; // 21 detik

unsigned long timer0StartTime = 0;
unsigned long timer1StartTime = 0;
unsigned long timer2StartTime = 0;
unsigned long timer3StartTime = 0;

bool timer0Active = false;
bool timer1Active = false;
bool timer2Active = false;
bool timer3Active = false;

bool ssr0State = false;
bool ssr1State = false;
bool ssr2State = false;
bool ssr3State = false;

unsigned long lastCheckTime = 0; // Waktu terakhir kali pemeriksaan dilakukan

void setup() {
  Serial.begin(115200);

  // Inisialisasi timer
  timer0StartTime = millis();
  timer1StartTime = millis();
  timer2StartTime = millis();
  timer3StartTime = millis();

  timer0Active = false;
  timer1Active = false;
  timer2Active = false;
  timer3Active = false;

  ssr0State = false;
  ssr1State = false;
  ssr2State = false;
  ssr3State = false;

  setSSRState();
  delay(1500);

  lastCheckTime = millis(); // Inisialisasi waktu pemeriksaan pertama
}

void setSSRState() {
  ssr0State = (timer0Duration != 0);
  timer0Active = (timer0Duration != 0);
  Serial.print("Heat Pad 1 Status: ");
  Serial.println(ssr0State);
  Serial.print("Timer 0 Status: ");
  Serial.println(timer0Active);

  ssr1State = (timer1Duration != 0);
  timer1Active = (timer1Duration != 0);
  Serial.print("Heat Pad 2 Status: ");
  Serial.println(ssr1State);
  Serial.print("Timer 1 Status: ");
  Serial.println(timer1Active);

  ssr2State = (timer2Duration != 0);
  timer2Active = (timer2Duration != 0);
  Serial.print("Heat Pad 3 Status: ");
  Serial.println(ssr2State);
  Serial.print("Timer 2 Status: ");
  Serial.println(timer2Active);

  ssr3State = (timer3Duration != 0);
  timer3Active = (timer3Duration != 0);
  Serial.print("Heat Pad 4 Status: ");
  Serial.println(ssr3State);
  Serial.print("Timer 3 Status: ");
  Serial.println(timer3Active);
}

void bacaTempPad(){
  
}

String convertToHourMeter(unsigned long seconds) {
  unsigned long totalMinutes = seconds / 60;
  unsigned long hours = totalMinutes / 60;
  unsigned long minutes = totalMinutes % 60;
  
  String hourMeter = String(hours) + " jam " + String(minutes) + " menit";
  return hourMeter;
}

void loop() {
  unsigned long currentTime = millis();

  // Pemeriksaan setiap 1 detik
  if (currentTime - lastCheckTime >= 1000) {
    lastCheckTime = currentTime; // Update waktu pemeriksaan terakhir

    // Cek timer0
    if (ssr0State && timer0Active && currentTime - timer0StartTime >= timer0Duration) {
      timer0Active = false; 
      Serial.println("Timer0 expired and Heat Pad 1 set to LOW");
    }

    // Cek timer1
    if (ssr1State && timer1Active && currentTime - timer1StartTime >= timer1Duration) {
      timer1Active = false;
      Serial.println("Timer1 expired and Heat Pad 2 set to LOW");
    }

    // Cek timer2
    if (ssr2State && timer2Active && currentTime - timer2StartTime >= timer2Duration) {
      timer2Active = false;
      Serial.println("Timer2 expired and Heat Pad 3 set to LOW");
    }

    // Cek timer3
    if (ssr3State && timer3Active && currentTime - timer3StartTime >= timer3Duration) {
      timer3Active = false;
      Serial.println("Timer3 expired and Heat Pad 4 set to LOW");
    }
    //Serial.println(currentTime/1000);
    String hourMeter = convertToHourMeter(currentTime/1000);
    Serial.println(hourMeter);
  }

  // Implementasi logika lain di sini
  //bacaTempPad();
}
