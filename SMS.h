#ifndef SMS_h
#define SMS_h


#include "Arduino.h"
#include "AT.h"
#include "SerialManager.h"



//AT commands
#define SMS_READ "AT+CMGR" // format: AT+C_MG_*
#define SMS_DELETE "AT+CMGD"
#define SMS_DELETE_ALL "AT+CMGDA"
#define SMS_SEND "AT+CMGS"
#define SMS_LIST "AT+CMGL"
#define SMS_SELECT_FORMAT "AT+CMGF"
#define SMS_RESTORE_SETTINGS "AT+CRES"
#define SMS_SAVE_SETTINGS "AT+CSAS"
#define SMS_SHOW_TEXT_MODE_PARAMS "AT+CSDH"
#define SMS_SET_TEXT_MODE_PARAMS "AT+CSMP"




const uint8_t SMS_DataLength = 255;
const uint8_t SMS_BufferLength = 40;

char SMS_Data[SMS_DataLength] = { '\0' };
char SMS_MSISDN[SMS_BufferLength] = { '\0' };
char SMS_Mask[SMS_BufferLength] = { '\0' };
char SMS_Timestamp[SMS_BufferLength] = { '\0' };



boolean SMS_List(const char* state, const uint8_t mode);
boolean SMS_Read(const uint8_t index, const uint8_t mode, char *buffer, uint8_t bufferLength);
boolean SMS_Delete(const uint8_t index, const uint8_t flag);
boolean SMS_DeleteAll();
void SMS_Parse(const char *source, char *MSISDN, char *mask, char *timestamp, char *data);
boolean SMS_Send(const char* MSISDN, const char* text);
boolean SMS_SetFormat(const char *format);
boolean SMS_IsAvailable(const char * MSISDN, const char *mask);
void SMS_Init();
void SMS_Flush();





boolean SMS_List(const char* state, const uint8_t mode)
{
	/*AT_Flush();

	unsigned long timeout = 3000;

	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s=\"%s\",%d", SMS_LIST, state, mode);
	Log("Buffer : ", AT_Buffer);
	Serial1.println(AT_Buffer);

	Serial1.setTimeout(timeout);
	memset(AT_Buffer, '\0', AT_BufferLength);
	while (Serial1.readBytes(AT_Buffer, 64))
	{
		Log(AT_Buffer);
		memset(AT_Buffer, '\0', AT_BufferLength);
	}

	return true;*/

	return false;
}


/*
[ Command ]

AT+CMGR=<index>[,<mode>]

TA returns SMS message with location value <index> from message
storage <mem1> to the TE. If status of the message is 'received unread',
status in the storage changes to 'received read'.


[ Parameters ]

<index>		Integer type; value in the range of location numbers supported by the associated memory

<mode>		0 Normal
			1 Not change status of the specified SMS record


[ Response ]

OK

*/
boolean SMS_Read(const uint8_t index, const uint8_t mode, char *buffer = AT_Buffer, uint8_t bufferLength = AT_BufferLength)
{
	memset(buffer, '\0', bufferLength);
	sprintf(buffer, "%s=%d,%d", SMS_READ, index, mode);

	return AT_Send(SIM808 , buffer, AT_OK, 5000, buffer, bufferLength);
}


/*
[ Command ]

AT + CMGD = <index>[, <delflag>]

TA deletes message from preferred message storage <mem1> location <index>.


[ Parameters ]

<index>		value in the range of location numbers supported by the associated memory

<delflag>	0 Delete the message specified in <index>
1 Delete all read messages from preferred message storage, leaving unread messages and stored mobile originated messages (whether sent or not) untouched
2 Delete all read messages from preferred message storage and sent mobile originated messages, leaving unread messages and unsent mobile originated messages untouched
3 Delete all read messages from preferred message storage, sent and unsent mobile originated messages leaving unread messages untouched
4 Delete all messages from preferred message storage including unread messages


[ Max Response Time ]

5s	delete 1 message
25s	delete 50 messages
25s	delete 150 messages


[ Response ]

OK

*/
boolean SMS_Delete(const uint8_t index, const uint8_t flag)
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s=%d,%d", SMS_DELETE, index, flag);

	return AT_Send(SIM808, AT_OK, 25000);
}


boolean SMS_DeleteAll()
{
	return SMS_Delete(1, 4);
}


void SMS_Parse(const char *source = AT_Buffer, char *MSISDN = SMS_MSISDN, char *mask=SMS_Mask, char *timestamp = SMS_Timestamp, char *data = SMS_Data)
{
	// clearing globals
	SMS_Flush();


	// creating local buffer to store source
	const uint8_t bufferLength = strlen(source) + 1;
	char *buffer = new char[bufferLength];
	strcpy(buffer, source);

	char *header;

	/*
	[ FORMAT ]	\r\n+CMGR: "REC UNREAD","+8801920961634","","17/07/25,17:48:29+24"\r\nAsalamu alikum. I am Ahnaf.\r\n\r\nOK\r\n
	[ FORMAT ]	\r\n+CMGR: "REC UNREAD","+8801915002504","","17/07/31,11:09:07+24"\r\nHello dear,\nHow are you?\nTake care. Goodbye\nYours,\nAnonymous friend\r\n\r\nOK\r\n
	*/

	char *token = NULL;

	token = strtok(buffer, "\r\n"); // gives everything before data
	header = new char[strlen(token) + 1]; // token includes \0. So do not need to worry
	strcpy(header, token);
	Log(F("\nHeader : "), header);

	token = strtok(NULL, "\r");		// gives the message body with an extra \n at the beginning
	strcpy(data, ++token);			// copies the message discarding the \n

	token = strtok(header, "\"");	// gives string '+CMGR: '

	token = strtok(NULL, "\"");		// gives 'REC UNREAD'
	
	token = strtok(NULL, ",");	// '"+8801920961634"'
	strncpy(MSISDN, ++token, strlen(token) - 2);

	token = strtok(NULL, ",");	// '"GP INTERNET"'
	strncpy(mask, ++token, strlen(token) - 2);

	token = strtok(NULL, ",\"");	// gives "17/07/25"
	strcpy(timestamp, token);
	token = strtok(NULL, "\"");		// "17:48:29+24"
	strcat(timestamp, ",");
	strcat(timestamp, token);

	Log(F("\nData : "), data);
	Log(F("\nMSISDN : "), MSISDN);
	Log(F("Mask : "), mask);
	Log(F("Timestamp : "), timestamp);

	delete[] header;
	delete[] buffer;
}



boolean SMS_Send(const char* MSISDN, const char* text)
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s=\"%s\",%d", SMS_SEND, MSISDN, 145);

	if (AT_Send(SIM808, ">", 1000))
	{
		memset(AT_Buffer, '\0', AT_BufferLength);
		sprintf(AT_Buffer, "\r%s%c", text, 26); // Ctrl-Z = 26

		return AT_Send(SIM808, AT_OK, 10000); // max response time 60s
	}

	return false;
}


boolean SMS_SetFormat(const char *format)
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s=%s", SMS_SELECT_FORMAT, format);

	return AT_Send(SIM808, AT_OK, 1000);
}


boolean SMS_IsAvailable(const char * MSISDN, const char *mask=NULL)
{
	boolean statusMSISDN = false;
	boolean statusMask = false;


	if (MSISDN == NULL && mask == NULL)
	{
		return false;
	}

	SMS_Read(1, 0);

	char *ptrSMS = strstr(AT_Buffer, "+CMGR: ");

	if (ptrSMS != NULL)
	{
		SMS_Parse(ptrSMS);
		
		Log(F("SMS_MSISDN : "), SMS_MSISDN);
		Log(F("MSISDN : "), MSISDN);
		Log(F("\nSMS_Mask : "), SMS_Mask);
		Log(F("Mask : "), mask);
		
		statusMSISDN = strcmp(SMS_MSISDN, MSISDN) == 0;
		
		if (strcmp(mask, "") && mask != NULL)
		{
			statusMask = strcmp(SMS_Mask, mask) == 0;
		}

		if (!(statusMSISDN || statusMask)) {
			SMS_Delete(1, 0);
		}
	}

	Log(F("\n[ Status MSISDN ] : "), statusMSISDN);
	Log(F("[ Status Mask ] : "), statusMask);
	Log(F("[ Status MSISDN || Status Mask ] : "), (statusMSISDN || statusMask));


	return (statusMSISDN || statusMask);
}


boolean SMS_InboxEmpty()
{
	SMS_Read(1, 0);

	char *ptrSMS = strstr(AT_Buffer, "+CMGR: ");

	if (ptrSMS != NULL)
	{
		SMS_Parse(ptrSMS);

		Log(F("SMS_MSISDN : "), SMS_MSISDN);
		Log(F("\nSMS_Mask : "), SMS_Mask);

		return !strcmp(SMS_MSISDN, "") && !strcmp(SMS_Mask, "") && !strcmp(SMS_Data, "");
	}

	return true;
}



void SMS_Init()
{
	while(!SMS_SetFormat("1"));
	while(!SMS_DeleteAll());
}


void SMS_Flush()
{
	memset(SMS_Data, '\0', SMS_DataLength);
	memset(SMS_MSISDN, '\0', SMS_BufferLength);
	memset(SMS_Mask, '\0', SMS_BufferLength);
	memset(SMS_Timestamp, '\0', SMS_BufferLength);
}







#endif // !SMS_h

