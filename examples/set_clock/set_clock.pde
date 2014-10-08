/* Demonstration of Rtc_Pcf8593 Set Time. 
 *
 * Set the clock to a time then loop over reading time and 
 * output the time and date to the serial console.
 *
 * I used a RBBB with Arduino IDE, the pins are mapped a 
 * bit differently. Change for your hw
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

//init the real time clock
Rtc_Pcf8593 rtc;

void setup()
{
  //clear out the registers
  rtc.initClock();
  //set a time to start with.
  //day, weekday(0=Sun), month, century(always=20xx), year(0-99)
  rtc.setDate(14, 6, 3, 0, 14);
  //hr, min, sec
  rtc.setTime(1, 15, 0);
}

void loop()
{
  //both format functions call the internal getTime() so that the 
  //formatted strings are at the current time/date.
  //Serial.print(rtc.formatTime(RTCC_TIME_HM));
  Serial.print(rtc.formatTime());
  Serial.print("\r\n");
  //Serial.print(rtc.formatDate(RTCC_DATE_ASIA));
  Serial.print(rtc.formatDate());
  Serial.print("\r\n");
  delay(1000);
}




