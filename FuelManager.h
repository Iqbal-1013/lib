#ifndef FuelManager_h
#define FuelManager_h


#include "Arduino.h"
#include "Service.h"
#include "Debug.h"



class FuelManager
{
public:

	Service * serviceOil = NULL;
	Service *serviceGas = NULL;

	int dataOil = 0;
	int dataGas = 0;


	FuelManager(Service *serviceOil, Service *serviceGas) : serviceOil(serviceOil), serviceGas(serviceGas)
	{}


	void Update()
	{
		dataOil = analogRead(serviceOil->pin);
		delay(100);
		dataGas = analogRead(serviceGas->pin);
		delay(100);

		Log(F("\n\n[ OIL ] : "), dataOil);
		Log(F("[ GAS ] : "), dataGas);
	}

	void onlyFuel(char *str, const char delim)
	{
		dataOil = analogRead(serviceOil->pin);
		delay(100);
		sprintf(str, "%s%c%s=%d", str, delim, "fl", dataOil);
	}


	void AppendToString(char *str, const char delim)
	{
		sprintf(str, "%s%c%s=%d", str, delim, "fl" , dataOil);
		sprintf(str, "%s%c%s=%d", str, delim, "gs", dataGas);
		//sprintf(str, "%s%c%d,%d", str, delim, serviceOil->serviceID, dataOil);
		//sprintf(str, "%s%c%d,%d", str, delim, serviceGas->serviceID, dataGas);

	}
};




#endif // !FuelManager_h
