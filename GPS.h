#ifndef GPS_h
#define GPS_h




#include "Arduino.h"
#include "AT.h"



#define GPS_POWER "AT+CGNSPWR"			// GNSS power control
#define GPS_INFO "AT+CGNSINF"			// GNSS navigation information parsed from NMEA sentences





float GPS_Latitude = 0.0;
float GPS_Longitude = 0.0;
float GPS_Velocity = 0.0;

float GPS_Latitude_buffer[3] = { 0.0 };
float GPS_Longitude_buffer[3] = { 0.0 };
float GPS_Velocity_buffer[3] = { 0.0 };

char GPS_Timestamp[20] = { '\0' };

int gps_counter = 0;




void GPS_Flush();


void GPS_Start();
boolean GPS_Read();
void GPS_Parse(char* gps, float &latitude, float &longitude, char* datetime, float &velocity);

double GPS_Distance(double lat1, double lon1, double lat2, double lon2);







/*
AT+CGNSPWR=<mode>
<mode>: 0 Turn off GNSS power supply
1 Turn on GNSS power supply
*/
void GPS_Start()
{
	do
	{
		memset(AT_Buffer, '\0', AT_BufferLength);
		sprintf(AT_Buffer, "%s=%s", GPS_POWER, "1");
	}
	while (!AT_Send(SIM808, AT_OK, 3000));
}

void GPS_Stop()
{
	do
	{
		memset(AT_Buffer, '\0', AT_BufferLength);
		sprintf(AT_Buffer, "%s=%s", GPS_POWER, "0");
	} while (!AT_Send(SIM808, AT_OK, 3000));
}

/*
[ Command ]

AT+CGNSINF

[ Response ]

+CGNSINF: <GNSS run status>,<Fix status>,<UTC date & Time>,<Latitude>,<Longitude>,
<MSL Altitude>,<Speed Over Ground>,<Course Over Ground>,<Fix Mode>,<Reserved1>,
<HDOP>,<PDOP>,<VDOP>,<Reserved2>,<GNSS Satellites in View>,<GNSS Satellites Used>,
<GLONASS SatellitesUsed>,<Reserved3>,<C/N0 max>,<HPA>,<VPA>

OK
*/
boolean GPS_Read()
{
	boolean check = false;
	AT_Send(SIM808, GPS_INFO, AT_OK, 1000);

	// condition for valid response
	while (strstr(AT_Buffer, "+CGNSINF: 1,1") != NULL)
	{
		check = true;
		GPS_Parse(AT_Buffer, GPS_Latitude, GPS_Longitude, GPS_Timestamp, GPS_Velocity);

		if (++gps_counter == 3) {
			break;
		}

		AT_Send(SIM808, GPS_INFO, AT_OK, 1000);
	}

	GPS_Latitude = 0;
	GPS_Longitude = 0;
	for (int i = 0; i < gps_counter; i++)
	{
		Log("\nlat ", GPS_Latitude_buffer[i]);
		Log("\nlng ",GPS_Longitude_buffer[i]);
		GPS_Latitude += GPS_Latitude_buffer[i];
		GPS_Longitude += GPS_Longitude_buffer[i];
	}

	GPS_Longitude /= gps_counter;
	GPS_Latitude /= gps_counter;
	gps_counter = 0;

	return check;
}


// AT+CGNSINF response : +CGNSINF: 1,1,20170709102949.000,23.794847,90.403707,74.200,00
void GPS_Parse(char* gps=AT_Buffer, float &latitude = GPS_Latitude, float &longitude = GPS_Longitude, char* datetime = GPS_Timestamp, float &velocity = GPS_Velocity)
{
	GPS_Flush();

	const uint8_t bufferLength = strlen(gps) + 1;
	char *buffer = new char[bufferLength];
	strcpy(buffer, gps);

	char *token = strtok(buffer, ",");//skip GNSS mode
	//DEBUG_ADV("Skipping GNSS mode : ", token)

	token = strtok(NULL, ","); // skip fix status
	//DEBUG_ADV("Skipping fix status : ", token)

	strcpy(datetime, strtok(NULL, ".")); // Get UTC date & time
	//DEBUG_ADV("datetime : ", datetime)

	token = strtok(NULL, ",");    //skip ms times
	//DEBUG_ADV("Skipping ms times : ", token)

	token = strtok(NULL, ",");
	latitude = atof(token); // Get latitude
	GPS_Latitude_buffer[gps_counter] = latitude;
	Log(F("Latitude : "), token);

	token = strtok(NULL, ",");
	longitude = atof(token); // Get longitude
	GPS_Longitude_buffer[gps_counter] = longitude;
	Log(F("Longitude : "), token);


	token = strtok(NULL, ",");	// Altitude


	token = strtok(NULL, ",");  // velocity
	velocity = atof(token);
	GPS_Velocity_buffer[gps_counter] = velocity;
	Log(F("Velocity : "), velocity);


	delete[] buffer;

}


// returns the distance between two Geo-coordinates (in meter)
double GPS_Distance(double lat1, double lon1, double lat2, double lon2)
{
	const double R = 6371.0e3; // radius of Earth in meter

	lat1 = lat1 * PI / 180.0;
	lon1 = lon1 * PI / 180.0;

	lat2 = lat2 * PI / 180.0;
	lon2 = lon2 * PI / 180.0;


	double dlat = lat2 - lat1;
	double dlon = lon2 - lon1;

	double a = sin(dlat / 2.0) * sin(dlat / 2.0) + (cos(lat1) * cos(lat2) * sin(dlon / 2.0) * sin(dlon / 2.0));
	double c = 2 * atan2(sqrt(a), sqrt(1 - a));

	return R * c;
}



void GPS_Flush()
{
	GPS_Latitude = 0.0;
	GPS_Longitude = 0.0;
	GPS_Velocity = 0.0;
	memset(GPS_Timestamp, '\0', sizeof(GPS_Timestamp));
}








#endif // GPS_h



