#ifndef GeofenceManager_h
#define GeofenceManager_h


#include "Arduino.h"
#include "CommercialID.h"
#include "Config.h"
#include "SMS.h"
#include "HTTP.h"
#include "GPS.h"
#include "Geofence.h"
#include "URL.h"
#include "List.h"
#include "Debug.h"




class GeofenceManager
{

public:

	Geofence *geofences = NULL;

	uint8_t *length;


	GeofenceManager()
	{
	}

	~GeofenceManager()
	{
	}


	void Init(Config &config)
	{
		geofences = NULL;
		geofences = config.geofences;
		length = &config.geofenceLength;
	}


	void Update(char *str)
	{
		CONSOLE.println("\n\n[Checking Geofence]\n\n");
		for (uint8_t i = 0; i < *length; i++)
		{
			CONSOLE.println("\n\n[Checking Geofence]\n\n");

			Log(F("\nGeofence : "), i);

			boolean violated = geofences[i].IsViolated(GPS_Latitude, GPS_Longitude);
			boolean state = geofences[i].GetState();

			// Enter/Exit logic

			// Violated		State		Result
			// -------------------------------
			//	False		False		Enter ( 1 )
			//	False		True		Inside
			//	True		False		Outside		
			//	True		True		Exit ( 0 )

			if (!(violated ^ state))
			{
				// Update status
				geofences[i].SetState(!state);

				uint8_t result = (violated && state) ? 0 : 1;


				if (!strcmp(str, ""))
				{
					sprintf(str, "%s,%d", geofences[i].GetID(), result);
				}
				else
				{
					sprintf(str, "%s,%s,%d", str, geofences[i].GetID(), result);
				}
			}
		}
	}
};




#endif // !GeofenManager_h
