/***************************************************************************/	
//	Function: Trigger a fade-up of lights when motion is detected.
//	Hardware: Passive Infrared (PIR) sensor, digital RGB LED strip with
//            TM1803 chipset (3 LEDs per pixel)
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

#define NUM_LEDS_INSIDE 10
#define DATA_PIN_INSIDE 8
#define SENSOR_PIN_INSIDE 2
#define DATA_PIN_OUTSIDE 3 //this is actually just the transistor base 
                           //for the analog strip; must be a PWM pin
#define SENSOR_PIN_OUTSIDE 7 
#define ADJUST_PIN A0 // this sets the max brightness for the inside light

#define FADE_AMOUNT 10 // how many points to fade the LED each iteration of the loop
#define DELAY_TIME 10 // time (milliseconds) to pause between each iteration of the loop
#define MAX_BRIGHTNESS_OUTSIDE 200

//the CGRB data type is from FastLED
CRGB leds[NUM_LEDS_INSIDE];

int sensorStateInside;
int fadeRaw,fade,fadeOld;
int brightnessInside = 0, brightnessOutside = 0;
int counterInside = 0;

void setup()
{
  Serial.begin(9600);
  
  //set the Arduino pins
  pinMode(ADJUST_PIN,INPUT);
  pinMode(DATA_PIN_INSIDE, OUTPUT); 
  pinMode(SENSOR_PIN_INSIDE, INPUT);
  pinMode(SENSOR_PIN_OUTSIDE, INPUT);

  // DATA_PIN_OUTSIDE must default to OUTPUT?
  
  //setup the inside strip with the FastLED library
  FastLED.addLeds<TM1803, DATA_PIN_INSIDE>(leds, NUM_LEDS_INSIDE);

  //set the color of the pixels
  //the TM1803's red component is visually nearly
  //twice as powerful as the green and blue, so
  //it's scaled down to create an overall white light
  for(int dot = 0; dot < NUM_LEDS_INSIDE; dot++)
  {
    leds[dot].red = 215;
    leds[dot].green = 255;
    leds[dot].blue = 235;
  }
}

void loop()
{
  //send state to outside lights
  analogWrite(DATA_PIN_OUTSIDE, brightnessOutside);
  //send state to inside lights
  FastLED.setBrightness(brightnessInside);
  FastLED.show();

  //controls the fade speed, and also the reaction
  //time to multiple sensor hits
  delay(DELAY_TIME);

  sensorStateInside = digitalRead(SENSOR_PIN_INSIDE);
  
  //inside (stairwell) light
  if (sensorStateInside==HIGH && counterInside == 0)
  // sensor sees no movement
  // fade down the lights!
  {
    if (brightnessInside > 0)
    {
      brightnessInside = brightnessInside - FADE_AMOUNT;
      //make sure the brightness doesn't go
      //negative
      if (brightnessInside < 0){ brightnessInside = 0;}
    }
  }  
  else
  // sensor detecting movement
  // fade up the lights!
  // they stay on for 5 seconds after trigger
  { 
    Serial.println("Motion detected inside!");
    Serial.println();
    
    //reset the counter if the sensor detects a new hit
    if(sensorStateInside==LOW){ counterInside = 20 * 1000/DELAY_TIME;}

    fadeRaw = analogRead(0);
    fade = map(fadeRaw,0,1023,0,255);
    
    if (brightnessInside < fade)
    {
      brightnessInside = brightnessInside + FADE_AMOUNT;
      if (brightnessInside > fade){ brightnessInside = fade;}
      Serial.print("Brightness inside: ");
      Serial.println(brightnessInside);
    }
    counterInside = counterInside - DELAY_TIME; 
  }
  
  //outside light

  if (digitalRead(SENSOR_PIN_OUTSIDE)==LOW)
  // sensor sees no movement
  // fade down the lights!
  {
    if (brightnessOutside > 0)
    {
      brightnessOutside = brightnessOutside - FADE_AMOUNT;
    }
  }  
  else
  // sensor detecting movement
  // fade up the lights!
  { 
    Serial.println("Motion detected outside!");
    if (brightnessOutside < MAX_BRIGHTNESS_OUTSIDE)
    {
      brightnessOutside = brightnessOutside + FADE_AMOUNT;
      Serial.print("Brightness outside: ");
      Serial.println(brightnessOutside);
    }
  }
}
