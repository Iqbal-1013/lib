#ifndef Time_h
#define Time_h


#include "Arduino.h"
#include "Config.h"
#include "HTTP.h"
#include "URL.h"



unsigned long SYSTEM_TIME = 0;
unsigned long ELAPSED_TIME_OFFSET = millis();


void Time_UpdateSystemTime();




void Time_UpdateSystemTime(Config &config)
{
	SYSTEM_TIME = 0;

	SYSTEM_TIME = config.timestamp;
	ELAPSED_TIME_OFFSET = millis() / 1000;

	Log(F("\nSYSTEM_TIME : "), SYSTEM_TIME);
	Log(F("ELAPSED_TIME_OFFSET : "), ELAPSED_TIME_OFFSET);
}


unsigned long Time_Get()
{
	return SYSTEM_TIME + (unsigned long)(millis() / 1000) - ELAPSED_TIME_OFFSET;
}



#endif // !Time_h
