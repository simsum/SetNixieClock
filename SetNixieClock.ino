//  Set RTC V2.0.1 by Sven Steinmeier - sven.steinmeier@gmail.com
//  2014-05-08
//  Changelog: V2.0.1
//  Low mem fix (ATmega168)
//  Changelog: V2.0
//  switch Library Time.h 
//  switch Library DS1307RTC.h (old: ds1307.h)
//  switch Library Wire.h
//  include some new function form Sample Sketch (getTime, getDate)
//  Fix workaround for daylightsaving

#include <Time.h>
#include <DS3232RTC.h>  //Arduino Pro Mini I2C: A4 (SDA) und A5 (SCL)
#include <Wire.h>
#include <math.h>

const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

unsigned long unixtime; //unsigned! long to fix the year 2038 problem
tmElements_t tm;

/* Info tmElements_t
tm.Second  Seconds   0 to 59
tm.Minute  Minutes   0 to 59
tm.Hour    Hours     0 to 23
tm.Wday    Week Day  0 to 6  (Sunday is day 0, not needed for mktime)
tm.Day     Day       1 to 31
tm.Month   Month     1 to 12
tm.Year    Year      0 to 99 (offset from 1970)
*/

void setup()
{
  // Setup Serial connection
  Serial.begin(9600);

  // Date and Time convert
  getDate(__DATE__);
  getTime(__TIME__);


  /*TEST
  tmIDE.Hour = 0;
  tmIDE.Minute = 1;
  tmIDE.Second = 5;
  tmIDE.Day = 31;
  tmIDE.Month = 3;
  tmIDE.Year = CalendarYrToTm(2014);
  */

  //Calculate unix time format
  unixtime = makeTime(tm); 

  // Daylight saving time activ (false = standard time, true = daylight saving time +1)
  bool DaylightSavingAtSetup = DST(tmYearToCalendar(tm.Year), tm.Month, tm.Day, tm.Hour, tm.Minute);
  // DaylightSaving adjustment, RTC should work with standard time
  if (DaylightSavingAtSetup) unixtime = unixtime - 3600;

  // Debug Print
  while (!Serial); // wait for Arduino Serial Monitor
  delay(1000);
  /*
  Serial.print("Year:                          ");
  Serial.println(tmYearToCalendar(tm.Year));
  Serial.print("Month:                         ");
  Serial.println(tm.Month);
  Serial.print("Day:                           ");
  Serial.println(tm.Day);
  Serial.print("Hour:                          ");
  Serial.println(tm.Hour);
  Serial.print("Minute:                        ");
  Serial.println(tm.Minute);
  Serial.print("Second:                        ");
  Serial.println(tm.Second);
  Serial.print("Daylight saving time at setup: ");
  Serial.println(DaylightSavingAtSetup);
  Serial.print("Unix-Time (after adjustment):  ");
  Serial.println(unixtime);
  Serial.println();
  */
  // RTC write
  breakTime(unixtime, tm);
  RTC.write(tm);
  delay(2000);
}

void loop()
{
  // Get data from RTC

 // if (RTC.read(tm)) {
    RTC.read(tm);
    Serial.print("Year:   ");
    Serial.println(tmYearToCalendar(tm.Year));
    Serial.print("Month:  ");
    Serial.println(tm.Month);
    Serial.print("Day:    ");
    Serial.println(tm.Day);
    Serial.print("Hour:   ");
    Serial.println(tm.Hour);
    Serial.print("Minute: ");
    Serial.println(tm.Minute);
    Serial.print("Second: ");
    Serial.println(tm.Second);
    float c = RTC.temperature() / 4.;
    Serial.print("Temperature: ");
    Serial.println(c);
 // } else {
   /* if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    delay(9000);*/
  //}
  

  Serial.println(fmod(5.5,2.5));
  delay(1000);
}

bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}


bool getDate(const char *str)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}

