/***************************************************************************/	
//	Function: Measure the distance to obstacles in front and print the distance
//			  value to the serial terminal.The measured distance is from 
//			  the range 0 to 400cm(157 inches).
//	Hardware: Ultrasonic Range sensor
//	Arduino IDE: Arduino-1.0
//	Author:	 LG		
//	Date: 	 Jan 17,2013
//	Version: v1.0 modified by FrankieChu
//           v2.0 heavily modified by PhattyMcG
//	by www.seeedstudio.com
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
//
/*****************************************************************************/
#include "Arduino.h"
class Ultrasonic
{
	public:
		Ultrasonic(int pin);
    void DistanceMeasure(void);
		long microsecondsToCentimeters(void);
		// long microsecondsToInches(void);
	private:
		int _pin;//pin number of Arduino that is connected with SIG pin of Ultrasonic Ranger.
        long duration;// the Pulse time received;
};
Ultrasonic::Ultrasonic(int pin)
{
	_pin = pin;
}

/*Begin the detection and get the pulse back signal*/
void Ultrasonic::DistanceMeasure(void)
{
  pinMode(_pin, OUTPUT);
	digitalWrite(_pin, LOW);
	delayMicroseconds(2); // change this to lengthen the pulse?
	digitalWrite(_pin, HIGH);
	delayMicroseconds(5); // change this to lengthen the delay between pulses?
	digitalWrite(_pin,LOW);
	pinMode(_pin,INPUT);
	duration = pulseIn(_pin,HIGH); // http://arduino.cc/en/Reference/pulseIn
                                       // "if value is HIGH, pulseIn() waits for the pin to go HIGH, starts timing, then waits for the pin to go LOW and stops timing"
}

/*The measured distance from the range 0 to 400 Centimeters*/
long Ultrasonic::microsecondsToCentimeters(void)
{
	return duration/29/2;	
}

/*The measured distance from the range 0 to 157 Inches*/
//long Ultrasonic::microsecondsToInches(void)
//{
//	return duration/74/2;	
//}

Ultrasonic ultrasonic(7); // set signal (SIG) pin for sensor to 7
long RangeInCentimeters;


int triggerDistance = 200; // the distance (in cm) at which to trigger the lights
int triggerPeriod = 1000;  // the period of time in msec over which to count
                           // trigger events before starting the fade up loop
const int triggerMemoryLength = 200; // number of data points to retain in triggerMemory
int triggerMemory [triggerMemoryLength];  // variable to retain the number of triggering events (detect
                     // ultrasound reflection less than triggerDistance
                     // TO DO: set an element of the array  to 1 every 500 msec, then 
                     // sum the elements every iteration of the void loop(), and if 
                     // it's greater than the threshold, then call the fade up
int triggerCounter = 0; // index of triggerMemory
int triggerCount = 0; // count of trigger events in triggerMemory over triggerMemoryLength
int triggerCountThreshold = 25; // threshold for triggerCounter, above which the lights
                             // turn on, and below which they turn off
//int triggerCounterDecreaseDelay = 500; // time in msec before decreasing the 
                                       // triggerCounter
int delayTimeOn = 200; // delay time (in multiples of the delayTime) once the lights are triggered to turn on
int delayCounter = 0;

int MOSFETPin = 10;       // pin for transistor gate; originally used a MOSFET!
//int LightSensorPin = 4;  // pin for light sensor
int brightness = 0;      // how bright the LED is; initialized to off
int maxBrightness = 200; // how high to fade up the PWM for the lights range 0-255
int fadeAmount = 10;     // how many points to fade the LED each iteration of the loop
int delayTime = 10;      // time (milliseconds) to pause between each iteration of the 
                         // main loop
int i;


void setup()
{
  Serial.begin(9600);
  pinMode(MOSFETPin, OUTPUT); // set the transistor gate pin
  //pinMode(LightSensorPin, INPUT);  // set the light sensor detection pin
}

void loop()
{

  ultrasonic.DistanceMeasure();// get the current signal time;
  RangeInCentimeters = ultrasonic.microsecondsToCentimeters();//convert the time to centimeters

	Serial.print("distance: ");
	Serial.print(RangeInCentimeters);//0~750cm
	Serial.println(" cm");
        Serial.print("Brightness: ");
        Serial.println(brightness);
	delay(delayTime);

  //record triggering events in the array
  if (RangeInCentimeters < triggerDistance) {
    triggerMemory[triggerCounter] = 1;
  } else {
    triggerMemory[triggerCounter] = 0;
  Serial.println("sum going down");
  }

  Serial.print("trigger counter: ");
  Serial.println(triggerCounter);

  //loop the index to the beginning once it reaches the max
  //number of data points
  if (triggerCounter < triggerMemoryLength - 1){
    triggerCounter += 1;  
  } else {
    triggerCounter = 0;
  }

  triggerCount = 0;
  // count the number of trigger events
  for(i=0;i<triggerMemoryLength;i++){
    triggerCount = triggerCount + triggerMemory[i];
  }

  Serial.print("trigger count: ");
  Serial.println(triggerCount);

  
  if (triggerCount > triggerCountThreshold) {
    // if the count goes above the threshold,  
    // rangefinder is measuring something less 
    // than the trigger distance
    // fade up the lights!
    //for (brightness=0; brightness < maxBrightness; fadeAmount)
    while (brightness < maxBrightness)
    {
      brightness = brightness + fadeAmount;
      analogWrite(MOSFETPin, brightness);
      delay(delayTime);
    }
    /*
    // keep the lights on for delayTimeOn
    delay(delayTimeOn);
    //reset the count of trigger events so the delay
    //doesn't slow down the void loop
    for(i=0;i<triggerMemoryLength;i++){
      triggerMemory[i]=0;
    }
    */
  } else {
    delayCounter++;
    // there's nothing in front of the sensor
    // fade down the lights!
    if(delayCounter > delayTimeOn){
      while (brightness > 0)
      {
       brightness = brightness - fadeAmount;
       analogWrite(MOSFETPin, brightness);
       delay(delayTime);
      }
      delayCounter = 0;
    }
  }
}
