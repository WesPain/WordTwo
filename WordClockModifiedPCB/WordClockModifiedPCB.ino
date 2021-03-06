#include <Wire.h>



/**************************************************************************
 *                                                                         *
 *  W O R D C L O C K   - A clock that tells the time using words.         *
 *                                                                         *
 * Hardware: Arduino Dumelove with a set of individual LEDs under a word   *
 *            stencil.                                                     *
 *                                                                         *
 *   Original Copyright (C) 2009  Doug Jackson (doug@doughq.com)           *
 *   Modifications Copyright (C) 2010 Scott Bezek (scott@bezekhome.com)    *
 *                                                                         *
 ***************************************************************************
 *                                                                         * 
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program; if not, write to the Free Software             *
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,                   *
 * MA  02111-1307  USA                                                     *
 *                                                                         *
 ***************************************************************************
 * 
 * Revision History
 * 
 * Date  	By	What
 * 20001025	DRJ	Initial Creation of Arduino Version 
 *                      - based on Wordclock.c - from PIC version
 * 20100124  Scott Bezek Changed LED pinout, added brightness control,
 *                        changed buttons to hour/minute increment 
 */


// Display output pin assignments
#define MTEN 	Display1=Display1 | (1<<0)  
#define HALF	Display1=Display1 | (1<<1)
#define QUARTER	Display1=Display1 | (1<<2)
#define TWENTY	Display1=Display1 | (1<<3)
#define MFIVE	Display1=Display1 | (1<<4)
#define MINUTES	Display1=Display1 | (1<<5)
#define PAST	Display1=Display1 | (1<<6)
#define UNUSED1	Display1=Display1 | (1<<7)

#define TO	Display2=Display2 | (1<<0)
#define ONE	Display2=Display2 | (1<<1)
#define TWO	Display2=Display2 | (1<<2)
#define THREE	Display2=Display2 | (1<<3)
#define FOUR	Display2=Display2 | (1<<4)
#define HFIVE	Display2=Display2 | (1<<5)
#define SIX	Display2=Display2 | (1<<6)
#define UNUSED2	Display2=Display2 | (1<<7)

#define SEVEN	Display3=Display3 | (1<<0)
#define EIGHT	Display3=Display3 | (1<<1)
#define NINE	Display3=Display3 | (1<<2)
#define HTEN	Display3=Display3 | (1<<3)
#define ELEVEN	Display3=Display3 | (1<<4)
#define TWELVE	Display3=Display3 | (1<<5)
#define OCLOCK  Display3=Display3 | (1<<6)
#define UNUSED3	Display3=Display3 | (1<<7)

int  hour=12, minute=00, second=00;
static unsigned long msTick =0;  // the number of Millisecond Ticks since we last 
                                 // incremented the second counter
int  count;
int selftestmode;
char Display1=0, Display2=0, Display3=0;


// hardware constants
int LEDClockPin=8;
int LEDDataPin=11;
int LEDStrobePin=10;

int MinuteButtonPin=2;
int HourButtonPin=3;
int PWMPin = 6;


void setup()
{
  // initialise the hardware	
  // initialize the appropriate pins as outputs:
  pinMode(LEDClockPin, OUTPUT); 
  pinMode(LEDDataPin, OUTPUT); 
  pinMode(LEDStrobePin, OUTPUT); 
  
  
  //pinMode(BrightnessPin, INPUT);
  pinMode(MinuteButtonPin, INPUT); 
  pinMode(HourButtonPin, INPUT);
  pinMode(13, OUTPUT);
  digitalWrite(MinuteButtonPin, HIGH);  //set internal pullup
  digitalWrite(HourButtonPin, HIGH); //set internal pullup
  
  
 // pinMode(PWMPin, OUTPUT); 
  
  Serial.begin(19200);
  
  /*
  while(true){
    Serial.print("In: ");
    Serial.print(analogRead(0), DEC);
    Serial.print(", ");
    Serial.print(digitalRead(MinuteButtonPin), DEC);
    Serial.print(", ");
    Serial.print(digitalRead(HourButtonPin), DEC);
    Serial.println();
  }
  */

  msTick=millis();      // Initialise the msTick counter
  displaytime();        // display the current time
}



void ledsoff(void) {
 Display1=0;
 Display2=0;
 Display3=0;
 }

void WriteLEDs(void) {
 // Now we write the actual values to the hardware
 shiftOut(LEDDataPin, LEDClockPin, MSBFIRST, Display3); //writes the values
 shiftOut(LEDDataPin, LEDClockPin, MSBFIRST, Display2);
 shiftOut(LEDDataPin, LEDClockPin, MSBFIRST, Display1);
 digitalWrite(LEDStrobePin,HIGH); //enables storage
 delay(2);
 digitalWrite(LEDStrobePin,LOW); //disables storage


 }

void selftest(void){
  Serial.print("TEST");
  //analogWrite(PWMPin, 255);
  
  ledsoff(); MTEN; WriteLEDs(); delay(500); 
  ledsoff(); HALF; WriteLEDs(); delay(500); 
  ledsoff(); QUARTER; WriteLEDs(); delay(500); 
  ledsoff(); TWENTY; WriteLEDs(); delay(500); 
  ledsoff(); MFIVE; WriteLEDs(); delay(500); 
  ledsoff(); MINUTES; WriteLEDs(); delay(500); 
  ledsoff(); PAST; WriteLEDs(); delay(500); 
  ledsoff(); TO; WriteLEDs(); delay(500); 
  ledsoff(); ONE; WriteLEDs(); delay(500); 
  ledsoff(); TWO; WriteLEDs(); delay(500); 
  ledsoff(); THREE; WriteLEDs(); delay(500); 
  ledsoff(); FOUR; WriteLEDs(); delay(500); 
  ledsoff(); HFIVE; WriteLEDs(); delay(500); 
  ledsoff(); SIX; WriteLEDs(); delay(500); 
  ledsoff(); SEVEN; WriteLEDs(); delay(500); 
  ledsoff(); EIGHT; WriteLEDs(); delay(500); 
  ledsoff(); NINE; WriteLEDs(); delay(500); 
  ledsoff(); HTEN; WriteLEDs(); delay(500); 
  ledsoff(); ELEVEN; WriteLEDs(); delay(500); 
  ledsoff(); TWELVE; WriteLEDs(); delay(500); 
  ledsoff(); OCLOCK; WriteLEDs(); delay(500); 
}


void displaytime(void){

  // start by clearing the display to a known state
  ledsoff(); //Clearing
  
  Serial.print("It is ");
  //The different valuses are or-ed into their appropiate display variables
  // now we display the appropriate minute counter
  if ((minute>4) && (minute<10)) { 
    MFIVE; 
    MINUTES; 
    Serial.print("Five Minutes ");
  } 
  if ((minute>9) && (minute<15)) { 
    MTEN; 
    MINUTES; 
    Serial.print("Ten Minutes ");
  }
  if ((minute>14) && (minute<20)) {
    QUARTER; 
      Serial.print("Quarter ");
  }
  if ((minute>19) && (minute<25)) { 
    TWENTY; 
    MINUTES; 
    Serial.print("Twenty Minutes ");
  }
  if ((minute>24) && (minute<30)) { 
    TWENTY; 
    MFIVE; 
    MINUTES;
    Serial.print("Twenty Five Minutes ");
  }  
  if ((minute>29) && (minute<35)) {
    HALF;
    Serial.print("Half ");
  }
  if ((minute>34) && (minute<40)) { 
    TWENTY; 
    MFIVE; 
    MINUTES;
    Serial.print("Twenty Five Minutes ");
  }  
  if ((minute>39) && (minute<45)) { 
    TWENTY; 
    MINUTES; 
    Serial.print("Twenty Minutes ");
  }
  if ((minute>44) && (minute<50)) {
    QUARTER; 
    Serial.print("Quarter ");
  }
  if ((minute>49) && (minute<55)) { 
    MTEN; 
    MINUTES; 
    Serial.print("Ten Minutes ");
  } 
  if (minute>54) { 
    MFIVE; 
    MINUTES; 
    Serial.print("Five Minutes ");
  }



  if ((minute <5))
  {
    switch (hour) {
    case 1: 
      ONE; 
      Serial.print("One ");
      break;
    case 2: 
      TWO; 
      Serial.print("Two ");
      break;
    case 3: 
      THREE; 
      Serial.print("Three ");
      break;
    case 4: 
      FOUR; 
      Serial.print("Four ");
      break;
    case 5: 
      HFIVE; 
      Serial.print("Five ");
      break;
    case 6: 
      SIX; 
      Serial.print("Six ");
      break;
    case 7: 
      SEVEN; 
      Serial.print("Seven ");
      break;
    case 8: 
      EIGHT; 
      Serial.print("Eight ");
      break;
    case 9: 
      NINE; 
      Serial.print("Nine ");
      break;
    case 10: 
      HTEN; 
      Serial.print("Ten ");
      break;
    case 11: 
      ELEVEN; 
      Serial.print("Eleven ");
      break;
    case 12: 
      TWELVE; 
      Serial.print("Twelve ");
      break;
    }
  OCLOCK;
  Serial.println("O'Clock");
  }
  else
    if ((minute < 35) && (minute >4))
    {
      PAST;
      Serial.print("Past ");
      switch (hour) {
    case 1: 
      ONE; 
      Serial.println("One ");
      break;
    case 2: 
      TWO; 
      Serial.println("Two ");
      break;
    case 3: 
      THREE; 
      Serial.println("Three ");
      break;
    case 4: 
      FOUR; 
      Serial.println("Four ");
      break;
    case 5: 
      HFIVE; 
      Serial.println("Five ");
      break;
    case 6: 
      SIX; 
      Serial.println("Six ");
      break;
    case 7: 
      SEVEN; 
      Serial.println("Seven ");
      break;
    case 8: 
      EIGHT; 
      Serial.println("Eight ");
      break;
    case 9: 
      NINE; 
      Serial.println("Nine ");
      break;
    case 10: 
      HTEN; 
      Serial.println("Ten ");
      break;
    case 11: 
      ELEVEN; 
      Serial.println("Eleven ");
      break;
    case 12: 
      TWELVE; 
      Serial.println("Twelve ");
      break;
      }
    }
    else
    {
      // if we are greater than 34 minutes past the hour then display
      // the next hour, as we will be displaying a 'to' sign
      TO;
      Serial.print("To ");
      switch (hour) {
      case 1: 
        TWO; 
       Serial.println("Two ");
       break;
      case 2: 
        THREE; 
      Serial.println("Three ");
        break;
      case 3: 
        FOUR; 
      Serial.println("Four ");
        break;
      case 4: 
        HFIVE; 
      Serial.println("Five ");
        break;
      case 5: 
        SIX; 
      Serial.println("Six ");
        break;
      case 6: 
        SEVEN; 
      Serial.println("Seven ");
        break;
      case 7: 
        EIGHT; 
      Serial.println("Eight ");
        break;
      case 8: 
        NINE; 
      Serial.println("Nine ");
        break;
      case 9: 
        HTEN; 
      Serial.println("Ten ");
        break;
      case 10: 
        ELEVEN; 
      Serial.println("Eleven ");
        break;
      case 11: 
        TWELVE; 
      Serial.println("Twelve ");
        break;
      case 12: 
        ONE; 
      Serial.println("One ");
        break;
      }
    }



   WriteLEDs();
   
}


void incrementtime(void){
  // increment the time counters keeping care to rollover as required
  second=0;
  if (++minute >= 60) {
    minute=0;
    if (++hour == 13) {
      hour=1;  
    }
  }  
  // debug outputs
  Serial.println();
  Serial.print(hour);
  Serial.print(",");
  Serial.print(minute);
  Serial.print(",");
  Serial.println(second);
  
}


void loop(void)
{
  digitalWrite(PWMPin,HIGH);
  //selftest();
 
  //Uncomment the following line and comment the next one in order to
  //  enable dimming via a potentiometer connected to pin 0:
  //analogWrite(PWMPin, analogRead(0)/4);
//  analogWrite(PWMPin, 255);
  
    // heart of the timer - keep looking at the millisecond timer on the Arduino
    // and increment the seconds counter every 1000 ms
    if ( millis() - msTick >999) {
        msTick=millis();
        second++;
        // Flash the onboard Pin13 Led so we know something is hapening!
        digitalWrite(13,HIGH);
        delay(100);
        digitalWrite(13,LOW);    
    }
    
    
    
    //test to see if we need to increment the time counters
    if (second==60) 
    {
      incrementtime();
      displaytime();
    }

    // test to see if a forward button is being held down
    // for time setting
    if ( (digitalRead(MinuteButtonPin) ==1 ) && second!=1) 
      // the forward button is down
      // and it has been more than one second since we
      // last looked
    {
      minute=(((minute/5)*5) +5); 
      second=0;
      incrementtime();
      second++;  // Increment the second counter to ensure that the name
      // flash doesnt happen when setting time
      displaytime();
    }

    // test to see if the back button is being held down
    // for time setting
    if ((digitalRead(HourButtonPin)==1 ) && second!=1) //legg til 1 her etter at du har koblet motstand til knappene
    
    {
      /*
      minute=(((minute/5)*5) -5); 
      second=0; // decrement the minute counter
      if (minute<0) { 
        minute=55; 
        if (--hour <0) hour=12;
      } 
      */
      minute = (minute/5)*5;  //round minute down to previous 5 min interval
      if (++hour == 13) {
        hour=1;  
      }
      incrementtime();
      second++;  // Increment the second counter to ensure that the name
      // flash doesnt happen when setting time  
      displaytime();
    }

}		  



