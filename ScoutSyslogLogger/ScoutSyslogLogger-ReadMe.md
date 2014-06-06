#ScoutSyslogLogger

##A Web-based Log4j-style Logger for Arduino Pinoccio Scout Apps

v .1, June 2, 2014, [https://github.com/Votsh/waves](https://github.com/Votsh/waves)

###Background

ScoutSyslogLogger comes from [Votsh](http://votsh.com). Votsh is a start-up company making entertaining machines to bring meaning to people's lives. The Votsh [Waves](http://votsh.com) project is perhaps a modern lava lamp, and maybe light therapy with a speaker. It uses Arduino Pinoccio technology in a distributed entertainment system running light, sound, and movement shows that span multiple networked boxes. While the Arduino IDE Serial Monitor is a fine tool to get started, in a distributed environment developers require sophisticated software development tools, including advanced and remote logging services.

One thing that bugs me every build is having to open the serial monitors on the target boards. The serial port is used to upload a program/sketch to the board. I find myself having to close the monitors after each compile/upload operation, and then re-open the monitor. Takes time and concentration.

ScoutSyslogLogger is a syslogd based logger utility. The advantages of this design are:

1) It scales to support dozens to hundreds of Scout devices

2) Simple Web-based view of log entries, no more opening the Arduino IDE Serial Monitor

3) Compatible with other logging utilities, including [LogEntries](http://logentries.com)

###Logging
ScoutSyslogLogger is for software developers working on [Arduino Pinoccio]("http://www.pinnoc.io") projects. Log to a central log manager and view the log from a Web page. ScoutSyslogLogger works from any Arduino sketch, C and C++ library. It runs in the background of your app and does not interfere with Pinoccio's operations. ScoutSyslogLogger functions with hq.pinocc.io and Bitlash functions. You can even continue using the Arduino IDE Serial Monitor.

```
Log.Info( "Hull breech detected on deck %d", 4 );
```

It does not matter which Scout you are logging from, ScoutSyslog delivers the log message to the Syslog server for display in a Web page.

<img style="width: 944px; height: 56px;" alt=""src="https://votsh.files.wordpress.com/2014/06/logcloseup.jpg">

The Web page includes tail -f to show the most recent log entries. The page  provides a filter system to search the log entries for values. The page is configureable to include Syslog streams from multiple nodes.

<img style="width: 793px; height: 502px;" alt="Web page viewer for logs"src="https://votsh.files.wordpress.com/2014/06/scoutsyslogger_example.jpg">

The system makes log entries to the server Syslog messages file. The Web page shows the most recent log entries - in a tail -f style.

##Configuration

ScoutSyslogLogger is compatible with Pinoccio Scout sketches and libraries. Write your sketch in Arduino IDE or your favorite editor. Ince the following libraries.

```
#include <Arduino.h>    // Arduino main libraries
#include <SPI.h>        // Serial Device Interface 
#include <Wire.h>       // TWI/I2C library for Wiring & Arduino
#include <Scout.h>      // Pinoccio Scout APIs, including setup and loop, mostly hardware related
#include <GS.h>         // HAL driver to talk with Gainspan GS1011 WiFi module
#include <bitlash.h>    // Tiny language interpreter that provides a serial port 
                        // shell environment for bit banging and hardware hacking.
#include <lwm.h>        // Atmel Light Weight Mesh radio network
#include <js0n.h>       // Javascript Object Notation JSON library 
#include "key/key.h"    // Key utilities

#include "ScoutSyslogLogger.h"    // Web-based log4j-style logger
```

Then change your setup to include:

```
  // See ScoutSyslogLogger.h for definitions
  // ip address of syslog server
  // port of syslog server, normally 514
  // driver LOG_WIFI, LOG_BLE or others
  // consoleOption If LOG_CONSOLE then logs to the Serial monitor too
  // facility syslog identifier
  // machine Scout name
  // procid Process number, default is 0
  // appname App name

  Log.setup( 
    LOG_LEVEL_DEBUG,
    baudrate,
    54,209,5,236,
    514,
    LOG_WIFI,
    LOG_SERIAL,
    user,
    "wave2",
    0,
    "ScoutSyslogLoggerExample" ); 
```

Change the loop to include:

```
void loop() {
  Scout.loop();          // Pinoccio Scout libraries process their tasks
  Log.handleMessages();	 // ScoutSyslogLogger handles log messages
```

In your code, add 

```
  Log.Info( "Hull breech detected on deck %d", 4 );      
```

###Arduino IDE Configuration

1. Copy libraries/ScoutSyslogLogger/* to your Arduino libraries directory. On a Mac this is ~/Documents/Arduino/libraries
2. Open ScoutSyslogLoggerExample.ino in Arduino IDE.
3. Compile


###Ports and Firewall Configuration

Syslog on Linux runs on port 514 by default.

Logit runs on port 27288 by default.

The following IPTables rules provide access to these ports:


On Redhat (RHEL) version 6 use:

```
/etc/sysconfig/iptables
-A INPUT -m state --state NEW -m tcp -p tcp --dport 28778 -j ACCEPT
-A INPUT -p udp -m udp --dport 514 -j ACCEPT
-A INPUT -p tcp -m tcp --dport 514 -j ACCEPT

```
Then restart iptables using ```/etc/init.d/iptables restart```

###syslogd with UDP reception enabled

On a Redhat (RHEL) environment, at the top of the file “/etc/rsyslog.conf”, at the top of the file, change the below lines

```
# Provides UDP syslog reception
#$ModLoad imudp
#$UDPServerRun 514

# Provides TCP syslog reception
#$ModLoad imtcp
#$InputTCPServerRun 514
```
to this

```
# Provides UDP syslog reception
$ModLoad imudp
$UDPServerRun 514

# Provides TCP syslog reception
$ModLoad imtcp
$InputTCPServerRun 514
```

Once you have saved your changes, restart the rsyslog service

```
[root@syslog ~]# service rsyslog restart
Shutting down system logger: [ OK ]
Starting system logger: [ OK ]
[root@syslog ~]#
```


##Log4J Logging Styles

[Apache log4j](http://logging.apache.org/log4j/2.x/) is a popular Java-based logging utility. Java developers like it because they insert logging commands within their source code. The commands indicate the severity of the issue being logged. A global property determines what level of severity appears in the log.

```
void myFunction( int theval ){

	if ( theval < 100 ){
		Log.Error( "Houston, we have a %s", "problem" ); 
		return;
	}

	if ( theval = 500 ){
      Log.Info( "Hull breech detected on deck %d", 4 );
      processTheValue( theval );      
	}

    Log.Debug( "Finished running myFunction" );      
}
```
Setting the logger to log at a Info level gives:

```
Info: Hull breech detected on deck 4
```

Setting the logger to log at the Debug level gives both the Info and Debug log entries:

```
Info: Hull breech detected on deck 4
Debug: Finished running myFunction
```

###Printf Style

ScoutSyslogLogger supports [printf](http://www.cplusplus.com/reference/cstdio/printf/) style logging.

```
Log.Info("Display my integers myInt1 %d, myInt2 %d", myInt1, myInt2);

Log.Info("Display as hex myInt1=%x, myInt1=%X", myInt1, myInt1);
    
Log.Info("Display as hex myInt2=%x, myInt2=%X", myInt2, myInt2);
    
Log.Info("Display as binary myInt1=%b, myInt1=%B", myInt1, myInt1);

Log.Info("Display as binary myInt2=%b, myInt2=%B", myInt2, myInt2);

Log.Info("Display my longs myLong1 %l, myLong2=%l", myLong1, myLong2);

Log.Info("Display my bool myBool1=%t, myBool2=%T", myBool1, myBool2);

Log.Info("Output: %s", myString);

Log.Error("is this an real error? %T", myBool2);

Log.Debug("%d, %d, %l, %l, %t, %T", myInt1, myInt2,
              myLong1, myLong2,
              myBool1, myBool2);

Log.Info("have fun with this Log");
```

ScoutSyslogLogger supports logging cariage return and line feed characters. By default ScoutSyslogLogger logs to Unix Syslog which does not record CR and LF characters. The format looks like this:

```
Log.Info("Have fun with this Log"CR);

Log.Info(CR"Have fun with this Log"CR);

Log.Info(CR"Have fun with this Log");

```

##LogIt Web Viewer for Syslog

ScoutSyslogLogger uses [LogIo](http://logio.org) to provide a Web view of the Syslog file:

- Shows a tail -f style view of log messages
- Filter the view to only the log messages you want to see
- Display multiple views
- Filter to different servers and log streams

LogIo is a Node.js based Web front end app. With Node.js already installed, use these LogIo installation instructions.

1. Install via npm
npm install -g log.io

2. Launch server
log.io-server

3. Configure log harvester
vi ~/.log.io/harvester.conf

4. Start log harvester
log.io-harvester

5. Browse to http://localhost:28778

An example harvester file from a Redhat (RHEL) 6 environment looks like this:

```
exports.config = {
  nodeName: "application_server",
  logStreams: {
    messages: [
      "/var/log/messages","/var/log/cron"
    ]
  },
  server: {
    host: '0.0.0.0',
    port: 28777
  }
}
```

##How It Works
ScoutSyslogLogger automatically uses the Atmel ATmega256RFR2 or similarly equipped processors with LightWeight Mesh (LWM) capability to deliver messages to the central logger.

<img style="width: 707px; height: 194px;"src="https://votsh.files.wordpress.com/2014/06/howitworks.jpg">

Messages come from an app running in the Lead Scout or Scouts. ScoutSyslogLogger delivers log messages originating in a Scout to the Lead Scout over the LWM radio network. ScoutSyslogLogger running in the Lead Scout sends the log message to the Syslog service on a server over Wifi (using UDP protocol)or Bluetooth (BLE) networking (using a BLE gateway app).

At a code leve, ScoutSyslogLogger uses the following object architecture.

<img style="width: 810px; height: 373px;"alt="Software object architecture diagram"src="https://votsh.files.wordpress.com/2014/06/scoutweblogger.jpg"><br>


##How To Contribute

ScoutSyslogLogger was originally written by Frank Cohen of Votsh. Frank's opinion of C and C++: "Come on C and C++, I remember back to the 1980's when you were written. You were an expression of teenage angst. Your times is past." The code is waiting for you to improve it past the original febile attempt.

Code Repository is at [https://github.com/Votsh/waves](https://github.com/Votsh/waves)

Make changes to the code and issue a push. Or send your changes to [fcohen@votsh.com](fcohen@votsh.com)

ScoutSyslogLogger is distributed under a GPL v3 Open Source Software license.

