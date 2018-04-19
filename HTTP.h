#ifndef HTTP_h
#define HTTP_h


#include "Arduino.h"
#include "BearerProfile.h"
#include "AT.h"
#include "Watchdog.h"



#define HTTTP_INIT "AT+HTTPINIT"
#define HTTP_PARAM "AT+HTTPPARA"
#define HTTP_DATA "AT+HTTPDATA"
#define HTTP_DOWNLOAD "DOWNLOAD"
#define HTTP_ACTION "AT+HTTPACTION"
#define HTTP_READ "AT+HTTPREAD"
#define HTTP_TERM "AT+HTTPTERM"
#define HTTP_STATUS "AT+HTTPSTATUS"
#define HTTP_CONTEXT "AT+HTTPSCONT"




const uint16_t HTTP_BufferLength = 1024;
char HTTP_Buffer[HTTP_BufferLength] = { '\0' };

uint8_t HTTP_URC_Method = 0;
uint16_t HTTP_URC_StatusCode = 0;
uint16_t HTTP_URC_DataLength = 0;


boolean HTTP_Start();
boolean HTTP_Stop();
boolean HTTP_SetParameter(const char* tag, const uint8_t value);
boolean HTTP_SetParameter(const char* tag, const char* value);
boolean HTTP_InputData(const char *data, const unsigned long time);
boolean HTTP_Action(const char *method, const unsigned long timeout);
boolean HTTP_Read(uint16_t startAddress, uint16_t byteSize);
boolean HTTP_Post(const char* url, char* dataBuffer, const uint16_t dataBufferLength, const uint8_t cid);
boolean HTTP_SafePost(const char* url, char* dataBuffer, const uint16_t dataBufferLength, const uint8_t cid);
boolean HTTP_ContextRead();
boolean HTTP_Status();
void HTTP_Parse(char *str);
boolean HTTP_WaitForStatusCode(const char *method, unsigned long timeout);
void HTTP_ParseURC(const char *str = AT_Buffer, uint8_t &method = HTTP_URC_Method, uint16_t &statusCode = HTTP_URC_StatusCode, uint16_t &dataLength = HTTP_URC_DataLength);
void HTTP_Flush();


int send_HTTP(char* dataBuffer = HTTP_Buffer);

char aux_str[400];








boolean HTTP_Start()
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s", HTTTP_INIT);

	return AT_Send(SIM808, AT_OK, 5000);
}


boolean HTTP_Stop()
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s", HTTP_TERM);

	return AT_Send(SIM808, AT_OK, 5000);
}


boolean HTTP_SetParameter(const char* tag, const uint8_t value)
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s=\"%s\",%d", HTTP_PARAM, tag, value);

	return AT_Send(SIM808, AT_OK, 3000);
}


boolean HTTP_SetParameter(const char* tag, const char* value)
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s=\"%s\",\"%s\"", HTTP_PARAM, tag, value);

	return AT_Send(SIM808, AT_OK, 3000);
}


boolean HTTP_InputData(const char *data, const unsigned long time)
{
	if (strlen(data) == 0)
	{
		return true;
	}

	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s=%d,%lu", HTTP_DATA, strlen(data), time);

	if (AT_Send(SIM808, HTTP_DOWNLOAD, 3000))
	{
		if (AT_Send(SIM808, data, AT_OK, time))
		{
			return true;
		}
	}

	return false;
}


boolean HTTP_Action(const char *method, const unsigned long timeout)
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s=%s", HTTP_ACTION, method);

	if (AT_Send(SIM808, AT_OK, 3000))
	{
		// wait for +HTTPACTION: 1,200
		if (waitFor(SIM808, strlen(AT_Buffer), "+HTTPACTION", timeout))
		{
			return waitFor(SIM808, strlen(AT_Buffer), "+HTTPACTION: 1,200", 1000);
		}

		/*if (HTTP_WaitForStatusCode(method, timeout))
		{
		HTTP_ParseURC();

		Log("\n[ METHOD ] : ", HTTP_URC_Method);
		Log("\n[ STATUS CODE ] : ", HTTP_URC_StatusCode);
		Log("\n[ DATA LENGTH ] : ", HTTP_URC_DataLength);
		}*/
	}

	return false;
}


boolean HTTP_Read(uint16_t startAddress, uint16_t byteSize)
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s=%d,%d", HTTP_READ, startAddress, byteSize);

	return AT_Send(SIM808, AT_Buffer, AT_OK, 3000, HTTP_Buffer, HTTP_BufferLength);
}


boolean  HTTP_Post(const char* url, char* dataBuffer = HTTP_Buffer, const uint16_t dataBufferLength = HTTP_BufferLength, const uint8_t cid = bearerProfile)
{
	boolean status = false;

	while (!HTTP_Start())
	{
		HTTP_Stop();
	}

	HTTP_SetParameter("CID", cid);
	HTTP_SetParameter("URL", url);
	HTTP_SetParameter("TIMEOUT",30);
	HTTP_SetParameter("CONTENT", "application/x-www-form-urlencoded");


	HTTP_InputData(dataBuffer, 2000);

	if (HTTP_Action("1", 30000))
	{
		status = true;
		HTTP_Read(0, HTTP_BufferLength);
	}

	HTTP_Stop();


	return status;
}


boolean HTTP_ContextRead()
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s?", HTTP_CONTEXT);

	return AT_Send(SIM808, AT_OK, 1000);
}


boolean HTTP_ContextSave()
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s", HTTP_CONTEXT);

	return AT_Send(SIM808, AT_OK, 1000);
}


boolean HTTP_Status()
{
	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s?", HTTP_STATUS);

	return AT_Send(SIM808, AT_OK, 3000);
}


void HTTP_Parse(char *str = HTTP_Buffer)
{
	const uint16_t bufferLength = strlen(str) + 1;
	char *buffer = new char[bufferLength];
	strcpy(buffer, str);

	char *token = strtok(buffer, ":");
	token = strtok(NULL, "\n");
	token = strtok(NULL, "\r");

	HTTP_Flush();
	strcpy(HTTP_Buffer, token);

	Log(F("HTTP Data : "), HTTP_Buffer);

	delete[] buffer;
}


void HTTP_Flush()
{
	memset(HTTP_Buffer, '\0', HTTP_BufferLength);
}


boolean HTTP_SafePost(const char* url, char* dataBuffer = HTTP_Buffer, const uint16_t dataBufferLength = HTTP_BufferLength, const uint8_t cid = bearerProfile)
{
	boolean status = false;

	while (!HTTP_Start())
	{
		HTTP_Stop();
		return false;
	}

	while (!HTTP_SetParameter("CID", cid));
	while (!HTTP_SetParameter("URL", url));
	while (!HTTP_SetParameter("CONTENT", "application/x-www-form-urlencoded"));


	while (!HTTP_InputData(dataBuffer, 2000));

	Watchdog::Reset();
	if (HTTP_Action("1", 30000))
	{

		status = true;
		HTTP_Read(0, HTTP_BufferLength);

	}


	delay(500);
	HTTP_Stop();


	return status;
}


int send_HTTP(const char* url, char* dataBuffer = HTTP_Buffer) {

	int answer = 0;

	// Initializes HTTP service
	Log(F("HTTP Initializes"));
	answer = sendATcommand("AT+HTTPINIT", "OK", 10000);
	delay(300);
	if (answer == 1)
	{
		// Sets CID parameter
		answer = sendATcommand("AT+HTTPPARA=\"CID\",1", "OK", 6000);
		delay(300);

		if (answer == 1)
		{
			answer = sendATcommand("AT+HTTPPARA=\"TIMEOUT\",30", "OK",5000);

			if (answer == 1)
			{
				// Sets url 
				memset(aux_str, '\0', 400);
				sprintf(aux_str, "AT+HTTPPARA=\"URL\",\"%s?", url);

				SIM808.write(aux_str);
				CONSOLE.print(aux_str);
				delay(500); 

				SIM808.write(dataBuffer);
				CONSOLE.print(dataBuffer);

				answer = sendATcommand("\"", "OK", 6000);


				if (answer == 1)
				{
					// Starts GET action
					answer = sendATcommand("AT+HTTPACTION=1", "+HTTPACTION: 1,200", 32000);
					delay(300);

					if (answer == 1)
					{
						Log(F("\nDone!"));
						if (!HTTP_Read(0, HTTP_BufferLength)) {
							HTTP_Stop();
						}

						HTTP_Parse();
					}
					else
					{
						Log(F("\n\nError httpaction\n\n"));
						if (sendATcommand("AT+HTTPTERM", "OK", 6000) == 1) {
							Log("\n\n[ HTTP Terminated successfully ]\n\n");
						}
						else {
							Log("\n\n[ HTTP Termina sution failed ]\n\n");
						}


						return answer;
					}
				}
				else
				{
					if (sendATcommand("AT+HTTPTERM", "OK", 6000) == 1) {
						Log("\n\n[ HTTP Terminated successfully ]\n\n");
					}
					else {
						Log("\n\n[ HTTP Termina sution failed ]\n\n");
					}
					Log(F("Error setting the url"));
					return answer;
				}
			}
			else
			{
				if (sendATcommand("AT+HTTPTERM", "OK", 6000) == 1) {
					Log("\n\n[ HTTP Terminated successfully ]\n\n");
				}
				else {
					Log("\n\n[ HTTP Termina sution failed ]\n\n");
				}
				Log(F("Error setting the TIMEOUT"));
				return answer;
			}
		}
		else
		{
			if (sendATcommand("AT+HTTPTERM", "OK", 6000) == 1) {
				Log("\n\n[ HTTP Terminated successfully ]\n\n");
			}
			else {
				Log("\n\n[ HTTP Termina sution failed ]\n\n");
			}
			Log(F("Error setting the CID"));
			return answer;
		}
	}
	else
	{
		if (sendATcommand("AT+HTTPTERM", "OK", 6000) == 1) {
			Log("\n\n[ HTTP Terminated successfully ]\n\n");
		}
		else {
			Log("\n\n[ HTTP Termina sution failed ]\n\n");
		}
		Log(F("Error initializating"));
		return answer;
	}
	Log(F("HTTP Global Termination "));

	if (sendATcommand("AT+HTTPTERM", "OK", 6000) == 1) {
		Log("\n\n[ HTTP Terminated successfully ]\n\n");
	}
	else {
		Log("\n\n[ HTTP Termination failed ]\n\n");
	}
	return answer;

}


//boolean HTTP_WaitForStatusCode(const char *method, unsigned long timeout)
//{
//	memset(HTTP_Buffer, '\0', HTTP_BufferLength);
//	sprintf(HTTP_Buffer, "%s: %s,", "+HTTPACTION", method);
//
//	return waitForURC(SIM808, HTTP_Buffer, timeout);
//}

/*
+HTTPACTION: <Method>,<StatusCode>,<DataLen>
*/
//void HTTP_ParseURC(const char *str=AT_Buffer, uint8_t &method=HTTP_URC_Method, uint16_t &statusCode=HTTP_URC_StatusCode, uint16_t &dataLength=HTTP_URC_DataLength)
//{
//	const uint8_t bufferLength = strlen(str) + 1;
//	char *buffer = new char[bufferLength];
//	strcpy(buffer, str);
//
//
//	char *token = strtok(buffer, ":"); // ignores +HTTPACTION
//	
//	token = strtok(NULL, ",");
//	method = atoi(token);
//
//	token = strtok(NULL, ",");
//	statusCode = atoi(token);
//
//	token = strtok(NULL, ",");
//	dataLength = atoi(token);
//
//
//	delete[] buffer;
//}


#endif // !HTTP_h

