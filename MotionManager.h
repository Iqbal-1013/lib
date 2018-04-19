#ifndef Motion_h
#define Motion_h



#include "Arduino.h"
#include "CommercialID.h"
#include "GPS.h"
#include "Timer.h"
#include "URL.h"
#include "Math.h"


class MotionManager
{
public:

	boolean flag_PrevEmpty = true;
	boolean flag_SpeedSmoothingEnabled = false;
	boolean flag_SpeedEngineOff = false;

	boolean flag_SpeedLimitExceededSoft = false;
	boolean flag_SpeedLimitExceededHard = false;

	boolean flag_EngineOffSafe = false;
	boolean flag_ServiceStringSendable = true;

	float latitudePrev;
	float longitudePrev;
	float latitude;
	float longitude;
 


	float distance = 0;	// m
	unsigned long  time = 0.0;	// millis
	float speed = 0.0;			// km / h
	float speedEngineOff = 0.0;	// km / h

	const uint8_t speedBufferLength = 6;
	const uint8_t speedBufferSoftLength = 6;
	float speedBuffer[6] = { 0 };
	uint8_t index_SpeedBuffer = 0;

	//unsigned long timerStart = 0;

	uint8_t distanceLimit = 10;		// m
	uint8_t speedLimitSoft = 60;	// km/h
	uint8_t speedLimitHard = 80;	// km/h
	uint8_t speedLimitEngineOff = 20; // km / h



	~MotionManager()
	{
	}

	void Init()
	{
	}

	void Update(char *str)
	{
		flag_ServiceStringSendable = true;

		this->latitude = GPS_Latitude;
		this->longitude = GPS_Longitude;

		for (int i = 0; i < 3; i++)
		{
			if (GPS_Velocity_buffer[i] > 110)
			{
				speedBuffer[index_SpeedBuffer] = 40;
				index_SpeedBuffer = ++index_SpeedBuffer % speedBufferLength;
			}
			else
			{
				speedBuffer[index_SpeedBuffer] = GPS_Velocity_buffer[i];
				index_SpeedBuffer = ++index_SpeedBuffer % speedBufferLength;
			}
			

		}



		if (flag_PrevEmpty)
		{
			latitudePrev = latitude;
			longitudePrev = longitude;
			flag_PrevEmpty = false;
			return;
		}

		speed = GPS_Velocity;	// km / h
								//distance = speed * time / 3600.0;	// m
		distance = GPS_Distance(latitude, longitude, latitudePrev, longitudePrev);

		Log(F("\n\n[ SPEED ] : "), speed);
		Log(F("[ DISTANCE ] : "), distance);

		//speedBuffer[index_SpeedBuffer] = speed;
		//index_SpeedBuffer = ++index_SpeedBuffer % speedBufferLength;


		Log(F("\n[ FLAG HARD SPEED LIMIT ] : "), flag_SpeedLimitExceededHard);
		Log(F("[ FLAG SOFT SPEED LIMIT ] : "), flag_SpeedLimitExceededSoft);

		// Engine off safety check 
		flag_EngineOffSafe = IsEngineOffSafe();
		Log(F("[ FLAG ENGINE OFF SAFE ] : "), flag_EngineOffSafe);


		CheckAndHandleEvent(str);


		// Updating lat lon
		latitudePrev = latitude;
		longitudePrev = longitude;
	}

	void EventHandler(char *str,const uint8_t &speedLimit, const boolean &flag)
	{
		//HTTP_Flush();
		//sprintf(HTTP_Buffer, "%s=%s", "com_id", COMMERCIAL_ID);
		//sprintf(HTTP_Buffer, "%s&%s=%s&%s=%d&%s=%d", HTTP_Buffer, "sp", Math::FloatToString(speed), "lm", speedLimit, "fl", flag);


		//for (uint8_t i = 0; i < 2 && !HTTP_SafePost(URL_SPEED_LIMIT); i++)
		/*{
		HTTP_Flush();
		sprintf(HTTP_Buffer, "%s=%s", "com_id", COMMERCIAL_ID);
		sprintf(HTTP_Buffer, "%s&%s=%s&%s=%d&%s=%d", HTTP_Buffer, "sp", Math::FloatToString(speed), "lm", speedLimit, "fl", flag);
		}*/
		//HTTP_SafePost(URL_SPEED_LIMIT);
		sprintf(str, "%s%d,%d", str, speedLimit, flag);
	}

	boolean IsSpeedLimitExceededSoft()
	{
		//return speed >= speedLimitSoft;

		for (uint8_t i = 0; i < speedBufferSoftLength; i++)
		{
			if (speedBuffer[i] < speedLimitSoft)
			{
				return false;
			}
		}

		return true;
	}

	boolean IsSpeedLimitExceededHard()
	{
		//return speed >= speedLimitHard;

		for (uint8_t i = 0; i < speedBufferLength; i++)
		{
			if (speedBuffer[i] < speedLimitHard)
			{
				return false;
			} 
		}

		return true;
	}

	void CheckAndHandleEvent(char *str)
	{
		sprintf(str, "%s%c%s=", str, '&', "sp");

		if (IsSpeedLimitExceededHard())
		{
			if (!flag_SpeedLimitExceededHard)
			{
				EventHandler(str,speedLimitHard, !flag_SpeedLimitExceededHard);
			}
		}
		else if (IsSpeedLimitExceededSoft())
		{
			if (flag_SpeedLimitExceededHard)
			{
				EventHandler(str,speedLimitHard, !flag_SpeedLimitExceededHard);
			}
			else if (!flag_SpeedLimitExceededSoft)
			{
				EventHandler(str,speedLimitSoft, !flag_SpeedLimitExceededSoft);
			}
		}
		else
		{
			if (flag_SpeedLimitExceededSoft)
			{
				EventHandler(str,speedLimitSoft, !flag_SpeedLimitExceededSoft);
			}
		}

		flag_SpeedLimitExceededHard = IsSpeedLimitExceededHard();
		flag_SpeedLimitExceededSoft = IsSpeedLimitExceededSoft();
	}

	boolean IsEngineOffSafe()
	{
		for (uint8_t i = 0; i < speedBufferLength; i++)
		{
			if (speedBuffer[i] > speedLimitEngineOff)
			{
				return false;
			}
		}
		return true;
	}

	void AppendToServiceString(char *str, const char delim)
	{
		sprintf(str, "%s%c%s=%s", str, delim, "d", Math::FloatToString(distance));
		//sprintf(str, "%s%c%s=%s", str, delim, "sp", Math::FloatToString(speed));
	}

	boolean IsServiceStringSendable()
	{
		return flag_ServiceStringSendable;
	}
};

#endif // !Motion_h
