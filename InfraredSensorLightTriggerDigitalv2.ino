/***************************************************************************/	
//	Function: Trigger a fade-up of lights when motion is detected.
//	Hardware: Passive Infrared (PIR) sensor, digital RGB LED strip
//  Arduino: Uno
//	Arduino IDE: Arduino-1.6.6
//	Author:	 Patrick		
//	Date: 	 Dec 2014
//	Version: v1.0
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
#include <FastLED.h>
#define NUM_LEDS 10
#define DATA_PIN 8
#define SENSOR_PIN 2
#define ADJUST_PIN 0

CRGB leds[NUM_LEDS];

int fadeRaw,fade,fadeOld;

int brightness = 0;      // how bright the LED is; initialized to off
// int maxBrightness = 200; // how high to fade up the PWM for the lights range 0-255
int fadeAmount = 10;     // how many points to fade the LED each iteration of the loop
int delayTime = 10;      // time (milliseconds) to pause between each iteration of the loop


void setup()
{
  Serial.begin(9600);
  pinMode(ADJUST_PIN,INPUT);
  pinMode(DATA_PIN, OUTPUT); // set the signal pin
  pinMode(SENSOR_PIN, INPUT);  // set the PIR sensor detection pin
  FastLED.addLeds<TM1803, DATA_PIN>(leds, NUM_LEDS);
  for(int dot = 0; dot < NUM_LEDS; dot++)
  {
    leds[dot].red = 100;
    leds[dot].green = 255;
    leds[dot].blue = 255;
  }
}

void loop()
{
  delay(delayTime);

  if (digitalRead(SENSOR_PIN)==HIGH)
  // sensor sees no movement
  // fade down the lights!
  {
    //analogWrite(DATA_PIN, brightness);
    FastLED.setBrightness(brightness);
    FastLED.show();
    delay(delayTime);
    
    if (brightness > 0)
    {
      brightness = brightness - fadeAmount;
      if (brightness < 0){ brightness = 0;}
    }
  }  
  else
  // sensor detecting movement
  // fade up the lights!
  {
   
    Serial.println("Motion detected!");
    Serial.println();
    delay(delayTime);

    fadeRaw = analogRead(0);
    fade = map(fadeRaw,0,1023,0,255);
    
    if (brightness < fade)
    {
      brightness = brightness + fadeAmount;
      if (brightness > fade){ brightness = fade;}
      Serial.print("Brightness: ");
      Serial.println(brightness);
      //analogWrite(DATA_PIN, brightness);
      FastLED.setBrightness(brightness);
      FastLED.show();
      delay(delayTime);
    }
  }
}
