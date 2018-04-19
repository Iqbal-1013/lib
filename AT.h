#ifndef AT_h
#define AT_h


#include "Arduino.h"
#include "SerialManager.h"
#include "Debug.h"
#include "BearerProfile.h"
#include "Watchdog.h"





#define AT_OK "OK"
#define AT_ERROR "ERROR"
#define AT_CME_ERROR "+CME ERROR"
#define AT_CMS_ERROR "+CMS ERROR"

#define GPRS_BEARER_SETTINGS "AT+SAPBR"




const uint8_t AT_BufferLength = 255;
char AT_Buffer[AT_BufferLength] = { '\0' };
boolean network_error = false;
int8_t answer = 0;





void AT_Flush(HardwareSerial &serial);


boolean AT_Send(HardwareSerial &serial, const char* command, const char* expectedResponse, unsigned long timeout, char *responseBuffer, const uint16_t bufferLength);
boolean AT_Send(HardwareSerial &serial, const char* command, const char* expectedResponse, unsigned long timeout);
boolean AT_Send(HardwareSerial &serial, const char *expectedResponse, unsigned long timeout);

//int8_t AT_Send(HardwareSerial &serial, const char* ATcommand, const char* expected_answer, unsigned long timeout);



boolean AT_GPRS_Init(const uint8_t cid);
void AT_GPRS_STOP();

boolean waitFor(HardwareSerial &serial, uint16_t index, const char* expectedResponse, unsigned long timeout, char *responseBuffer, const uint16_t bufferLength);
boolean waitFor(HardwareSerial &serial, uint16_t index, const char* expectedResponse, unsigned long timeout);
boolean waitFor(HardwareSerial &serial, const char* expectedResponse, unsigned long timeout);


boolean waitForURC(HardwareSerial &serial, uint16_t index, const char* expectedResponse, unsigned long timeout, char *responseBuffer, const uint16_t bufferLength);
boolean waitForURC(HardwareSerial &serial, uint16_t index, const char* expectedResponse, unsigned long timeout);
boolean waitForURC(HardwareSerial &serial, const char* expectedResponse, unsigned long timeout);


boolean AT_SendOnlyOK(HardwareSerial &serial, const char* command, const char* expectedResponse, unsigned long timeout, char *responseBuffer, const uint16_t bufferLength);
boolean AT_SendOnlyOK(HardwareSerial &serial, const char* command, const char* expectedResponse, unsigned long timeout);
boolean AT_SendOnlyOK(HardwareSerial &serial, const char *expectedResponse, unsigned long timeout);

boolean waitForOnlyOK(HardwareSerial &serial, uint16_t index, const char* expectedResponse, unsigned long timeout, char *responseBuffer, const uint16_t bufferLength);
boolean waitForOnlyOK(HardwareSerial &serial, uint16_t index, const char* expectedResponse, unsigned long timeout);
boolean waitForOnlyOK(HardwareSerial &serial, const char* expectedResponse, unsigned long timeout);


int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout);


boolean AT_Send(HardwareSerial &serial, const char* command, const char* expectedResponse, unsigned long timeout, char *responseBuffer, const uint16_t bufferLength)
{
	AT_Flush(serial);

	Log(F("\n\nCommand : "), command);
	serial.println(command);				// automatically appeands \r\n at the end

											// When only single buffer is used for both command and response, responseBuffer should be cleared after command is sent. 
	memset(responseBuffer, '\0', bufferLength);

	return waitFor(serial, 0, expectedResponse, timeout, responseBuffer, bufferLength);
}

void AT_GPRS_STOP() {

	memset(AT_Buffer, '\0', AT_BufferLength);
	sprintf(AT_Buffer, "%s=%s,%d", GPRS_BEARER_SETTINGS, "0", 1);


	{
		sprintf(AT_Buffer, "%s,\"%s\",\"%s\"", AT_Buffer, NULL, NULL);
	}

	sendATcommand(AT_Buffer, AT_OK, 5000);

}

boolean AT_Send(HardwareSerial &serial, const char* command, const char* expectedResponse, unsigned long timeout)
{
	return AT_Send(serial, command, expectedResponse, timeout, AT_Buffer, AT_BufferLength);
}

boolean AT_Send(HardwareSerial &serial, const char *expectedResponse, unsigned long timeout)
{
	return AT_Send(serial, AT_Buffer, expectedResponse, timeout, AT_Buffer, AT_BufferLength);
}



boolean AT_SendOnlyOK(HardwareSerial &serial, const char* command, const char* expectedResponse, unsigned long timeout, char *responseBuffer, const uint16_t bufferLength)
{
	AT_Flush(serial);

	Log(F("\n\nCommand : "), command);
	serial.println(command);				// automatically appeands \r\n at the end

											// When only single buffer is used for both command and response, responseBuffer should be cleared after command is sent. 
	memset(responseBuffer, '\0', bufferLength);

	return waitForOnlyOK(serial, 0, expectedResponse, timeout, responseBuffer, bufferLength);
}

boolean AT_SendOnlyOK(HardwareSerial &serial, const char* command, const char* expectedResponse, unsigned long timeout)
{
	return AT_SendOnlyOK(serial, command, expectedResponse, timeout, AT_Buffer, AT_BufferLength);
}

boolean AT_SendOnlyOK(HardwareSerial &serial, const char *expectedResponse, unsigned long timeout)
{
	return AT_SendOnlyOK(serial, AT_Buffer, expectedResponse, timeout, AT_Buffer, AT_BufferLength);
}



//boolean waitFor(uint16_t index, const char* expectedResponse, unsigned long timeout, char *responseBuffer, const uint16_t bufferLength)
//{
//	Log("Index : ", index);
//	Log("Expected response : ", expectedResponse);
//	Log("Timeout : ", timeout);
//	Log("Response buffer : ", responseBuffer);
//	Log("Buffer length : ", bufferLength);
//
//	boolean status = false;
//	unsigned long start = millis();
//	const uint8_t bytesToRead = 64;
//
//	while (millis() - start < timeout && (index + bytesToRead) < bufferLength)
//	{
//		index += Serial1.readBytes(responseBuffer + index, bytesToRead);
//
//		Log("Index : ", index);
//		Log("Response : ", responseBuffer);
//		Log_All("Response all : ", responseBuffer);
//
//		if (strstr(responseBuffer, expectedResponse) != NULL)
//		{
//			status = true;
//			break;
//		}
//		else if (strstr(responseBuffer, AT_ERROR) != NULL || strstr(responseBuffer, AT_CME_ERROR) != NULL)
//		{
//			break;
//		}
//	}
//
//	// Protects from overflow
//	index == bufferLength ? responseBuffer[index - 1] = '\0' : responseBuffer[index] = '\0';
//
//	return status;
//}

int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout) {
	uint8_t x = 0, answer = 0;
	char response[200];
	unsigned long previous;
	const unsigned long watchdog_fixrd_time = 6000;
	unsigned long prev_time = 0;

	memset(response, '\0', 200);    // Initialice the string
	delay(100);
	while (SIM808.available() > 0) SIM808.read();    // Clean the input buffer
	if (ATcommand[0] != '\0')
	{
		SIM808.println(ATcommand);    // Send the AT command 
	}


	x = 0;
	previous = millis();
	prev_time = previous - 1000;
	// this loop waits for the answer
	do {
		if (timeout > 3000 && (millis() - previous) % watchdog_fixrd_time <500 && millis()>prev_time)
		{
			Watchdog::Reset();
			prev_time += watchdog_fixrd_time;
		}
		if (SIM808.available() != 0) {    // if there are data in the UART input buffer, reads it and checks for the asnwer
			response[x] = SIM808.read();
			CONSOLE.print(response[x]);
			x++;
			if (strstr(response, expected_answer) != NULL)    // check if the desired answer (OK) is in the response of the module
			{
				answer = 1;
				break;
			}

			if (strstr(response, "+HTTPACTION: 1,601,0") != NULL)    // check if the desired answer (OK) is in the response of the module
			{
				network_error = true;
				sendATcommand("AT+SAPBR=0,1", "OK", 20000);
				if (!AT_GPRS_Init(1)) {
					return -2;
				}
				return -1;
			}
		}
	} while ((millis() - previous < timeout));    // Waits for the asnwer with time out

	return answer;
}

boolean waitFor(HardwareSerial &serial, uint16_t index, const char* expectedResponse, unsigned long timeout)
{
	return waitFor(serial, index, expectedResponse, timeout, AT_Buffer, AT_BufferLength);
}

boolean waitFor(HardwareSerial &serial, const char* expectedResponse, unsigned long timeout)
{
	return waitFor(serial, 0, expectedResponse, timeout, AT_Buffer, AT_BufferLength);
}

// Experimental
boolean waitFor(HardwareSerial &serial, uint16_t index, const char* expectedResponse, unsigned long timeout, char *responseBuffer, const uint16_t bufferLength)
{
	Log(F("Index : "), index);
	Log(F("Expected response : "), expectedResponse);
	Log(F("Timeout : "), timeout);
	Log(F("Response buffer : "), responseBuffer);
	Log(F("Buffer length : "), bufferLength);

	boolean status = false;
	unsigned long start = millis();

	while ((millis() - start < timeout) && index < bufferLength)
	{
		if (serial.available())
		{
			responseBuffer[index++] = (char)serial.read();
		}

		if (strstr(responseBuffer, expectedResponse) != NULL)
		{
			status = true;
			break;
		}
		else if (strstr(responseBuffer, AT_ERROR) != NULL || strstr(responseBuffer, AT_CME_ERROR) != NULL)
		{
			break;
		}
	}

	// Protects from overflow
	index == bufferLength ? responseBuffer[index - 1] = '\0' : responseBuffer[index] = '\0';

	Log(F("Index : "), index);
	Log(F("Response : "), responseBuffer);
	Log_All(F("Response all : "), responseBuffer);
	Log(F("Status : "), status);

	return status;
}


boolean waitForOnlyOK(HardwareSerial &serial, uint16_t index, const char* expectedResponse, unsigned long timeout, char *responseBuffer, const uint16_t bufferLength)
{
	Log(F("Index : "), index);
	Log(F("Expected response : "), expectedResponse);
	Log(F("Timeout : "), timeout);
	Log(F("Response buffer : "), responseBuffer);
	Log(F("Buffer length : "), bufferLength);

	boolean status = false;
	unsigned long start = millis();

	while ((millis() - start < timeout) && index < bufferLength)
	{
		if (serial.available())
		{
			responseBuffer[index++] = (char)serial.read();
		}

		if (strstr(responseBuffer, expectedResponse) != NULL)
		{
			status = true;
			break;
		}
	}

	// Protects from overflow
	index == bufferLength ? responseBuffer[index - 1] = '\0' : responseBuffer[index] = '\0';

	Log(F("Index : "), index);
	Log(F("Response : "), responseBuffer);
	Log_All(F("Response all : "), responseBuffer);
	Log(F("Status : "), status);

	return status;
}



boolean AT_GPRS_Init(const uint8_t cid = bearerProfile)
{
	delay(500);

	while (sendATcommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"", "OK", 5000) == 0);
	Log(F("\n\n[  ConType is complete ]"));
	delay(500);
	while (sendATcommand("AT+SAPBR=3,1,\"APN\",\"blweb\"", "OK", 5000) == 0);
	Log(F("\n\n[  APN is complete ]"));
	delay(500);
	//sendATcommand("AT+SAPBR=0,1", "OK", 20000);
	//Log(F("\n\n[  Beginning GPRS Start ]"));
	if (sendATcommand("AT + CGATT?", "+CGATT: 1", 5000) == 0 ){
		while (sendATcommand("AT+CGATT=1", "OK", 10000) == 0) {
			Log("\n\n[ Attatch Failed ]\n\n");
			delay(500);
		}
		Log("\n\n[ Attatch Successful ]\n\n");
	}

	int counter = -1;

	// gets the GPRS bearer
	while (sendATcommand("AT+SAPBR=1,1", "OK", 20000) == 0)
	{

		Watchdog::Reset();

		Log(F("\n\n[  Failed : Stoping GPRS ]"));
		delay(1000);
		sendATcommand("AT+SAPBR=0,1", "OK", 20000);
		delay(1000);
		Log(F("\n\n[  Retrying GPRS Start ]"));

		if (++counter == 3) {
			break;
		}

	}

	if (counter >= 3) {

		Log("\n\n[Stopping shield]\n\n");

		digitalWrite(21, HIGH);
		delay(2000);
		digitalWrite(21, LOW);
		delay(2000);
		return false;
		
	}
	return true;
}





boolean waitForOnlyOK(HardwareSerial &serial, uint16_t index, const char* expectedResponse, unsigned long timeout)
{
	return waitForOnlyOK(serial, index, expectedResponse, timeout, AT_Buffer, AT_BufferLength);
}

boolean waitForOnlyOK(HardwareSerial &serial, const char* expectedResponse, unsigned long timeout)
{
	return waitForOnlyOK(serial, 0, expectedResponse, timeout, AT_Buffer, AT_BufferLength);
}



//boolean waitForURC(HardwareSerial &serial, uint16_t index, const char* expectedResponse, unsigned long timeout, char *responseBuffer, const uint16_t bufferLength)
//{
//	Log(F("Index : "), index);
//	Log(F("Expected response : "), expectedResponse);
//	Log(F("Timeout : "), timeout);
//	Log(F("Response buffer : "), responseBuffer);
//	Log(F("Buffer length : "), bufferLength);
//
//	boolean status = false;
//	unsigned long start = millis();
//
//	while ((millis() - start < timeout) && index < bufferLength)
//	{
//		Log(F("Status : "), status);
//
//		if (serial.available())
//		{
//			responseBuffer[index++] = (char)serial.read();
//		}
//
//		if (strstr(responseBuffer, expectedResponse) != NULL)
//		{
//			status = true;
//		}
//
//		if (status && strstr(responseBuffer, "\r\n\0") != NULL)
//		{
//			Log_All(F("Response all : "), responseBuffer);
//			//break;
//		}
//	}
//
//	// Protects from overflow
//	index == bufferLength ? responseBuffer[index - 1] = '\0' : responseBuffer[index] = '\0';
//
//	Log(F("Index : "), index);
//	Log(F("Response : "), responseBuffer);
//	Log_All(F("Response all : "), responseBuffer);
//	Log(F("Status : "), status);
//
//	return status;
//}
//
//boolean waitForURC(HardwareSerial &serial, uint16_t index, const char* expectedResponse, unsigned long timeout)
//{
//	return waitForURC(serial, index, expectedResponse, timeout, AT_Buffer, AT_BufferLength);
//}
//
//boolean waitForURC(HardwareSerial &serial, const char* expectedResponse, unsigned long timeout)
//{
//	return waitForURC(serial, 0, expectedResponse, timeout, AT_Buffer, AT_BufferLength);
//}


//
//int8_t AT_Send(HardwareSerial &serial, const char* ATcommand, const char* expected_answer, unsigned long timeout)
//{
//	Log("\n[ Guest AT Function ]\n");
//	uint8_t x = 0, answer = 0;
//	char response[100];
//	unsigned long previous;
//
//	memset(response, '\0', 100);    // Initialice the string
//
//	delay(100);
//
//	while (serial.available() > 0)
//		serial.read();    // Clean the input buffer
//
//	if (ATcommand[0] != '\0')
//	{
//		serial.println(ATcommand);    // Send the AT command 
//	}
//
//
//	x = 0;
//	previous = millis();
//
//	// this loop waits for the answer
//	do
//	{
//		if (serial.available() != 0)
//		{    // if there are data in the UART input buffer, reads it and checks for the asnwer
//			response[x] = serial.read();
//			Serial.print(response[x]);
//			x++;
//			if (strstr(response, expected_answer) != NULL)    // check if the desired answer (OK) is in the response of the module
//			{
//				answer = 1;
//			}
//		}
//	}
//	while ((answer == 0) && ((millis() - previous) < timeout));    // Waits for the asnwer with time out
//
//	return answer;
//}



void AT_Flush(HardwareSerial &serial)
{
	while (serial.available())
	{
		serial.read();
	}
}








#endif // !AT_h
