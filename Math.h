#ifndef Math_h
#define Math_h


#include "Arduino.h"
#include "Debug.h"
#include "stdlib.h"


namespace Math
{
	const uint16_t bufferLength = 32;
	char buffer[bufferLength] = { '\0' };


	// Converts float to string
	// buffer			: stores the converted float value
	// width			: minimum width of the output converted string with sign
	// precision	: set the number of digits after the decimal point
	char *FloatToString(float &val, uint16_t width=Math::bufferLength-1, uint16_t precision=6, char *buffer=Math::buffer, uint16_t bufferLength=Math::bufferLength)
	{
		memset(buffer, '\0', bufferLength);
		dtostrf(val, width, precision, buffer);

		uint16_t spaces = 0;
		
		for (uint16_t i = 0; i < bufferLength; i++)
		{
			if (buffer[i] == ' ')
			{
				spaces++;
			}
			else
			{
				break;
			}
		}

		strncpy(buffer, buffer + spaces, bufferLength - spaces);

		return buffer;
	}


}





#endif


