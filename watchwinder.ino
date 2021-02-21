
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"
RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);
// timing constants
typedef unsigned long clock_t;
const clock_t SECOND = 1000L;           // 1 secpmd = 1000 ms
const clock_t MINUTE = 60L * SECOND;    // 1 minute = 60000 ms
const clock_t HOUR = 60L * MINUTE;      // 1 hour = 3600000 ms

#include <Stepper.h>        
const int stepsPerRevolution = 64;

Stepper stepper(stepsPerRevolution, 8, 10, 9, 11);  


const int ProxSensor=3;
bool hasrun;
bool detected;
int DisplayClock()
{
  //prints clock to lcd screen
  DateTime now = rtc.now();
  lcd.clear();
  lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
  lcd.setCursor(0, 1);
  lcd.print(now.hour(), DEC);
  lcd.print(':');
  lcd.print(now.minute(), DEC);
  lcd.print(':');
  lcd.print(now.second(), DEC);
  lcd.setCursor(0, 2);
  lcd.print(now.day(), DEC);
  lcd.print('/');
  lcd.print(now.month(), DEC);
  lcd.print('/');
  lcd.print(now.year(), DEC);
  lcd.print(' ');
}

int DisplayProgress(float progress, const TimeSpan& elapsedTime)
{
  lcd.clear();
  lcd.print(progress, 2);
  lcd.print("% complete");
  lcd.setCursor(0, 1);
  lcd.print("Elapsed time:");
  lcd.setCursor(0, 2);
  if (elapsedTime.hours() < 10)
       lcd.print('0');
  lcd.print(elapsedTime.hours(), DEC);
  lcd.print(':');
  if (elapsedTime.minutes() < 10)
       lcd.print('0');
  lcd.print(elapsedTime.minutes(),DEC);
  lcd.print(':');
  if (elapsedTime.seconds() < 10)
      lcd.print('0');
  lcd.print(elapsedTime.seconds(), DEC);
}

int StartWinding()
{
  //turns fully clockwise and fully counter clockwise and updates progress and time elapsed
  stepper.setSpeed(300);

  enableStepper();
  DateTime startTime = rtc.now();
  
  for (int counter = 0; counter < 349; counter++)
  {
    DateTime currentTime = rtc.now();
    TimeSpan elapsedTime = currentTime - startTime;
    float currentProgress = 100.0 * (float)counter / 350.0;
    //current progress is stored as a precentage
   
    DisplayProgress(currentProgress, elapsedTime);
    
    int Steps = -2048;
    //-2048 is one full rotation counter clockwise 

   
    stepper.step(Steps);
    //move motor

    delay(2 * SECOND);     

    Steps  = 2048;

   
    stepper.step(Steps);

    delay(2 * SECOND);     
  }

  disableStepper();
}




void enableStepper()
{
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  digitalWrite(5, HIGH);        
}

void disableStepper()
{
  
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  pinMode(11, INPUT);
  digitalWrite(5, LOW);         
}



bool DetectStartingPosition()
//detects if IR sensor has been triggered
{
  if (digitalRead(ProxSensor) == LOW)   
  {
    detected = true;
    digitalWrite(13, LOW);
    return true;
  }
  else
  {
    digitalWrite(13, HIGH);
    return false;
  }
}

void GotoStartingPosition()
{
  // moves step motor 10 steps and shows current time in between 
  stepper.setSpeed(100);
  enableStepper();
  stepper.step(10);
  disableStepper();
}

void setup()
{
  //IR sensor setup
  pinMode(13, OUTPUT);   
  pinMode(ProxSensor, INPUT);
  hasrun = false;
  //lcd setup
  Wire.begin();
  lcd.setCursor(0, 0);
  lcd.begin();
  lcd.backlight();
  detected = false;
}

void loop()
{
  DisplayClock();

  if (!hasrun)
  {
 // spin step motor until we trigger IR sensor which means we have reached home position
// this is done so that the rings are alligned in a way which makes it easy to put in the watch into the winder so the rings dont obstruct anything
    if (!DetectStartingPosition())
    {
      GotoStartingPosition();
    }
  }

  if (!hasrun && DetectStartingPosition()) {
   
    StartWinding();
    hasrun = true;
  }

  DateTime now = rtc.now();
  if (now.hour() == 8 && now.minute() == 30 && now.second() == 0)
  {
    hasrun = false;
  }

  if (hasrun)
  {
    delay(SECOND);
  }
}
