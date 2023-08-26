/*

ArduinoWatchdog 

Judson Hartley 08/26/2023

This program monitors a gpio pin of a raspberry pi via an optoisolator.
If the input is not cycled up and down at a slow rate, high and low for at least 100ms,
after the time specified in watchdogInterval, will make the RELAY_PIN high, then low after
the watchdogResetInterval. RELAY_PIN should be hooked to a relay who's COM and NC have the power for the pi.


*/



#include <Arduino.h>

#define OUT_PIN 3 //self test output pin... connect to input on optoisolator to reset ext. watchdog
#define IN_PIN 9 //pin for exteral watchdog input from optoisolator
#define RELAY_PIN 10 //pin for relay for external watchdog

//external watchdog timer settings in ms...
const unsigned long watchdogInterval = 300000;
const unsigned long watchdogResetInterval = 10000;


#include <Adafruit_SleepyDog.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(OUT_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  pinMode(IN_PIN, INPUT_PULLUP);
  delay(2000);
  int countdownMS = Watchdog.enable(20000);
  Serial.print("Enabled the watchdog with max countdown of ");
  Serial.print(countdownMS, DEC);
  Serial.println(" milliseconds!");
  Serial.println();

}

int counter = 0;
unsigned long currentTime = 0;
unsigned long previousTime = 0;
const unsigned long interval = 2000;
bool last = true;
int lastInValue = 0;

unsigned long watchdogTime = 0;

void loop() {
  // put your main code here, to run repeatedly:
  Watchdog.reset();
  currentTime = millis();
  if (currentTime-previousTime >= interval) {
    if (last) Serial.println("Switch... high"); else Serial.println("Switch...low");
    if (last) digitalWrite(OUT_PIN, HIGH); else digitalWrite(OUT_PIN, LOW);
    last = !last;
    previousTime = currentTime;
  }
  if (currentTime-watchdogTime >= watchdogInterval) {
    //if here no life on input pin...
    Serial.println("External Watchdog Triggered!!!");
    digitalWrite(RELAY_PIN, HIGH);
    watchdogTime = currentTime;
    do
    {
      currentTime = millis();
      Watchdog.reset();

    } while (currentTime-watchdogTime < watchdogResetInterval);
    Serial.println("External Watchdog Trigger cleared...");
    digitalWrite(RELAY_PIN, LOW);
    watchdogTime = currentTime;

  }
  int value = digitalRead(IN_PIN);
  if (value != lastInValue) {
    //have life on IN_PIN...
    //reset watchdog time...
    Serial.println("External Watchdog Reset...");
    watchdogTime = currentTime;
    lastInValue = value;

  }
  Serial.print(value);
  // Serial.print(" ");
  // Serial.println(counter);

  // counter++;
  delay(100);
  
}

