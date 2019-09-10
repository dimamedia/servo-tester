/*
 * DiA Servo tester v1.0
 * 2019-09-09
 *
 * Current code is for Mellbell Pico board, which has Arduino Leonardo compatible schema.
 */


#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

Adafruit_SSD1306 display(128, 32, &Wire, -1);

// Pins
const int servoPin = 1;
const int buttonPin = 0;
const int ledPin =  18; // optional
// A1 pin used for voltage sensor

// Servo params
Servo myservo;
int pos = 1500;
int steps = 10;
int dir = 1;
unsigned long pause = 0;
unsigned long pauseLength = 1000;

// Button and mode

int modeState = LOW; // LOW - Center & led off, HIGH - Sweep & led on
int buttonState;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

// Voltage meter

float voltage;

static const unsigned char PROGMEM logo_bmp[] =
{ B11111111, B11100000, B00000000,
  B11111111, B11111100, B00000000,
  B11111111, B11111111, B00000000,
  B11111111, B11111111, B10000000,
  B11111100, B00111111, B11000000,
  B01111100, B00001111, B11100000,
  B00111110, B00000011, B11110000,
  B00111111, B00000001, B11110000,
  B00011111, B00000001, B11111000,
  B00001111, B10000000, B11111000,
  B00001111, B11000000, B01111100,
  B01100111, B11000000, B01111100,
  B11110011, B11100000, B01111100,
  B11110011, B11110000, B00111110,
  B11110001, B11110000, B00111110,
  B01100000, B11111000, B00111110,
  B00000000, B11111110, B11111110,
  B11111110, B01111110, B11111110,
  B11111111, B00111111, B11111110,
  B11111111, B10111111, B11111100,
  B11111111, B10011111, B11111100,
  B11110000, B00001111, B11111100,
  B11110000, B00000111, B11111000,
  B11110000, B00000111, B11111000,
  B11110000, B00000111, B11110000,
  B11110000, B00001111, B11110000,
  B11110000, B01111111, B11110000,
  B11111111, B11111111, B11111000,
  B11111111, B11111111, B11111100,
  B11111111, B11111111, B01111100,
  B11111111, B11111100, B00111110,
  B11111111, B11100000, B00011111};

void setup() {
  myservo.attach(servoPin);

  pinMode(buttonPin, INPUT);

  pinMode(ledPin, OUTPUT); // optional
  digitalWrite(ledPin, modeState); // optional

  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();

  display.drawBitmap(0, 0, logo_bmp, 24, 32, 1);
  display.setTextSize(2);
  display.setTextColor(WHITE, BLACK);
  display.setCursor(30,0);
  display.println(F("Servo"));
  display.setCursor(30,18);
  display.println(F("tester"));
  display.setTextSize(1);
  display.setCursor(104, 25);
  display.println(F("v1.0"));
  display.display();
  delay(2000);

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println(F("DiA Servo tester"));
  display.setCursor(0, 12);
  display.print("Center: "); 
  display.setCursor(98, 15 );
  display.print("Batt: "); 

  updateValues();
}

void updateValues(void) {
  display.setTextSize(2);
  display.setCursor(45, 11);
  display.print(pos);
  display.setTextSize(1);
  display.setCursor(104, 25);
  display.print(voltage, 1); display.print("V");
  display.display();
}

void loop() {
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();  // reset the debouncing timer
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        modeState = !modeState;
        if(modeState == HIGH) {
          display.setTextSize(1);
          display.setCursor(0, 12);
          display.print("Sweep:  "); 
        }
        else {
          display.setTextSize(1);
          display.setCursor(0, 12);
          display.print("Center: ");         
        }
      }
    }
  }
  digitalWrite(ledPin, modeState); // optional
  lastButtonState = reading;

  if(modeState == HIGH) {
    if(pause > 0 && ((millis() - pause) > pauseLength)) pause = 0;
    else if(pause == 0) {
      pos = pos + steps * dir;        // moving servo by one step somewhere
      if(pos <= 1000) dir = 1;        // change direction to positive
      else if(pos >= 2000) dir = -1;  // change direction to negative
      myservo.writeMicroseconds(pos); // send pwm to servo
      delay(15);                      // wait a bit for a servo to reach the destination

      if(pos == 1000 || pos == 1500 || pos == 2000) pause = millis(); // pause at the center and both ends for a second
    }
  }
  else {
    pos = 1500; // fixed center
    myservo.writeMicroseconds(pos);
    delay(15);
  }

  int sensorValue = analogRead(A1); // read the value from voltage "sensor"
  voltage = sensorValue * (5.00 / 1023.00) * 2; //convert the value to a true voltage. 
   
  updateValues();
}
