#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Servo steeringServo;
const int BUZZER_PIN = 9, LED1_PIN = 11, LED2_PIN = 12;
const int SERVO_PIN = 10;

unsigned long previousMillis = 0;
const long blinkInterval = 500;
bool ledState = false;
bool sleepingMode = false;

String currentState = "ACTIVE";
int currentPercentage = 100;

void setup() {
  Serial.begin(9600);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);

  steeringServo.attach(SERVO_PIN);
  steeringServo.write(0);

  // OLED Start
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED failed");
    for(;;);
  }

  showStartup();

  noTone(BUZZER_PIN);
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);

  Serial.println("SleepX READY - Send: a+90, s+80, d+40");
}

void loop() {
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    parseCommand(cmd);
  }

  if (sleepingMode) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= blinkInterval) {
      previousMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(LED1_PIN, ledState);
      digitalWrite(LED2_PIN, ledState);
    }
  }
}

void parseCommand(String cmd) {
  int plusPos = cmd.indexOf('+');
  if (plusPos == -1) return;

  String stateCode = cmd.substring(0, plusPos);
  String percStr = cmd.substring(plusPos + 1);
  int percentage = percStr.toInt();

  if (percentage > 100) percentage = 100;
  if (percentage < 0) percentage = 0;

  currentPercentage = percentage;

  if (stateCode == "a") {
    handleActive();
  } 
  else if (stateCode == "s") {
    handleSleeping();
  } 
  else if (stateCode == "d") {
    handleDrowsy();
  }

  Serial.print("Parsed: ");
  Serial.print(stateCode);
  Serial.print("+");
  Serial.print(percentage);
  Serial.println("%");
}

void showStartup() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);

  int16_t x, y;
  uint16_t w, h;
  display.getTextBounds("SleepX", 0, 0, &x, &y, &w, &h);

  int centerX = (SCREEN_WIDTH - w) / 2;
  int centerY = (SCREEN_HEIGHT - h) / 2;

  display.setCursor(centerX, centerY);
  display.print("SleepX");
  display.display();

  delay(2000);
  updateDisplay();
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // State centered
  display.setTextSize(2);
  int16_t x1, y1;
  uint16_t w1, h1;
  display.getTextBounds(currentState, 0, 0, &x1, &y1, &w1, &h1);
  display.setCursor((SCREEN_WIDTH - w1) / 2, 0);
  display.print(currentState);

  // Percentage centered
  String perc = String(currentPercentage) + "%";
  display.setTextSize(2);
  int16_t x2, y2;
  uint16_t w2, h2;
  display.getTextBounds(perc, 0, 0, &x2, y2, &w2, &h2);
  display.setCursor((SCREEN_WIDTH - w2) / 2, 18);
  display.print(perc);

  display.display();
}

void handleActive() {
  sleepingMode = false;
  steeringServo.write(0);
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  noTone(BUZZER_PIN);

  currentState = "ACTIVE";
  updateDisplay();
}

void handleSleeping() {
  sleepingMode = true;
  steeringServo.write(90);
  tone(BUZZER_PIN, 2000);

  currentState = "SLEEP";
  updateDisplay();
}

void handleDrowsy() {
  sleepingMode = false;
  steeringServo.write(0);
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);

  for (int i = 0; i < 3; i++) {
    tone(BUZZER_PIN, 1000);
    delay(150);
    noTone(BUZZER_PIN);
    delay(100);
  }

  currentState = "DROWSY";
  updateDisplay();
}
