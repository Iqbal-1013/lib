#ifndef Watchdog_h
#define Watchdog_h


#include "avr\wdt.h"

#include "Arduino.h"
#include "Diagnostics.h"
#include "HTTP.h"
#include "URL.h"


#define WATCHDOG_PIN 32



namespace Watchdog
{
	void Enable();
	void Disable();
	void Reset();


	void Enable()
	{
		//cli();
		//wdt_reset();
		///*
		//WDTCSR configuration:
		//WDIE = 1: Interrupt Enable
		//WDE = 1 :Reset Enable
		//See table for time-out variations:
		//WDP3 = 0 :For 1000ms Time-out
		//WDP2 = 1 :For 1000ms Time-out
		//WDP1 = 1 :For 1000ms Time-out
		//WDP0 = 0 :For 1000ms Time-out
		//*/
		//// Enter Watchdog Configuration mode:
		//WDTCSR |= (1 << WDCE) | (1 << WDE);
		//// Set Watchdog settings:
		//WDTCSR = (1 << WDIE) | (1 << WDE) |
		//	(1 << WDP3) | (0 << WDP2) | (0 << WDP1) |
		//	(1 << WDP0);
		//sei();

		wdt_enable(WDTO_8S);
	}


	void Reset()
	{
		wdt_reset();
	}


	void Disable()
	{
		wdt_disable();
	}
}








#endif // !Watchdog_h
