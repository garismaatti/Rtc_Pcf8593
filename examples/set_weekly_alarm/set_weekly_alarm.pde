/* Demonstration of Rtc_Pcf85893 Weekday alarm.
 *
 * The Pcf8593 has an interrupt output, Pin7.
 * Pull Pin3 HIGH with a resistor, I used a 10kohm to 5v.
 * I used a RBBB with Arduino IDE, the pins are mapped a
 * bit differently.  Change for your hw.
 * SCK - A5, SDA - A4, INT - D3/INT1
 *
 * After loading and starting the sketch, use the serial monitor
 * to see the clock output.
 *
 * setup:  see Pcf8593 data sheet.
 *         1x 10Kohm pullup on Pin3 RESET
 *         No pullups on Pin1 or Pin2 (I2C internals used)
 *         1x 0.1pf on power
 *         1x 32khz chrystal
 *
 * Joe Robertson, jmr orbitalair@bellsouth.net
 * Modified by garismaatti garismaatti@gmail.com to work with Pcf8593
 */
#include <Wire.h>
#include <Rtc_Pcf8593.h>

/* get a real time clock object */
Rtc_Pcf8593 rtc;

/* a flag for the interrupt */
volatile int alarm_flag=0;

/* the interrupt service routine */
void blink()
{
  alarm_flag=1;
}

void setup()
{
  pinMode(3, INPUT);           // set pin to input
  digitalWrite(3, HIGH);       // turn on pullup resistors

  Serial.begin(9600);

  /* setup int on pin 3 of arduino */
  attachInterrupt(1, blink, FALLING);
  /* clear out all the registers */
  rtc.initClock();
  /* set a time to start with.
   * day, weekday(0=Sun), month, century, year */
  rtc.setDate(14, 3, 6, 0, 14);
  /* hr, min, sec */
  rtc.setTime(1, 15, 40);
  /* set an alarm for 20 secs later...
   * alarm pin goes low when match occurs
   * this triggers the interrupt routine
   * hr, min, sec */
  rtc.setAlarmTime(1, 16, 00);
  /* set alarm weekdays
   * B00000001 = Sun
   * B00000010 = Mon
   * B00000100 = Tue
   * ...
   * B00000011 = Sun + Mon
   */
  rtc.setAlarmWeekday(B00111110);
  rtc.setAlarmMode(RTCC_ALARM_WEEKDAY);
}

void loop()
{
  /* each sec update the display */
  Serial.print(rtc.formatTime());
  Serial.print("  ");
  Serial.print(rtc.formatDate(RTCC_DATE_WORLD));
  Serial.print("  0x");
  Serial.print(rtc.alarmEnabled());
  Serial.print("  0x");
  Serial.print(rtc.alarmActive());
/*  // print alarm time
  Serial.print("  ");	
  rtc.getAlarm();
  Serial.print(rtc.getAlarmHour());
  Serial.print(":");
  Serial.print(rtc.getAlarmMinute());
  Serial.print(":");
  Serial.print(rtc.getAlarmSecond());
  Serial.print("  B");
  Serial.print(rtc.getAlarmWeekday(), BIN);*/
  Serial.print("\r\n");
  delay(1000);
  if (alarm_flag==1){
    clr_alarm();
  }

}

void clr_alarm()
{
  detachInterrupt(1);
  Serial.print("blink!\r\n");

  rtc.resetAlarm();	//clear INT and wait for tomorrow
  //rtc.clearAlarm();	//remove alarm
  delay(1000);
  alarm_flag=0;
  attachInterrupt(1, blink, FALLING);
}

