#ifndef Diagnostics_h
#define Diagnostics_h



#include "Arduino.h"
#include "CommercialID.h"






int freeRAM()
{
	extern int __heap_start, *__brkval;
	int v;
	return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}


namespace Diagnostics
{
	const uint8_t sendLoopCount = 200;
	int Diagnostics_FreeRAM();
	void Update(unsigned long time_loop);
	void ToString(char *buffer, const uint16_t bufferLength);
	void Print();



	// Count
	unsigned long count_loop = 0;
	unsigned long count_http_error = 0;
	//unsigned long count_gps = 0;
	//unsigned long count_geofence_violation = 0;


	// Time
	unsigned long time_setup = 0;
	unsigned long time_loop_total = 0;
	unsigned long time_loop_max = 0;
	unsigned long time_loop_min = 9999999;


	// RAM
	int ram_free = 0;
	int ram_free_max = 0;
	int ram_free_min = 32000;



	void ToString(char *buffer, const uint16_t bufferLength)
	{
		memset(buffer, '\0', bufferLength);
		sprintf(buffer, "%s=%s&%s=%lu&%s=%lu&%s=%lu&%s=%lu&%s=%lu&%s=%d&%s=%d",
			"com_id", COMMERCIAL_ID,
			"count_loop", count_loop,
			"time_setup", time_setup,
			"time_avg_loop", count_loop == 1 ? time_loop_total : time_loop_total / sendLoopCount,
			"time_min_loop", time_loop_min,
			"time_max_loop", time_loop_max,
			"free_ram_min", ram_free_min,
			"free_ram_max", ram_free_max
		);
	}


	int Diagnostics_FreeRAM()
	{
		return freeRAM();
	}

	void Update(unsigned long time_loop)
	{
		count_loop++;
		time_loop_total += time_loop;

		if (time_loop < time_loop_min)
		{
			time_loop_min = time_loop;
		}
		if (time_loop > time_loop_max)
		{
			time_loop_max = time_loop;
		}


		ram_free = freeRAM();
		if (ram_free < ram_free_min)
		{
			ram_free_min = ram_free;
		}
		if (ram_free > ram_free_max)
		{
			ram_free_max = ram_free;
		}
	}

	void Print()
	{
		Log(F("\n\n[ DIAGNOSTICS ]\n"));
		Log(F("\tSetup Time : "), time_setup / 1000.0);
		Log(F("\tLoop Count : "), count_loop);
		Log(F("\tAvg Loop Time : "), time_loop_total / (count_loop * 1000.0));
		Log(F("\tMin Loop Time : "), time_loop_min / 1000.0);
		Log(F("\tMax Loop Time : "), time_loop_max / 1000.0);
		Log(F("\tFree RAM : "), ram_free);
		Log(F("\tMin Free RAM : "), ram_free_min);
		Log(F("\tMax Free RAM : "), ram_free_max);
	}

	void Reset()
	{
		// Time
		time_loop_total = 0;
		time_loop_max = 0;
		time_loop_min = 9999999;


		// RAM
		ram_free = 0;
		ram_free_max = 0;
		ram_free_min = 32000;
	}
}





#endif // !Diagnostics_h
