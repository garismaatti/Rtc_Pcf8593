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

#ifndef Rtc_Pcf8593_H
#define Rtc_Pcf8593_H

#include "Arduino.h"
#include "Wire.h"

/* the read and write values for Pcf8593 rtcc */
/* these are adjusted for arduino */
#define RTCC_R 	0xa3
#define RTCC_W 	0xa2

/* register addresses in the rtc */
#define RTCC_STAT1_ADDR			0x0
#define RTCC_HUND_SEC_ADDR		0x01
#define RTCC_SEC_ADDR  			0x02
#define RTCC_MIN_ADDR 			0x03
#define RTCC_HR_ADDR 			0x04
#define RTCC_DAY_ADDR 			0x05  // also 2bit year, used to roll year in timer
#define RTCC_MONTH_ADDR 		0x06  // also weekday
#define RTCC_YEAR_ADDR 			0x07  // timer, but used for year
#define RTCC_STAT2_ADDR			0x08  // alarm control/status2
#define RTCC_ALRM_HUND_SEC_ADDR         0x09
#define RTCC_ALRM_SEC_ADDR 		0x0A
#define RTCC_ALRM_MIN_ADDR 		0x0B
#define RTCC_ALRM_HR_ADDR 		0x0C
#define RTCC_ALRM_DAY_ADDR 		0x0D
#define RTCC_ALRM_MONTH_ADDR 		0x0E  // also used weekday alarm

/* setting the alarm flag to 1 enables the alarm.
 * set it to 0 to disable the alarm for that value.
 */
#define RTCC_ALARM			0x04  // Alarm register enable (default on), status1
#define RTCC_ALARM_AIE 			0x80  // Alarm Interrupt Enable, status2
#define RTCC_ALARM_AF 			0x02  // Alarm Flag, status1
#define RTCC_ALARM_DISABLED		0x00  // Alarm mode disabled, status2
#define RTCC_ALARM_DAILY		0x10
#define RTCC_ALARM_WEEKDAY		0x20
#define RTCC_ALARM_DATED		0x30


/* optional val for no alarm setting */
#define RTCC_NO_ALARM			99

#define RTCC_YEAR_MASK                  0xC0  // used to roll years in timer

/* date format flags */
#define RTCC_DATE_WORLD			0x01
#define RTCC_DATE_ASIA			0x02
#define RTCC_DATE_US			0x04
/* time format flags */
#define RTCC_TIME_HMS			0x01
#define RTCC_TIME_HM			0x02





/* arduino class */
class Rtc_Pcf8593 {
	public:
		Rtc_Pcf8593();

		void initClock();	/* zero out all values, disable all alarms */
		void clearStatus();	/* set both status bytes to zero */

		void getDate();		/* get date vals to local vars */
		void setDate(byte day, byte weekday, byte month, byte century, byte year);
		void getTime();    	/* get time vars + status1 byte to local vars */
		//void getAlarm();
		void setTime(byte sec, byte minute, byte hour);
		byte readStatus1();	/* get status1 byte */
		byte readStatus2();	/* get status2 byte */
		boolean alarmEnabled();
        	boolean alarmActive();

        	void enableAlarm(); 	/* activate alarm flag and interrupt */
		void setAlarmTime(byte hour, byte min, byte sec);
		void setAlarmDate(byte day, byte month);
		void setAlarmWeekday(byte weekday);
		void setAlarmMode(byte mode); /* set alarm mode, off/daily/weekly/dated */
		void getAlarm();	/* get alarm vals to local vars */
		void clearAlarm();	/* clear alarm flag and interrupt */
		void resetAlarm();  	/* clear alarm flag but leave interrupt unchanged */

		byte getSecond();
		byte getMinute();
		byte getHour();
		byte getDay();
		byte getMonth();
		byte getYear();
		byte getWeekday();
		byte getStatus1();
		byte getStatus2();

		byte getAlarmSecond();
		byte getAlarmMinute();
		byte getAlarmHour();
		byte getAlarmDay();
		byte getAlarmMonth();
		byte getAlarmWeekday();

		/*get a output string, these call getTime/getDate for latest vals */
		char *formatTime(byte style=RTCC_TIME_HMS);
		/* date supports 3 styles as listed in the wikipedia page about world date/time. */
		char *formatDate(byte style=RTCC_DATE_US);

	private:
		/* methods */
		byte decToBcd(byte value);
		byte bcdToDec(byte value);
		/* time variables */
		byte hour;
		byte minute;
		byte sec;
		byte hund_sec;
		byte day;
		byte weekday;
		byte month;
		byte year;
		/* alarm */
		byte alarm_second;
		byte alarm_minute;
		byte alarm_hour;
		byte alarm_weekday;
		byte alarm_day;
		byte alarm_month;
		/* support */
		byte status1;
		byte status2;

		char strOut[9];
		char strDate[11];

		int Rtcc_Addr;
};

#endif
