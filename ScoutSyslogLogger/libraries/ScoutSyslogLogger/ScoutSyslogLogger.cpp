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

#include "ScoutSyslogLogger.h"

ScoutSyslogLoggerWifiDriver wifidriver;
//ScoutSyslogLoggerBLEDriver bledriver;    // Coming later, I hope

void Logging::setup(int level, long baud, 
		int ip0, int ip1, int ip2, int ip3,
		int port, int driver, int consoleOption, int facility, 
		char * machinename, int procid, char * appname )
{
    _level = constrain(level,LOG_LEVEL_NOOUTPUT,LOG_LEVEL_VERBOSE);
    _consoleOption = consoleOption;
    _level = level;
    _baud = baud;
    _driver = driver;

	if ( _consoleOption == LOG_SERIAL )
	{
	    Serial.begin(_baud);
	}

	if ( _driver == LOG_WIFI )
	{
		// Set up bridge to talk to Gainspan library and to syslog over wifi

		wifidriver = ScoutSyslogLoggerWifiDriver();
		wifidriver.setup( ip0,ip1,ip2,ip3, port, driver, facility, 
		procid, machinename, appname );
	}

	if ( _driver == LOG_BLE )
	{
		// Setup channel to talk BLE to the syslog
		
		// Not yet, coming soon to a Votsh Wave near you, http://www.votsh.com		
	}
}

/**
* loop, processed message reception over LWM from the Scouts
*/

void Logging::handleMessages()
{
	wifidriver.handleLWM();
}

void Logging::Error(char* msg, ...){
    if (LOG_LEVEL_ERRORS <= _level) {   
        va_list args;
        va_start(args, msg);
        print(LOG_LEVEL_ERRORS, msg,args);
    }
}


void Logging::Info(char* msg, ...){
    if (LOG_LEVEL_INFOS <= _level) {
        va_list args;
        va_start(args, msg);
        print(LOG_LEVEL_INFO, msg,args);
    }
}

void Logging::Debug(char* msg, ...){
    if (LOG_LEVEL_DEBUG <= _level) {
        va_list args;
        va_start(args, msg);
        print(LOG_LEVEL_DEBUG, msg,args);
    }
}


void Logging::Verbose(char* msg, ...){
    if (LOG_LEVEL_VERBOSE <= _level) {
        va_list args;
        va_start(args, msg);
        print(LOG_LEVEL_VERBOSE, msg,args);
    }
}

void Logging::print(int msgtype, const char *format, va_list args ) {
    //
    // loop through format string
    
    String m = String("");
    
    for (; *format != 0; ++format) {
        
        if (*format == '%') 
        {
            ++format;

            if (*format == '\0') break;

            if (*format == '%') {
                //Serial.print(*format,3);
                m+= String( *format,3 );
                continue;
            }
            if( *format == 's' ) {
				register char *s = (char *)va_arg( args, int );
				//Serial.print(s);
                m+= String( s );
				continue;
			}
            if( *format == 'd' || *format == 'i') {
				//Serial.print(va_arg( args, int ),DEC);
                m+= String( va_arg( args, int ),DEC );
				continue;
			}
            if( *format == 'x' ) {
				int val = va_arg( args, int );           	
     			if ( val < 16 ) { Serial.print("0"); }
				Serial.print( val, HEX);
				continue;
			}
            if( *format == 'X' ) {
				int val = va_arg( args, int );           	
     			if ( val < 16 ) { Serial.print("0x0"); } else { Serial.print("0x"); }
				Serial.print( val,HEX);
				continue;
			}
            if( *format == 'b' ) {
				//Serial.print(va_arg( args, int ),BIN);
                m+= String( va_arg( args, int ), BIN );
				continue;
			}
            if( *format == 'B' ) {
				//Serial.print("0b");
				//Serial.print(va_arg( args, int ),BIN);
				m+= String("0b");
                m+= String( va_arg( args, int ), BIN );
				continue;
			}
            if( *format == 'l' ) {
				//Serial.print(va_arg( args, long ),DEC);
                m+= String( va_arg( args, long ), DEC );
				continue;
			}
            if( *format == 'c' ) {
				//Serial.print(va_arg( args, int ));
                m+= String( va_arg( args, int ) );
				continue;
			}
            if( *format == 'f' ) {
				//Serial.print(va_arg( args, double));
                m+= String( va_arg( args, int ) );
				continue;
			}
            if( *format == 't' ) {
				if (va_arg( args, int ) == 1) {
					//Serial.print("T");
	                m+= String( "T" );
				}
				else {
					//Serial.print("F");				
	                m+= String( "F" );
				}
				continue;
			}
            if( *format == 'T' ) {
				if (va_arg( args, int ) == 1) {
					//Serial.print("true");
	                m+= String( "T" );
				}
				else {
					//Serial.print("false");				
	                m+= String( "F" );
				}
				continue;
			}
        }
        
        m+= String( *format );            
    }
    
	if ( _consoleOption == LOG_SERIAL )
	{
		Serial.print( m );
	}
	
	if ( _driver == LOG_WIFI )
	{
		wifidriver.sendMsg( m, msgtype );			
	}
	
	if ( _driver == LOG_BLE )
	{
		Serial.println("ScoutSysLogger BLE not implemented.");	
	}
    
 }
 
Logging Log = Logging();

