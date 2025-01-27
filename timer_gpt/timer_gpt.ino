// Timer duration in seconds
volatile int timerDuration[4] = {10, 20, 30, 40}; // Example initial durations for timers
volatile int timers[4] = {0, 0, 0, 0}; // Initialize timers to 0
volatile bool stateTimer[4] = {true, true, true, true}; // Initial states for timers

unsigned long previousMillis = 0; // Store the last time millis() was updated
const long interval = 1000; // Interval to update timers (1 second)

const int startButtonPin = 3; // interrupt pin mega
volatile bool startPressed = false;

void setup() {
  Serial.begin(9600); // Start the serial communication
  pinMode(startButtonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(startButtonPin), changeState, FALLING);
  for (int i = 0; i < 4; i++) {
    timers[i] = timerDuration[i]; // Initialize the timers with the given duration
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
  
  // Check if it's time to update the timers
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // Save the last time we updated the timers
    
    for (int i = 0; i < 4; i++) {
      if (stateTimer[i]) {
        if (timers[i] > 0) {
          timers[i]--; // Decrement the timer
          Serial.print("Timer ");
          Serial.print(i + 1);
          Serial.print(": ");
          Serial.println(timers[i]);
          
          // Check if the timer has reached zero
          if (timers[i] == 0) {
            stateTimer[i] = false; // Set the state to false
            Serial.print("Timer ");
            Serial.print(i + 1);
            Serial.println(" stop");
          }
        }
      } else {
        timers[i] = 0; // Ensure the timer is set to 0
        Serial.print("Timer ");
        Serial.print(i + 1);
        Serial.println(" stop");
      }
      
    }
  }
}
