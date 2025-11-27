// Arduino Code for Sleep Detector Alert System
// Receives: "d" → Drowsy (short beep)
//           "s" → Sleeping (continuous alarm + stop motor + blink 2 LEDs)

const int BUZZER_PIN   = 9;    // Buzzer on PWM pin (for tone)
const int MOTOR_PIN    = 10;   // Motor control pin (HIGH = run, LOW = stop)
const int LED1_PIN     = 11;   // First warning LED
const int LED2_PIN     = 12;   // Second warning LED

unsigned long previousMillis = 0;
const long blinkInterval = 500;  // LED blink speed in ms when sleeping
bool ledState = false;
bool sleepingMode = false;

void setup() {
  Serial.begin(115200);           // Must match Python baud rate (115200)
  
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MOTOR_PIN,  OUTPUT);
  pinMode(LED1_PIN,   OUTPUT);
  pinMode(LED2_PIN,   OUTPUT);

  // Start with everything safe
  digitalWrite(MOTOR_PIN, HIGH);   // Motor ON (change to LOW if your logic is inverted)
  noTone(BUZZER_PIN);
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);

  Serial.println("Arduino ready – waiting for commands (d / s / a)");
}

void loop() {
  // ---- Check for incoming commands from Python ----
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();               // remove any \r or \n
    cmd.toLowerCase();

    if (cmd == "d") {                 // DROWSY
      drowsyAlert();
    }
    else if (cmd == "s") {            // SLEEPING !!!
      sleepingAlert();
    }
    else if (cmd == "a") {            // ACTIVE – back to normal
      normalState();
    }
  }

  // ---- Blink LEDs when in sleeping mode ----
  if (sleepingMode) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= blinkInterval) {
      previousMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(LED1_PIN, ledState ? HIGH : LOW);
      digitalWrite(LED2_PIN, ledState ? HIGH : LOW);
    }
  }
}

// -------------------------------------------------
void drowsyAlert() {
  sleepingMode = false;
  digitalWrite(MOTOR_PIN, HIGH);   // Keep motor running
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);

  // Short warning beep (3 quick beeps)
  for (int i = 0; i < 3; i++) {
    tone(BUZZER_PIN, 1000);   // 1 kHz tone
    delay(150);
    noTone(BUZZER_PIN);
    delay(100);
  }
  Serial.println("Drowsy alert triggered");
}

void sleepingAlert() {
  sleepingMode = true;
  digitalWrite(MOTOR_PIN, LOW);    // STOP THE MOTOR
  tone(BUZZER_PIN, 2000);          // High-pitched continuous alarm
  Serial.println("SLEEPING – MOTOR STOPPED + ALARM + BLINKING");
}

void normalState() {
  sleepingMode = false;
  digitalWrite(MOTOR_PIN, HIGH);   // Motor back ON
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  noTone(BUZZER_PIN);
  Serial.println("Back to normal – Active");
}