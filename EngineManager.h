#ifndef EngineManager_h
#define EngineManager_h


#include "Arduino.h"
#include "CommercialID.h"
#include "Config.h"
#include "EEPROM.h"
#include "Service.h"
#include "SMS.h"
#include "HTTP.h"
#include "URL.h"
#include "Debug.h"
#include "Time.h"
#include "MotionManager.h"



#define ENGINE_OFF "ENGINE OFF"
#define ENGINE_LOCK "ENGINE LOCK"
#define ENGINE_UNLOCK "ENGINE UNLOCK"



class EngineManager
{

public:

	MotionManager *motionManager = NULL;

	Service *serviceEngineOn = NULL;
	Service *serviceEngineOff = NULL;
	boolean *locked;

	boolean flag_engineTurnOff = false;
	boolean flag_TurnOffPending = false;
	boolean engine_turned_off = false;
	uint16_t indexEEPROM_EngineOff = 0;


	unsigned long timestamp = 0;
	unsigned long timeout = 0;



	EngineManager(Service *serviceEngineOn, Service *serviceEngineOff, MotionManager *motionManager) : serviceEngineOn(serviceEngineOn), serviceEngineOff(serviceEngineOff), motionManager(motionManager)
	{
		pinMode(this->serviceEngineOn->pin, INPUT);
		//pinMode(this->serviceEngineOn->pin, OUTPUT);
		pinMode(this->serviceEngineOff->pin, OUTPUT);
		digitalWrite(this->serviceEngineOff, LOW);
	}


	~EngineManager()
	{}


	void Init(Config &config)
	{
		flag_engineTurnOff = false;
		flag_TurnOffPending = false;

		locked = &config.lockStatus;

		if (*locked)
		{
			Log(F("\n\n[ LOCKING ENGINE ]"));
			Lock();

			if (!IsLocked_EEPROM())
			{
				Log(F("\n\n[ WRITING ENGINE STATUS TO EEPROM ]"));
				EEPROM.update(indexEEPROM_EngineOff, '1');
			}
		}
		else
		{
			Log(F("\n\n[ UNLOCKING ENGINE ]"));
			if (IsLocked_EEPROM())
			{
				Log(F("\n\n[ ERASING ENGINE STATUS FROM EEPROM ]"));
				EEPROM.update(indexEEPROM_EngineOff, '0');
			}

			TurnOn();
		}
	}


	boolean Update()
	{
		if (!flag_engineTurnOff && flag_TurnOffPending && motionManager->IsEngineOffSafe())
		{
			Log(F("\n\n[ TURNING OFF ENGINE ]"));
			EEPROM.update(indexEEPROM_EngineOff, '1');
			TurnOff();
			Log(F("[ DONE ]"));
			engine_turned_off =  true;
		}
		return engine_turned_off;
	}

	// Handles Engine off SMS
	void EventHandler()
	{
		// No need to check MSISDN. It is calling function's job to ensure sms validity
		if (strstr(SMS_Data, ENGINE_OFF) != NULL)
		{
			Parse();


			// timeout logic
			unsigned long diff = Time_Get() - timestamp;

			Log(F("Now : "), Time_Get());
			Log(F("Timestamp : "), timestamp);
			Log(F("Timeout : "), timeout);
			Log(F("Diff : "), diff);


			if (diff > timeout)
			{
				return;
			}

			flag_TurnOffPending = true;
		}
		else if (strstr(SMS_Data, ENGINE_LOCK) != NULL)
		{
			Lock();
		}
		else if (strstr(SMS_Data, ENGINE_UNLOCK) != NULL)
		{
			Unlock();
		}
	}

	boolean HttpEvent() 
	{
		if (strstr(HTTP_Buffer, "0"))
		{
			flag_TurnOffPending = false;
			flag_engineTurnOff = false;
			Unlock();
			engine_turned_off = false;
			return false;
		}
		else if (strstr(HTTP_Buffer, "1"))
		{
			flag_TurnOffPending = true;
			return true;
			
			//Lock();
		}


	}


	void Lock()
	{
		//digitalWrite(serviceEngineOn->pin, LOW);
		digitalWrite(serviceEngineOff->pin, HIGH);
	}


	void Unlock()
	{
		engine_turned_off = false;
		//digitalWrite(serviceEngineOn->pin, HIGH);
		digitalWrite(serviceEngineOff->pin, LOW);
		//sendEngineOn();
	}


	// Unlocks and performs HTTP
	void TurnOn()
	{
		Unlock();
		flag_engineTurnOff = false;

	}

	void sendEngineOn() {
		HTTP_Flush();
		sprintf(HTTP_Buffer, "%s=%s&%s=%s", "com_id", COMMERCIAL_ID, "status", IsEngineOn());

		while(!send_HTTP(URL_ENGINE_UPDATE_STATUS));
		/*for (uint8_t i = 0; i < 5 && !HTTP_SafePost(URL_ENGINE_UPDATE_STATUS); i++);*/
	}


	// Locks and performs HTTP
	void TurnOff()
	{
		Lock();
		flag_TurnOffPending = false;
		flag_engineTurnOff = true;

		HTTP_Flush();
		sprintf(HTTP_Buffer, "%s=%s&%s=%s", "com_id", COMMERCIAL_ID, "status", "0");
		while (!HTTP_SafePost(URL_ENGINE_UPDATE_STATUS));
	}

	boolean IsLocked_EEPROM()
	{
		return (char)EEPROM.read(indexEEPROM_EngineOff) == '1';
	}


	boolean IsEngineOn()
	{
		boolean engine_status =  digitalRead(serviceEngineOn->pin) == HIGH;
		delay(100);
		return engine_status;
	}


	// Format : ENGINE OFF,1234567890,120
	void Parse(const char *str = SMS_Data)
	{
		const uint8_t bufferLength = strlen(str) + 1;
		char *buffer = new char[bufferLength];
		strcpy(buffer, str);

		char *token = strtok(buffer, ",");

		token = strtok(NULL, ",");
		timestamp = atol(token);

		token = strtok(NULL, "\0");
		timeout = atol(token);

		delete[] buffer;
	}

	void AppendToString(char *str, const char delim)
	{
		sprintf(str, "%s%c%s=%d", str, delim, "es", IsEngineOn());
	}
};



#endif // !EngineManager_h