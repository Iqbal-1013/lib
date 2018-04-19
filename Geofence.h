#ifndef Geofence_h
#define Geofence_h


#include "Arduino.h"
#include "Debug.h"




class Geofence
{

private:

	char id[25] = { '\0' };
	double latitude = 0.0;
	double longitude = 0.0;
	double radius = 0.0;		// meter

	boolean state = false;		// true = inside,	false = outside


public:

	Geofence(const char *id=NULL, double latitude=0.0, double longitude=0.0, double radius=0.0) : latitude(latitude), longitude(longitude), radius(radius)
	{
		strcpy(this->id, id);
	}


	boolean IsViolated(double lat, double lon)
	{
		double distance = CalculateDistance(lat, lon, this->latitude, this->longitude);

		Log(F("\n\nID : "), id);
		Log(F("G-Latitude : "), latitude);
		Log(F("G-Longitude : "), longitude);
		Log(F("G-Radius : "), radius);
		Log(F("\nLatitude : "), lat);
		Log(F("Longitude : "), lon);
		Log(F("Distance : "), distance);

		if (distance > radius)
		{
			return true;
		}

		return false;
	}

	// returns the distance between two Geo-coordinates (in meter)
	static double CalculateDistance(double lat1, double lon1, double lat2, double lon2)
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

	const char* GetID()
	{
		return id;
	}

	boolean GetState()
	{
		return state;
	}

	void SetState(boolean state)
	{
		this->state = state;
	}

	void Set(const char *id, double latitude = 0.0, double longitude = 0.0, double radius = 0.0)
	{
		strcpy(this->id, id);
		this->latitude = latitude;
		this->longitude = longitude;
		this->radius = radius;
	}

	void Print()
	{
		Log(F("\n\n[ Geofence Definition ]\n"));
		Log(F("ID : "), id);
		Log(F("Latitude : "), latitude);
		Log(F("Longitude : "), longitude);
		Log(F("Radius : "), radius);
	}
};




#endif // !Geofence_h
