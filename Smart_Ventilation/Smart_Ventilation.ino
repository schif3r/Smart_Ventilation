#include <DHT.h>
#include <ESP32Servo.h> // error countered i first because of the library

// Pins
#define DHTPIN 4
#define DHTTYPE DHT11
#define SERVOPIN 13

#define LED_GREEN 14
#define LED_YELLOW 27
#define LED_RED 26
#define BUTTON_PIN 33

DHT dht(DHTPIN, DHTTYPE);
Servo myServo;

// Mode control
int mode = 0; // default if teh device start mode=0 
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;
bool lastButtonState = HIGH;
bool buttonState;

void setup() {
  Serial.begin(115200);
  dht.begin();

  myServo.attach(SERVOPIN);
  myServo.write(0);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

pinMode(BUTTON_PIN, INPUT_PULLUP);

  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
}

  void loop() {
  //Button Debounce
  bool reading = digitalRead(BUTTON_PIN);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        // Toggle between 2 modes: 0 and 1
        mode = (mode + 1) % 2;

        if (mode == 0) {
          Serial.println("Switched to MODE 0: LED Temp Display");
        } else if (mode == 1) {
          Serial.println("Switched to MODE 1: Silent Servo Only");
        }
      }
    }
  }
  lastButtonState = reading;

  //Sensor Reading
  float temperature = dht.readTemperature();
  if (isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  //Proportional Control
  float targetTemp = 25.0;
  float Kp = 10.0;
  float error = temperature - targetTemp;
  float angle = Kp * error;
  angle = constrain(angle, 0, 180);
  myServo.write(angle);

  Serial.print("Servo Angle: ");
  Serial.println(angle);

  //Mode Behavior
  if (mode == 0) {
    // LED status mode based on temperature
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);

    if (temperature < 27) {
      digitalWrite(LED_GREEN, HIGH);
      Serial.println("LED Status: GREEN");
    } else if (temperature >= 27 && temperature < 32) {
      digitalWrite(LED_YELLOW, HIGH);
      Serial.println("LED Status: YELLOW");
    } else {
      digitalWrite(LED_RED, HIGH);
      Serial.println("LED Status: RED");
    }

  } else if (mode == 1) {
    // Silent servo mode, all LEDs off
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
    Serial.println("LEDs OFF — Mode 1 active.");
  }

  delay(200);
}
