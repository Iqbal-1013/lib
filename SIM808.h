#ifndef SIM808_h
#define SIM808_h

#include "Arduino.h"
#include "AT.h"





#define AT "AT"
#define AT_MEE "AT+CMEE"				// Configure Mobile Equipment Error
#define AT_ECHO "ATE"					// Set Command Echo Mode
#define AT_LOCAL_RATE "AT+IPR"			// Set TE-TA Fixed Local Rate
#define AT_REG "AT+CREG"				// Network registration
#define AT_IMEI "AT+GSN"
#define AT_IMSI "AT+CIMI"
#define AT_ICCID "AT+CCID"
#define AT_USSD "AT+CUSD"
#define AT_FUNC "AT+CFUN"
#define AT_SLOW_CLOCK "AT+CSCLK"




boolean SIM808_SetBaudRate(const unsigned long baudrate);
boolean SIM808_IsUp();
boolean SIM808_SetEcho(const char *value);
boolean SIM808_SetMEEReporting(const char *value);
boolean SIM808_GetIMEI();
boolean SIM808_GetIMSI();
boolean SIM808_GetICCID();
boolean SIM808_SetSlowClock(uint8_t mode);
boolean SIM808_TestSlowClock();
boolean SIM808_ReadSlowClock();
void SIM808_Sleep();
void SIM808_WakeUp();
void SIM808_Parse(char *target, const char *source);
boolean SIM808_NetworkRegistration_SetURC(int8_t n);



/*
AT+IPR=<rate>

<rate>	Baud rate per second

0 (Auto-bauding)
1200
2400
4800
9600
19200
38400
57600
115200
230400
460800
*/
boolean SIM808_SetBaudRate(const unsigned long baudrate)
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s=%lu", AT_LOCAL_RATE, baudrate);

	return AT_Send(SIM808, AT_OK, 3000);
}


boolean SIM808_IsUp()
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s", AT);

	return AT_Send(SIM808, AT_OK, 1000);
}


/*
ATE<value>

<value>		0 Echo mode off
1 Echo mode on
*/
boolean SIM808_SetEcho(const char *value)
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s%s", AT_ECHO, value);

	return AT_Send(SIM808, AT_OK, 500);
}


/*
AT+CMEE=[<n>]

<n>			0 Disable +CME ERROR: <err> result code and use	ERROR instead.
1 Enable +CME ERROR: <err> result code and use numeric <err>
2 Enable +CME ERROR: <err> result code and use verbose <err> values
*/
boolean SIM808_SetMEEReporting(const char *value)
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s=%s", AT_MEE, value);

	return AT_Send(SIM808, AT_OK, 3000);
}



boolean SIM808_NetworkRegistration_SetURC(int8_t n)
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s=%d", AT_REG, n);

	return AT_Send(SIM808, AT_OK, 3000);
}


boolean SIM808_NetworkRegistration_GetStatus()
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s?", AT_REG);

	return AT_Send(SIM808, AT_OK, 3000);
}


boolean SIM808_NetworkRegistration_IsRegistered()
{
	SIM808_NetworkRegistration_SetURC(0);

	if (SIM808_NetworkRegistration_GetStatus())
	{
		return strstr(AT_Buffer, "+CREG: 0,1") != NULL;
	}

	return false;
}


boolean SIM808_GetIMEI()
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s", AT_IMEI);

	return AT_Send(SIM808, AT_OK, 3000);
}


boolean SIM808_GetIMSI()
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s", AT_IMSI);

	return AT_Send(SIM808, AT_OK, 3000);
}


boolean SIM808_GetICCID()
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s", AT_ICCID);

	return AT_Send(SIM808, AT_OK, 3000);
}


void SIM808_Parse(char *target, const char *source=AT_Buffer)
{
	const uint8_t bufferLength = strlen(source) + 1;
	char *buffer = new char[bufferLength];
	strcpy(buffer, source);

	char *token = strtok(buffer, "\n");
	token = strtok(NULL, "\r");
	strcpy(target, token);
}


// <numeric parameter>
// 0 disable the result code presentation in the TE
// 1 enable the result code presentation in the TE
// 2 cancel session (not applicable to read Command response)
boolean SIM808_DialUSSD(const uint8_t controlParam, const char* code)
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s=%d,\"%s\"", AT_USSD, controlParam, code);

	if (AT_Send(SIM808, AT_OK, 5000))
	{
		memset(AT_Buffer, '\0', AT_BufferLength);
		if (waitFor(SIM808, 0, "+CUSD: ", 10000))
		{
			memset(AT_Buffer, '\0', AT_BufferLength);
			return waitFor(SIM808, 0, "\r\n", 3000);
		}
	}

	return false;
}



//	Parameters
//	<fun>	0 Minimum functionality
//			1 Full functionality(Default)
//			4 Disable phone both transmit and receive RF circuits.
//	
//	<rst>	1 Reset the MT before setting it to <fun> power level
//
// Max Response Time	10s
boolean SIM808_SetFunctionality(uint8_t func, uint8_t reset)
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s=%d,%d", AT_FUNC, func, reset);

	return AT_Send(SIM808, AT_OK, 1000);
}


boolean SIM808_TestSlowClock()
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s=?", AT_SLOW_CLOCK);

	return AT_Send(SIM808, AT_OK, 1000);
}


boolean SIM808_SetSlowClock(uint8_t mode)
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s=%d", AT_SLOW_CLOCK, mode);
	
	return AT_Send(SIM808, AT_OK, 1000);
}


boolean SIM808_ReadSlowClock()
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s?", AT_SLOW_CLOCK);

	return AT_Send(SIM808, AT_OK, 1000);
}


void SIM808_Sleep()
{
	while (!SIM808_SetSlowClock(2));
	delay(5000);
}

void SIM808_WakeUp()
{
	while(!SIM808_IsUp());
	while (!SIM808_SetSlowClock(0));
}


void SIM808_Init()
{
	while (!SIM808_IsUp());
	while (!SIM808_SetBaudRate(9600));
	while (!SIM808_SetEcho("0"));
	while (!SIM808_NetworkRegistration_IsRegistered());
}


void SIM808_Reset()
{
	while (!SIM808_SetFunctionality(1, 1));
}


#endif //ifndef SIM808_h
