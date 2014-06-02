/*

Scout Syslog Logger

A Web-based log4j-style logger for Arduino Pinoccio Scout apps

by Frank Cohen, fcohen@votsh.com

Provided by Votsh, http://www.votsh.com

Copyright (c) 2014 Frank Cohen. All rights reserved.
GPL v3 license:
This file is part of A Transfer Protocol (ATP)
ATP is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
VOS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with ATP.  If not, see <http://www.gnu.org/licenses/>.

Based on https://github.com/astagi/Arduino-logging
*/

#ifndef _ScoutSyslogLogger_H_
#define _ScoutSyslogLogger_H_

#include <inttypes.h>
#include <stdarg.h>
#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif
//#include "pins_arduino.h"
extern "C" {
  #include <avr/io.h>
}

#include "ScoutSyslogLoggerWifiDriver.h"

#define LOG_LEVEL_NOOUTPUT 0 
#define LOG_LEVEL_ERRORS 1
#define LOG_LEVEL_INFOS 2
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_DEBUG 3
#define LOG_LEVEL_VERBOSE 4

#define LOG_WIFI 0
#define LOG_BLE 1
#define LOG_SERIAL 2

// facility
#define kern 0
#define user 1
#define mail 2
#define daemon 3
#define authen 4    // had to change this, conflicts with auth reserved word
#define syslog 5
#define lpr 6
#define news 7
#define uucp 8
#define cron 9
#define authpriv 10
#define ftp 11
#define local0 16
#define local1 17
#define local2 18
#define local3 19
#define local4 20
#define local5 21
#define local6 22
#define local7 23

// level
#define emerg 0
#define alert 1
#define crit 2
#define err 3
#define warning 4
#define notice 5
#define info 6
#define debug 7
#define ludicrous 8 (Spaceballs)

// default loglevel if nothing is set from user
#define LOGLEVEL LOG_LEVEL_DEBUG 

#define CR "\r\n"
#define LOGGING_VERSION 1

/*!
* Logging is a helper class to output app operation status to a syslog service
* If you know log4j or log4net, this logging class is more or less similar ;-)
*
* Different loglevels can be used to extend or reduce output
* All methods are able to handle any number of output parameters.
* All methods print out a formated string (like printf).<br>
* To reduce output and program size, reduce loglevel.
*
* Output format string can contain below wildcards. Every wildcard
* must be start with percent sign (\%)
*
* <b>Depending on loglevel, source code is excluded from compile !</b><br>
* <br>
* <b>Wildcards</b><br>
* <ul>
* <li><b>\%s</b>	replace with an string (char*)</li>
* <li><b>\%c</b>	replace with an character</li>
* <li><b>\%d</b>	replace with an integer value</li>
* <li><b>\%l</b>	replace with an long value</li>
* <li><b>\%x</b>	replace and convert integer value into hex</li>
* <li><b>\%X</b>	like %x but combine with <b>0x</b>123AB</li>
* <li><b>\%b</b>	replace and convert integer value into binary</li>
* <li><b>\%B</b>	like %x but combine with <b>0b</b>10100011</li>
* <li><b>\%t</b>	replace and convert boolean value into <b>"t"</b> or <b>"f"</b></li>
* <li><b>\%T</b>	like %t but convert into <b>"true"</b> or <b>"false"</b></li>
* </ul><br>
* <b>Loglevels</b><br>
* <table border="0">
* <tr><td>0</td><td>LOG_LEVEL_NOOUTPUT</td><td>no output </td></tr>
* <tr><td>1</td><td>LOG_LEVEL_ERRORS</td><td>only errors </td></tr>
* <tr><td>2</td><td>LOG_LEVEL_INFOS</td><td>errors and info </td></tr>
* <tr><td>3</td><td>LOG_LEVEL_DEBUG</td><td>errors, info and debug </td></tr>
* <tr><td>4</td><td>LOG_LEVEL_VERBOSE</td><td>all </td></tr>
* </table>
* <br>
* <h1>History</h1><br>
* <table border="0">
* <tr><td>01 FEB 2012</td><td>initial release</td></tr>
* <tr><td>06 MAR 2012</td><td>implement a preinstanciate object (like in Wire, ...)</td></tr>
* <tr><td></td><td>methode init get now loglevel and baud parameter</td></tr>
*/

class Logging {

private:
    int _level;
    long _baud;
    int _consoleOption;
    int _driver;
    
public:
    /*! 
	 * default Constructor
	 */
    Logging(){} ;
	
    /** 
	* Initializing, must be called as first.
	* \param void
	* \return void
	* \ip address of syslog server
	* \port of syslog server, normally 514
	* \driver LOG_WIFI, LOG_BLE or others
	* \consoleOption If LOG_CONSOLE then logs to the Serial monitor too
	* \facility syslog identifier
	* \machine Scout name
	* \procid Process number, default is 0
	* \appname App name
	*/

	void setup(int level, long baud,
	 int ip0, int ip1, int ip2, int ip3,
	 int port,
	 int driver,
	 int consoleOption,
	 int facility,
	 char * machine,
	 int procid,
	 char * appname
	);
	
	/**
	* loop, processed message reception over LWM from the Scouts
	*/
	
	void handleMessages();	
	
    /**
	* Output an error message. Output message contains
	* ERROR: followed by original msg
	* Error messages are printed out, at every loglevel
	* except 0 ;-)
	* \param msg format string to output
	* \param ... any number of variables
	* \return void
	*/
    void Error(char* msg, ...);
	
    /**
	* Output an info message. Output message contains
	* Info messages are printed out at l
	* loglevels >= LOG_LEVEL_INFOS
	*
	* \param msg format string to output
	* \param ... any number of variables
	* \return void
	*/

   void Info(char* msg, ...);
	
    /**
	* Output an debug message. Output message contains
	* Debug messages are printed out at l
	* loglevels >= LOG_LEVEL_DEBUG
	*
	* \param msg format string to output
	* \param ... any number of variables
	* \return void
	*/

    void Debug(char* msg, ...);
	
    /**
	* Output an verbose message. Output message contains
	* Debug messages are printed out at l
	* loglevels >= LOG_LEVEL_VERBOSE
	*
	* \param msg format string to output
	* \param ... any number of variables
	* \return void
	*/

    void Verbose(char* msg, ...);   

    
private:
    void print(int msgtype, const char *format, va_list args);
};

extern Logging Log;
#endif




