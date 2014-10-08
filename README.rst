
This is my rewrited version of the original Rtc_Pcf8563 library for Arduino.
All credits go to the original authors.

My version differs for

* Rewrited everyting to be for Pcf8593 Real Time Clock (Not Pcf8563 anymore)
* Removed square wave, not supported by Pcf8593
* Added new alarm methods supported by Pcf8593 (daily, weekdays, dated)
* Timer is not supported, because used for saving year
* 


NAME
----
Pcf8593 Real Time Clock support routines

AUTHOR
------
Joe Robertson, jmr
orbitalair@bellsouth.net
http://orbitalair.wikispaces.com/Arduino

CREATION DATE
-------------
9/24/06,  init - built off of usart demo.  using mikroC

NOTES
-----

HISTORY
-------

* 10/14/06 ported to CCS compiler, jmr
* 2/21/09  changed all return values to hex val and not bcd, jmr
* 1/10/10  ported to arduino, jmr
* 2/14/10  added 3 world date formats, jmr
* 28/02/2012 A. Pasotti
   * fixed a bug in RTCC_ALARM_AF,
   * added a few (not really useful) methods
* 09/10/2014 garismaatti
   * rewrited fom Pcf8563 to Pcf8593 Real Time Clock
   * https://github.com/garismaatti/Rtc_Pcf8593/
   * removed square wave, not supported by Pcf8593
   * added new alarm methods supported by Pcf8593
   * timer is not supported, because used for saving year


TODO
----
Add 24h/12h format
Add timer support
