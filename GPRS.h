#ifndef GPRS_h
#define GPRS_h


#include "Arduino.h"
#include "BearerProfile.h"
#include "AT.h"
#include "Debug.h"



#define GPRS_BEARER_SETTINGS "AT+SAPBR"

#define AT_CHECK_GPRS "AT+CGATT?"
#define RESPONSE_GPRS_CHECK "+CGATT: 1"





boolean GPRS_Init(const uint8_t cid);
boolean GPRS_SetParameter(const char *commandType, const uint8_t cid, const char *tag, const char *value);
boolean GPRS_Query(const uint8_t cid);
boolean GPRS_IsBearerConnected(const uint8_t cid);
boolean GPRS_Start(const uint8_t cid);
boolean GPRS_Stop(const uint8_t cid);





/*
[ COMMAND ]

AT+SAPBR=<cmd_type>,<cid>[,<ConParamTag>,<ConParamValue>]

<cmd_type>	0 Close bearer
1 Open bearer
2 Query bearer
3 Set bearer parameters
4 Get bearer parameters

<cid>		Bearer profile identifier

<Status>	0 Bearer is connecting
1 Bearer is connected
2 Bearer is closing
3 Bearer is closed

<ConParamTag> Bearer parameter
"CONTYPE" Type of Internet connection. Value refer to

<ConParamValue_ConType>
"APN" Access point name string: maximum 64 characters
"USER" User name string: maximum 32 characters
"PWD" Password string: maximum 32 characters
"PHONENUM" Phone number for CSD call
"RATE" CSD connection rate. For value refer to

<ConParamValue_Rate>

<ConParamValue> Bearer paramer value

<ConParamValue_ConType>
"CSD" Circuit-switched data call.
"GPRS" GPRS connection.

<ConParamValue_Rate>
0 2400
1 4800
2 9600
3 14400

<IP_Addr> The IP address of bearer

Max Response Time
When <cmd_type> is 1, 85 seconds
When <cmd_type> is 0, 65 seconds


[ RESPONSE ]

OK

If <cmd_type> = 2
+SAPBR: <cid>,<Status>,<IP_Addr>
OK

If <cmd_type>=4
+SAPBR:
<ConParamTag>,<ConParamValue>
OK

Unsolicited Result Code
+SAPBR <cid>: DEACT
*/



boolean GPRS_Init(const uint8_t cid = bearerProfile)
{
	/*GPRS_SetParameter("3", cid, "CONTYPE", "GPRS");
	GPRS_SetParameter("3", cid, "APN", "gpinternet");*/

	//while (!GPRS_Start(cid))
	//{
	//	//GPRS_Stop(cid);
	//	delay(1000);
	//}


	/*Time();
	Log("Network Registration Check");
	while ((AT_SendOnlyOK(SIM808, "AT+CREG?", "+CREG: 0,1", 1000) || AT_SendOnlyOK(SIM808, "AT+CREG?", "+CREG: 0,5", 1000)) == 0);
	while (AT_SendOnlyOK(SIM808, "AT+CREG?", "+CREG: 0,1", 2000) == 0);
	Time();
	Log("Network Registration Check  Completaed");*/


	//Time();
	//AT_SendOnlyOK(SIM808, "AT+SAPBR=3,1,\"Contype\",\"GPRS\"", "OK", 4000);
	//AT_SendOnlyOK(SIM808, "AT+SAPBR=3,1,\"APN\",\"gpinternet\"", "OK", 4000);

	//AT_SendOnlyOK(SIM808, "AT+SAPBR=0,1", "OK", 5000);

	//// gets the GPRS bearer
	//uint8_t i = 0;
	//while (!GPRS_IsBearerConnected(cid))
	//{
	//	AT_SendOnlyOK(SIM808, "AT+SAPBR=1,1", "OK", 4000);
	//	i++;

	//	if (i == 4)
	//	{
	//		Log("\n[ Stoping GPRS ]\n");
	//		i = 0;
	//		AT_SendOnlyOK(SIM808, "AT+SAPBR=0,1", "OK", 5000);
	//	}
	//}
	//Log("AT+SAPBR Completed");


	//while(!GPRS_SetParameter("3", cid, "CONTYPE", "GPRS"));
	//while(!GPRS_SetParameter("3", cid, "APN", "gpinternet"));
	//GPRS_Stop(cid);

	//while (!GPRS_IsBearerConnected(cid))
	//{
	//	GPRS_Stop(cid);
	//	delay(1000);
	//	GPRS_Start(cid);
	//	
	//}

	delay(500);

	while (sendATcommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"", "OK", 5000) == 0);
	Log(F("\n\n[  ConType is complete ]"));
	delay(500);
	while (sendATcommand("AT+SAPBR=3,1,\"APN\",\"blweb\"", "OK", 5000) == 0);
	Log(F("\n\n[  APN is complete ]"));
	delay(500);

	Log(F("\n\n[  Beginning GPRS Start ]"));


	if (sendATcommand(AT_CHECK_GPRS, RESPONSE_GPRS_CHECK, 5000) == 0)
	{
		//if(sendATcommand(AT_CHECK_GPRS, "OK", 5000) == 0)
		while (sendATcommand("AT+CGATT=1", "OK", 10000) == 0) {
			delay(500);
			Log("\n\n[ Attatch Failed ]\n\n");
		}
			
	}
	Log("\n\n[ Attatched successfully]\n\n ");

	// gets the GPRS bearer
	int counter = -1;

	while (sendATcommand("AT+SAPBR=1,1", "OK", 20000) == 0)
	{

		Watchdog::Reset();
		Log(F("\n\n[  Failed : Stoping GPRS ]"));
		delay(2500);
		sendATcommand("AT+SAPBR=0,1", "OK", 20000);
		Log(F("\n\n[  Retrying GPRS Start ]"));
		delay(2500);

		if (++counter == 3)
		{
			break;
		}
	}

	if (counter >= 3 ) {
		Log("\n\n[Stopping shield]\n\n");
		digitalWrite(21,HIGH);
		delay(2000);
		digitalWrite(21,LOW);	
		delay(2000);
		return false;
	}

	return true;
}


boolean GPRS_SetParameter(const char *commandType, const uint8_t cid = bearerProfile, const char *tag = NULL, const char *value = NULL)
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s=%s,%d", GPRS_BEARER_SETTINGS, commandType, cid);

	if (tag != NULL && value != NULL)
	{
		sprintf(AT_Buffer, "%s,\"%s\",\"%s\"", AT_Buffer, tag, value);
	}

	return AT_Send(SIM808, AT_OK, 5000);
}


boolean GPRS_Query(const uint8_t cid = bearerProfile)
{
	return GPRS_SetParameter("2", cid);
}


boolean GPRS_IsBearerConnected(const uint8_t cid = bearerProfile)
{
	if (GPRS_Query())
	{
		return strstr(AT_Buffer, "+SAPBR: 1,1") != NULL;
	}

	return false;
}


boolean GPRS_Start(const uint8_t cid = bearerProfile)
{
	return GPRS_SetParameter("1", cid); //enable the GPRS
}

boolean GPRS_Stop(const uint8_t cid = bearerProfile)
{
	return GPRS_SetParameter("0", cid); //disable the GPRS
}



#endif // !GPRS_h


