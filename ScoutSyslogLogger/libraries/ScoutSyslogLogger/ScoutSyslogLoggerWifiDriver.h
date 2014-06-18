/*

Scout Syslog Logger Wifi Driver

A Web-based log4j-style logger for Arduino Pinoccio Scout apps
Driver communicates to a syslog service over Wifi

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
*/

#ifndef _ScoutSyslogLoggerWifiDriver_H_
#define _ScoutSyslogLoggerWifiDriver_H_

#include <GS.h>
#include <SPI.h>
#include <Scout.h>
#include <Wire.h>
#include <bitlash.h>
#include <lwm.h>
#include <js0n.h>

class ScoutSyslogLoggerWifiDriver
{
  private:
    int _ip0;
    int _ip1;
    int _ip2;
    int _ip3;
    int _consoleOption;
    int _port;
    int _driver;
    int _facility;
    int _procid;
    String _appname;
    String _machinename;
	int _level;
	int _max_queue_size;
	
	int connected;
	int isLeadScout;
  	int sending;
  	
  	char msgbuf[1000];
  	
  public:
   ScoutSyslogLoggerWifiDriver( void );

	void setup( int ip0,int ip1,int ip2,int ip3, int port, int driver,
	 int facility, int procid, char * machinename, char * appname,
	 int max_queue_size );

	void sendMsg( String message, int msgtype );
	
	void loop();
	
	void PrintSendList();
	
	void PrintReceiveList();	
	
};


#endif

