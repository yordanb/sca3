int period = 1000;
unsigned long time_now = 0;

int line1Time = 30;
int line2Time = 20;
int line3Time = 0;
int line4Time = 15;

bool line1State = false;
bool line2State = false;
bool line3State = false;
bool line4State = false;

const int startButtonPin = 3; //interupt pin mega
volatile bool startPressed = false;

void setup() {
    Serial.begin(9600);
    pinMode(startButtonPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(startButtonPin), changeState, FALLING);
    inisiasiTimer();
}

void inisiasiTimer(){
  if(line1Time != 0){
    line1State = true;
  }
  if(line2Time != 0){
    line2State = true;
  }
  if(line3Time != 0){
    line3State = true;
  }
  if(line4Time != 0){
    line4State = true;
  }
}

void changeState(){
  line2State = !line2State;
}

void cekLineState(){
  Serial.print(line1State);Serial.print(" ");
  Serial.print(line2State);Serial.print(" ");
  Serial.print(line3State);Serial.print(" ");
  Serial.println(line4State);
}

void cekSisaLineTimer(){
  if(line1Time - 1 == 0){
    line1Time = 0; line1State = false;
  }
  if(line2Time - 1 == 0){
    line2Time = 0; line2State = false;
  }
  if(line3Time - 1 == 0){
    line3Time = 0; line3State = false;
  }
  if(line4Time - 1 == 0){
    line4Time = 0; line4State = false;
  }
}

void tampilSisaLineTimer(){
  Serial.print(line1Time);Serial.print(" ");
  Serial.print(line2Time);Serial.print(" ");
  Serial.print(line3Time);Serial.print(" ");
  Serial.println(line4Time);
}

void loop() {
    if(millis() >= time_now + period){
        time_now += period;
        Serial.print(time_now);
        Serial.println(" - Hello");
        cekLineState();
        cekSisaLineTimer();
        tampilSisaLineTimer();
    }
   
    //Run other code
}
