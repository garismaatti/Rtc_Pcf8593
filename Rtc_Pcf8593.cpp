/*****
 *  NAME
 *    Pcf8593 Real Time Clock support routines
 *  AUTHOR
 *    Joe Robertson, jmr
 *    orbitalair@bellsouth.net
 *    http://orbitalair.wikispaces.com/Arduino
 *  CREATION DATE
 *    9/24/06,  init - built off of usart demo.  using mikroC
 *  NOTES
 *  HISTORY
 *    10/14/06 ported to CCS compiler, jmr
 *    2/21/09  changed all return values to hex val and not bcd, jmr
 *    1/10/10  ported to arduino, jmr
 *    2/14/10  added 3 world date formats, jmr
 *    28/02/2012 A. Pasotti
 *             fixed a bug in RTCC_ALARM_AF,
 *             added a few (not really useful) methods
 *    09/10/2014 garismaatti
 *             rewrited fom Pcf8563 to Pcf8593 Real Time Clock
 *             https://github.com/garismaatti/Rtc_Pcf8593/
 *             removed square wave, not supported by Pcf8593
 *             added new alarm methods supported by Pcf8593
 *             timer is not supported, because used for saving year
 *
 *  TODO
 *    Add 24h/12h format
 *    Add timer support
 ******
 *  Robodoc embedded documentation.
 *  http://www.xs4all.nl/~rfsber/Robo/robodoc.html
 */

#include "Arduino.h"
#include "Wire.h"
#include "Rtc_Pcf8593.h"

Rtc_Pcf8593::Rtc_Pcf8593(void)
{
    Wire.begin();
    Rtcc_Addr = RTCC_R>>1;
}

void Rtc_Pcf8593::initClock()
{
  Wire.beginTransmission(Rtcc_Addr);    // Issue I2C start signal
  Wire.write((byte)0x0);     // start address

  Wire.write((byte)0x04);    //control/status1, reset value 0x04 
  Wire.write((byte)0x01);    //set hundredth seconds
  Wire.write((byte)0x01);    //set seconds
  Wire.write((byte)0x01);    //set minutes
  Wire.write((byte)0x01);    //set hour
  Wire.write((byte)0x01);    //set day, set year
  Wire.write((byte)0x01);    //set weekday, set month
  Wire.write((byte)0x01);    //set timer (year) to 1
  Wire.write((byte)0x0);    //set alarm control/status2
  Wire.write((byte)0x0);    //hundredth seconds alarm value reset to 00
  Wire.write((byte)0x0);    //seconds alarm value reset to 00
  Wire.write((byte)0x0);    //minute alarm value reset to 00
  Wire.write((byte)0x0);    //hour alarm value reset to 00
  Wire.write((byte)0x0);    //day alarm value reset to 00
  Wire.write((byte)0x0);    //month alarm value reset to 00
  Wire.write((byte)0x0);    //alarm timer off
  Wire.endTransmission();

}

/* Private internal functions, but useful to look at if you need a similar func. */
byte Rtc_Pcf8593::decToBcd(byte val)
{
  return ( (val/10*16) + (val%10) );
}

byte Rtc_Pcf8593::bcdToDec(byte val)
{
  return ( (val/16*10) + (val%16) );
}


void Rtc_Pcf8593::clearStatus()
{
  Wire.beginTransmission(Rtcc_Addr);      // Issue I2C start signal
  Wire.write((byte)0x0);
  Wire.write((byte)0x04);                 //control/status1
  Wire.endTransmission();
  
  Wire.beginTransmission(Rtcc_Addr);      // Issue I2C start signal
  Wire.write((byte)RTCC_STAT2_ADDR);
  Wire.write((byte)0x0);                  //alarm control/status2
  Wire.endTransmission();

  status1 = 0x04;
  status2 = 0x00;
}

void Rtc_Pcf8593::setTime(byte hour, byte minute, byte sec)
{
  Wire.beginTransmission(Rtcc_Addr);     // Issue I2C start signal
  Wire.write((byte)RTCC_SEC_ADDR);       // send addr low byte, req'd

  Wire.write((byte)decToBcd(sec));       //set seconds
  Wire.write((byte)decToBcd(minute));    //set minutes
  Wire.write((byte)decToBcd(hour));      //set hour
  Wire.endTransmission();
}

void Rtc_Pcf8593::setDate(byte day, byte weekday, byte mon, byte century, byte year)
{
    /* year val is 00 to 99, xx, saved in timer
        century is always = 20xx
       weekday is month 3 high bit
        */

    Wire.beginTransmission(Rtcc_Addr);       // Issue I2C start signal
    Wire.write((byte)RTCC_DAY_ADDR);
    Wire.write((byte)decToBcd(day));         //set day, year to 0
    mon = decToBcd(mon);
    weekday = decToBcd(weekday);
    weekday = weekday << 5;
    mon = mon | weekday;                     //compine weekday to month
    Wire.write((byte)mon);                   //set month and weekday
    Wire.endTransmission();
    
    Wire.beginTransmission(Rtcc_Addr);       // Issue I2C start signal
    Wire.write((byte)RTCC_YEAR_ADDR);
    Wire.write((byte)decToBcd(year));        //set year
    Wire.endTransmission();
}

/* enable alarm interrupt
 * whenever the clock matches these values an int will
 * be sent out pin 7 of the Pcf8593 chip
 */
void Rtc_Pcf8593::enableAlarm()
{
    Rtc_Pcf8593::readStatus1();		//read current status
    Rtc_Pcf8593::readStatus2();
    //set status1 AF val to zero
    status1 &= ~RTCC_ALARM_AF;
    //enable the interrupt
    status2 |= RTCC_ALARM_AIE;

    //clear alarm flag
    Wire.beginTransmission(Rtcc_Addr);  // Issue I2C start signal
    Wire.write((byte)RTCC_STAT1_ADDR);
    Wire.write((byte)status1);		//set status1
    Wire.endTransmission();
    
    //enable the interrupt
    Wire.beginTransmission(Rtcc_Addr);  // Issue I2C start signal
    Wire.write((byte)RTCC_STAT2_ADDR);
    Wire.write((byte)status2);		//set status2
    Wire.endTransmission();
}


/*
* Read status1 byte
*/
byte Rtc_Pcf8593::readStatus1()
{
    /* set the start byte of the status1 data */
    Wire.beginTransmission(Rtcc_Addr);
    Wire.write((byte)RTCC_STAT1_ADDR);
    Wire.endTransmission();

    Wire.requestFrom(Rtcc_Addr, 1); //request 1 bytes
    status1 = Wire.read();
    return status1;
}


/*
* Read status2 byte
*/
byte Rtc_Pcf8593::readStatus2()
{
    /* set the start byte of the status2 data */
    Wire.beginTransmission(Rtcc_Addr);
    Wire.write((byte)RTCC_STAT2_ADDR);
    Wire.endTransmission();

    Wire.requestFrom(Rtcc_Addr, 1); //request 1 bytes
    status2 = Wire.read();
    return status2;
}

/*
* Returns true if AIE is on
*
*/
boolean Rtc_Pcf8593::alarmEnabled()
{
    if (Rtc_Pcf8593::readStatus2() & RTCC_ALARM_AIE){
        return true;
    }else{
        return false;
    }
}

/*
* Returns true if AF is on
*
*/
boolean Rtc_Pcf8593::alarmActive()
{
    if (Rtc_Pcf8593::readStatus1() & RTCC_ALARM_AF){
        return true;
    }else{
        return false;
    }
}


/* set the alarm time values
 * whenever the clock matches these values an int will
 * be sent out pin 7 of the Pcf8593 chip
 */
void Rtc_Pcf8593::setAlarmTime(byte hour, byte min, byte sec)
{
    if (sec <99) {
        sec = constrain(sec, 0, 59);
        sec = decToBcd(sec);
    } else {
        sec = 0x0;
    }

    if (min <99) {
        min = constrain(min, 0, 59);
        min = decToBcd(min);
    } else {
        min = 0x0;
    }

    if (hour <99) {
        hour = constrain(hour, 0, 23);
        hour = decToBcd(hour);
    } else {
        hour = 0x0;
    }


    Wire.beginTransmission(Rtcc_Addr);    // Issue I2C start signal
    Wire.write((byte)RTCC_ALRM_HUND_SEC_ADDR);
    Wire.write((byte)0x0);                //hunred second alarm value to 00
    Wire.write((byte)sec);                //second alarm value reset to 00
    Wire.write((byte)min);                //minute alarm value reset to 00
    Wire.write((byte)hour);               //hour alarm value reset to 00
    Wire.endTransmission();
}


/* set the alarm date values
 * whenever the clock matches these values an int will
 * be sent out pin 7 of the Pcf8593 chip
 */
void Rtc_Pcf8593::setAlarmDate(byte day, byte month)
{
    if (day <99) {
        day = constrain(day, 1, 31);
        day = decToBcd(day);
    } else {
        day = 0x0;
    }

    if (month <99) {
        month = constrain(month, 1, 12);
        month = decToBcd(month);
    } else {
        month = 0x0;
    }


    Wire.beginTransmission(Rtcc_Addr);    // Issue I2C start signal
    Wire.write((byte)RTCC_ALRM_DAY_ADDR);
    Wire.write((byte)day);                //day alarm value
    Wire.write((byte)month);              //month alarm value
    Wire.endTransmission();
}


/* set the alarm weekday value
 * whenever the weekday matches bit place an int will
 * be sent out pin 7 of the Pcf8593 chip
 */
void Rtc_Pcf8593::setAlarmWeekday(byte weekday)
{
    /* bit0 = 0x01 = Sun
    *  bit1 = 0x02 = Mon
    *  bit2 = 0x04 = Tue
    *  bit3 = 0x08 = Wed
    *  bit4 = 0x10 = Thu
    *  bit5 = 0x20 = Fri
    *  bit6 = 0x40 = Sat
    *  bit7 = 0x80 = Not used
    * examples
    *  B01111111 = every day
    *  B01001001 = Sat, Wed & Sun
    */


    Wire.beginTransmission(Rtcc_Addr);    	// Issue I2C start signal
    Wire.write((byte)RTCC_ALRM_MONTH_ADDR);
    Wire.write((byte)weekday);                  //weekday alarm value reset to 00
    Wire.endTransmission();
}


/* set the alarm mode value
 */
void Rtc_Pcf8593::setAlarmMode(byte mode)
{
    Rtc_Pcf8593::readStatus2();
    status2 &= ~0x30;		//clear old value
    status2 |= mode;		//add new value

    Wire.beginTransmission(Rtcc_Addr);    // Issue I2C start signal
    Wire.write((byte)RTCC_STAT2_ADDR);
    Wire.write((byte)status2);            //set alarm mode
    Wire.endTransmission();
    
    if (mode == 0x00){		//enable or disable alarm
        Rtc_Pcf8593::clearAlarm();
    }else{
        Rtc_Pcf8593::enableAlarm();
    }
}

/**
* Get alarm values to local vars
*/
void Rtc_Pcf8593::getAlarm()
{
  
    // set the start byte of the alarm data
    Wire.beginTransmission(Rtcc_Addr);
    Wire.write((byte)RTCC_STAT2_ADDR);
    Wire.endTransmission();

    Wire.requestFrom(Rtcc_Addr, 7);		//request 6 bytes
    status2 = Wire.read();
    alarm_second = Wire.read();			//alarm hunred seconds is ignored
    alarm_second = bcdToDec(Wire.read());
    alarm_minute = bcdToDec(Wire.read());
    alarm_hour = bcdToDec(Wire.read());
    alarm_day = bcdToDec(Wire.read());
    alarm_weekday = Wire.read();		//alarm month and weekday are in the same place
    alarm_month = bcdToDec(alarm_weekday);
}

/**
* Reset the alarm leaving interrupt unchanged
*/
void Rtc_Pcf8593::resetAlarm()
{
    Rtc_Pcf8593::readStatus1();
    //set status1 AF val to zero to reset alarm
    status1 &= ~RTCC_ALARM_AF;
    Wire.beginTransmission(Rtcc_Addr);
    Wire.write((byte)RTCC_STAT1_ADDR);
    Wire.write((byte)status1);		//set status1
    Wire.endTransmission();
}

/**
* Remove the alarm
*/
void Rtc_Pcf8593::clearAlarm()
{
    Rtc_Pcf8593::readStatus1();		//read current status
    Rtc_Pcf8593::readStatus2();
    //set status2 AF val to zero to reset alarm
    status1 &= ~RTCC_ALARM_AF;
    //turn off the interrupt
    status2 &= ~RTCC_ALARM_AIE;

    Wire.beginTransmission(Rtcc_Addr);
    Wire.write((byte)RTCC_STAT1_ADDR);
    Wire.write((byte)status1);		//set status1
    Wire.endTransmission();

    Wire.beginTransmission(Rtcc_Addr);
    Wire.write((byte)RTCC_STAT2_ADDR);
    Wire.write((byte)status2);		//set status2
    Wire.endTransmission();
}

void Rtc_Pcf8593::getDate()
{
    /* set the start byte of the date data */
    Wire.beginTransmission(Rtcc_Addr);
    Wire.write((byte)RTCC_DAY_ADDR);
    Wire.endTransmission();

    Wire.requestFrom(Rtcc_Addr, 2); //request 2 bytes
    
    day = Wire.read();
    //get raw day data byte and (rolling) year with it.
    byte yearsPassed = day & RTCC_YEAR_MASK;
    yearsPassed = yearsPassed >> 6;
    yearsPassed = bcdToDec(yearsPassed);

    //0x3f = 0b00111111
    day = day & 0x3f;
    day = bcdToDec(day);
        
    month = Wire.read();
    //0xE0 = 0b11100000
    weekday = month & 0xE0;
    weekday = weekday >> 5;
    weekday = bcdToDec(weekday);
    //0x1f = 0b00011111
    month = month & 0x1f;
    month = bcdToDec(month);
    
    Wire.beginTransmission(Rtcc_Addr);
    Wire.write((byte)RTCC_YEAR_ADDR);
    Wire.endTransmission();

    Wire.requestFrom(Rtcc_Addr, 1);	//request 1 bytes
    year = bcdToDec(Wire.read());
    
    if (yearsPassed > 0){		//If year changed
      year = year +yearsPassed;		//Add passed year(s) to current year
      Wire.beginTransmission(Rtcc_Addr);    // Issue I2C start signal
      Wire.write((byte)RTCC_YEAR_ADDR);
      Wire.write((byte)decToBcd(year));     //set new year
      Wire.endTransmission();
      
      Wire.beginTransmission(Rtcc_Addr);    // Issue I2C start signal
      Wire.write((byte)RTCC_DAY_ADDR);
      Wire.write((byte)day);            //set day, (rolling) year to 0
      Wire.endTransmission();
    }
}

void Rtc_Pcf8593::getTime()
{
    /* set the start byte , get the status1 byte */
    Wire.beginTransmission(Rtcc_Addr);
    Wire.write((byte)RTCC_STAT1_ADDR);
    Wire.endTransmission();

    Wire.requestFrom(Rtcc_Addr, 5); //request 5 bytes
    status1 = Wire.read();
    hund_sec = Wire.read();
    sec = bcdToDec(Wire.read());
    minute = bcdToDec(Wire.read());
    hour = bcdToDec(Wire.read());
}

char *Rtc_Pcf8593::formatTime(byte style)
{
    getTime();
    switch (style) {
        case RTCC_TIME_HM:
            strOut[0] = '0' + (hour / 10);
            strOut[1] = '0' + (hour % 10);
            strOut[2] = ':';
            strOut[3] = '0' + (minute / 10);
            strOut[4] = '0' + (minute % 10);
            strOut[5] = '\0';
            break;
        case RTCC_TIME_HMS:
        default:
            strOut[0] = '0' + (hour / 10);
            strOut[1] = '0' + (hour % 10);
            strOut[2] = ':';
            strOut[3] = '0' + (minute / 10);
            strOut[4] = '0' + (minute % 10);
            strOut[5] = ':';
            strOut[6] = '0' + (sec / 10);
            strOut[7] = '0' + (sec % 10);
            strOut[8] = '\0';
            break;
        }
    return strOut;
}


char *Rtc_Pcf8593::formatDate(byte style)
{
    getDate();

        switch (style) {

        case RTCC_DATE_ASIA:
            //do the asian style, yyyy-mm-dd
            strDate[0] = '2';
            strDate[1] = '0';
            strDate[2] = '0' + (year / 10 );
            strDate[3] = '0' + (year % 10);
            strDate[4] = '-';
            strDate[5] = '0' + (month / 10);
            strDate[6] = '0' + (month % 10);
            strDate[7] = '-';
            strDate[8] = '0' + (day / 10);
            strDate[9] = '0' + (day % 10);
            strDate[10] = '\0';
            break;
        case RTCC_DATE_US:
        //the pitiful US style, mm/dd/yyyy
            strDate[0] = '0' + (month / 10);
            strDate[1] = '0' + (month % 10);
            strDate[2] = '/';
            strDate[3] = '0' + (day / 10);
            strDate[4] = '0' + (day % 10);
            strDate[5] = '/';
            strDate[6] = '2';
            strDate[7] = '0';
            strDate[8] = '0' + (year / 10 );
            strDate[9] = '0' + (year % 10);
            strDate[10] = '\0';
            break;
        case RTCC_DATE_WORLD:
        default:
            //do the world style, dd-mm-yyyy
            strDate[0] = '0' + (day / 10);
            strDate[1] = '0' + (day % 10);
            strDate[2] = '-';
            strDate[3] = '0' + (month / 10);
            strDate[4] = '0' + (month % 10);
            strDate[5] = '-';
            strDate[6] = '2';
            strDate[7] = '0';
            strDate[8] = '0' + (year / 10 );
            strDate[9] = '0' + (year % 10);
            strDate[10] = '\0';
            break;

    }
    return strDate;
}

byte Rtc_Pcf8593::getSecond() {
    return sec;
}

byte Rtc_Pcf8593::getMinute() {
    return minute;
}

byte Rtc_Pcf8593::getHour() {
    return hour;
}

byte Rtc_Pcf8593::getAlarmSecond() {
    return alarm_second;
}

byte Rtc_Pcf8593::getAlarmMinute() {
    return alarm_minute;
}

byte Rtc_Pcf8593::getAlarmHour() {
    return alarm_hour;
}

byte Rtc_Pcf8593::getAlarmDay() {
    return alarm_day;
}

byte Rtc_Pcf8593::getAlarmMonth() {
    return alarm_month;
}

byte Rtc_Pcf8593::getAlarmWeekday() {
    return alarm_weekday;
}

byte Rtc_Pcf8593::getDay() {
    return day;
}

byte Rtc_Pcf8593::getMonth() {
    return month;
}

byte Rtc_Pcf8593::getYear() {
    return year;
}

byte Rtc_Pcf8593::getWeekday() {
    return weekday;
}

byte Rtc_Pcf8593::getStatus1() {
    return status1;
}

byte Rtc_Pcf8593::getStatus2() {
    return status2;
}
