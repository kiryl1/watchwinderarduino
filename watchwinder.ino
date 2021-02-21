
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
int clockthread() {
  //outputs current time onto lcd
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


int windthread() {
  // rotates step motor one full rotation clocwise then ccw 
  //total 700 rotations
      stepper.setSpeed(300);     
      enableStepper();
      for (int counter = 0; counter < 349; counter++) {
        clockthread();
        int Steps = -2048;
        clockthread();
        stepper.step(Steps);
        clockthread();
        delay(2 * SECOND);     
        clockthread();
        Steps  = 2048;  
        clockthread();
        stepper.step(Steps);
        clockthread();
        delay(2 * SECOND);    
      }
      disableStepper();
  
}



void enableStepper() {
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  digitalWrite(5, HIGH);        
}

void disableStepper() {
  
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  pinMode(11, INPUT);
  digitalWrite(5, LOW);         
}



bool detecthome(){
  //detects if IR sensor has been triggered
    if(digitalRead(ProxSensor)==LOW)      
             { 
              detected = true;
               digitalWrite(13, LOW);
               //pin 13 is led 
               return true;
             }
  else
            {
              digitalWrite(13, HIGH); 
              return false;
             }
            
   
}
void gohome(){
  // moves step motor 10 steps and shows current time in between 
  stepper.setSpeed(100);     

      enableStepper();
        clockthread();
        stepper.step(10);
           
      disableStepper();
      
}
void setup()
{
  pinMode(13, OUTPUT);   
  pinMode(ProxSensor,INPUT);
  // IR sensor setup 
  hasrun = false;

  lcd.setCursor(0, 0);
  rtc.adjust(DateTime(__DATE__, __TIME__));
  DateTime now = rtc.now();
  Wire.begin();

  lcd.begin();

  lcd.backlight();
  detected = false;
}

void loop()
{DateTime now = rtc.now();
// spin step motor until we trigger IR sensor which means we have reached home position
// this is dones so that the rings are alligned in a way which makes it easy to put in the watch into the winder so the rings dont obstruct anything
if(!hasrun){ 
   
   if(!detecthome() ){
   
      gohome(); 
      
  }
}
  if(!hasrun && detecthome()){
    delay(60*SECOND);
  windthread();
  hasrun = true;
  }
  if(now.hour() == 8 && now.minute() == 30 && now.second() == 0){
    hasrun = false;
  }
  

}
