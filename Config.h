#ifndef Config_h
#define Config_h


#include "Arduino.h"
#include "CommercialID.h"
#include "Geofence.h"
#include "HTTP.h"
#include "URL.h"


class Config
{
public:

	// Engine
	boolean lockStatus;

	// Timestamp
	unsigned long timestamp;
	
	// Geofence
	const uint8_t geofenceLimit = 10;
	uint8_t geofenceLength = 0;
	Geofence *geofences = NULL;

	// USSD
	uint8_t indexStart = -1;
	char code[20] = { '\0' };
	char msisdn[40] = { '\0' };
	char mask[40] = { '\0' };



	Config()
	{
		geofences = new Geofence[geofenceLimit];
	}

	~Config()
	{
		delete[] geofences;
	}
	
	void Update()
	{
		delete[] geofences;
		geofences = new Geofence[geofenceLimit];

		//memset(geofences, '\0', geofenceLimit);
		//geofences = new Geofence[geofenceLimit];
		
		do
		{
			HTTP_Flush();
			sprintf(HTTP_Buffer, "%s=%s", "com_id", COMMERCIAL_ID);
		}
		while(send_HTTP(URL_SETUP_INIT) != 1);
		//HTTP_Parse();

		Parse();
	}

	void Parse(const char *str=HTTP_Buffer)
	{
		const uint16_t bufferLength = strlen(str) + 1;
		char *buffer = new char[bufferLength];
		strcpy(buffer, str);

		char *rest;
		char delim[] = "\n";

		// Parsing engine status
		char *token = strtok_r(buffer, delim, &rest);
		lockStatus = atoi(token) == 1;
		Log(F("\nLock status : "), lockStatus);

		// Parsing timestamp
		token = strtok_r(rest, delim, &rest);
		timestamp = atol(token);
		Log(F("\nTimestamp : "), timestamp);

		// Garbage
		token = strtok_r(rest, delim, &rest);

		// Parsing USSD
		token = strtok_r(rest, delim, &rest);
		Parse_USSDFormat(token);
		Log(F("Code : "), code);
		Log(F("Index start : "), indexStart);
		Log(F("MSISDN : "), msisdn);
		Log(F("Mask : "), mask);

		// Parsing Geofence
		geofenceLength = 0;

		char id[25] = { '\0' };
		float latitude = 0.0;
		float longitude = 0.0;
		float radius = 0.0;
		Log("\n[Parsing Geofence]\n");

		token = strtok_r(rest, delim, &rest);
		while (token != NULL && geofenceLength < geofenceLimit)
		{
			Log("\n\n[Counting geofence]\n\n");
			Parse_GeofenceDefinition(token, id, latitude, longitude, radius);
			geofences[geofenceLength] = Geofence(id, latitude, longitude, radius);
			

			geofences[geofenceLength].Print();

			geofenceLength++;

			token = strtok_r(rest, delim, &rest);
		}		
	}


	void Parse_USSDFormat(const char *str)
	{
		memset(code, '\0', sizeof(code));
		memset(mask, '\0', sizeof(mask));
		indexStart = -1;

		const char delim[] = ",\n";
		const uint16_t bufferLength = strlen(str) + 1;
		char *buffer = new char[bufferLength];
		strcpy(buffer, str);

		char *token = strtok(buffer, delim);
		strcpy(code, token);

		token = strtok(NULL, delim);
		indexStart = atoi(token);

		token = strtok(NULL, delim);
		strcpy(msisdn, token);

		token = strtok(NULL, delim);
		strcpy(mask, token);

		delete[] buffer;
	}


	void Parse_GeofenceDefinition(const char *geofence_str, char *id, float &latitude, float &longitude, float &radius)
	{
		const char delim[] = ",";

		const uint16_t bufferLength = strlen(geofence_str) + 1;
		char *buffer = new char[bufferLength];
		strcpy(buffer, geofence_str);

		char *token = strtok(buffer, delim);
		strcpy(id, token);

		token = strtok(NULL, delim);
		latitude = atof(token);

		token = strtok(NULL, delim);
		longitude = atof(token);

		token = strtok(NULL, delim);
		radius = atof(token);

		delete[] buffer;
	}
};




#endif // !Config_h
