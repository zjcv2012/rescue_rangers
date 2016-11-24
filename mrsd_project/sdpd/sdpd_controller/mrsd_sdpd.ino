/*
const int buttonPin = 7;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status

void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
}

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
    digitalWrite(ledPin, HIGH);
  } else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
  }
}*/

#include <Servo.h>

Servo servo; 

void setup() {

  pinMode(1,OUTPUT);
  servo.attach(A0); //analog pin 0
    
  // Initialize serial
  Serial.begin(9600);
  
  //servo1.setMaximumPulse(2000);
  //servo1.setMinimumPulse(700);
}

void drive_forward() 
{
    for(int pos = 0; pos < 180; pos += 1)  
        servo.write(pos);              
}

void loop() {

    if ( Serial.available()) 
    {
        int power = 0;
        String input = Serial.readString();
        input.trim();
        Serial.println(input);
        if (input.equals("1")) {
            Serial.println("f");
        }
        if (input.equals("0")) {
            Serial.println("r");
            for(int pos = 180; pos > 0; pos -= 1)  // goes from 0 degrees to 180 degrees 
            {                                  // in steps of 1 degree 
                servo.write(pos);              // tell servo to go to position in variable 'pos'
            } 
        }
    }
} 

