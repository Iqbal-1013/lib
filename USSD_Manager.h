#ifndef USSD_Manager_h
#define USSD_Manager_h


#include "Arduino.h"
#include "CommercialID.h"
#include "Config.h"
#include "SMS.h"
#include "SIM808.h"
#include "HTTP.h"
#include "URL.h"





class USSD_Manager
{
public:

	uint8_t *indexStart;
	char *code;
	char *msisdn;
	char *mask;
	
	char balance[20] = { '\0' };


	void Init(Config &config)
	{
		code = config.code;
		msisdn = config.msisdn;
		mask = config.mask;
		indexStart = &config.indexStart;


		boolean statusMSISDN = false;
		boolean statusMask = false;

		// to be redesigned
		for(uint8_t i=0; i<3; i++)
		{
			SIM808_DialUSSD(1, code);
			
			unsigned long start = millis();
			unsigned long timeout = 15000;

			while ((!statusMSISDN && !statusMask) && (millis() - start) < timeout)
			{
				statusMSISDN = SMS_IsAvailable(msisdn, mask);
				statusMask = SMS_IsAvailable(mask, msisdn);
				
				Log(F("\n[ Status MSISDN ] : "), statusMSISDN);
				Log(F("[ Status Mask ] : "), statusMask);
			}

			if ((statusMSISDN || statusMask))
			{
				break;
			}
		}
		
		if ((statusMSISDN || statusMask))
		{
			Parse_USSD();
			Log(F("\n\n[ BALANCE ] : "), balance);
			SendBalance();
			SMS_DeleteAll();
		}
	}

	
	void SendBalance()
	{
		HTTP_Flush();
		sprintf(HTTP_Buffer, "%s=%s&%s=%s", "com_id", COMMERCIAL_ID, "balance", balance);
		for (uint8_t i = 0; i < 2 && !HTTP_Post(URL_USSD_BALANCE); i++)
		{
			HTTP_Flush();
			sprintf(HTTP_Buffer, "%s=%s&%s=%s", "com_id", COMMERCIAL_ID, "balance", balance);
		}
	}


	//	[ GP ] :	You are enjoying 1536MB Internet.
	//				Available Regular Internet 1513.25MB Valid till 09 Oct, 17; 0 SMS any number Valid till 09 Oct, 17; Download MyGP goo.
	void Parse_USSD(char *str = SMS_Data)
	{
		const uint8_t bufferLength = strlen(str) + 1;
		char *buffer = new char[bufferLength];
		strcpy(buffer, str);

		char *token = strtok(buffer + *indexStart, " ");
		strcpy(balance, token);

		delete[] buffer;
	}
};



#endif // !USSD_Manager_h
