#ifndef Service_h
#define Service_h


#include "Arduino.h"
#include "Debug.h"


class Service
{

public:

	boolean enabled = true;
	uint8_t serviceID;
	uint8_t pin; //pin number
	uint8_t type = 1; //0=analog 1=digital, 2=digital event triggered
	uint16_t interval = 5000; //interval interval in seconds to read
	unsigned long lastAccessTime = 0; //Last data read time, last state for event based


	Service(uint8_t serviceID, uint8_t pin, uint8_t type=1, uint16_t interval=5000) : serviceID(serviceID), pin(pin), type(type), interval(interval)
	{}


	void Enable()
	{
		enabled = true;
	}

	void Disable()
	{
		enabled = false;
	}
};





#endif // !Service_h
