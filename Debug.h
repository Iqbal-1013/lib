#ifndef Debug_h 
#define Debug_h


#include "SerialManager.h"


const bool debug = true;

template<typename T>
inline void Log(const T& value)
{
	if (debug)
	{
		CONSOLE.println(value);
	}
}


template<typename T1, typename T2>
inline void Log(const T1 *label, const T2& value)
{
	if (debug)
	{
		CONSOLE.print(label);
		Log(value);
	}
}


template<typename T1, typename T2>
inline void Log(const T1 *preLabel, const T2& value, const T1 *postLabel)
{
	if (debug)
	{
		CONSOLE.print(preLabel);
		CONSOLE.print(value);
		CONSOLE.print(postLabel);
	}
}



inline void Log()
{
	if (debug)
	{
		CONSOLE.println();
	}
}


template<typename T>
inline void Log_All(const T *str, uint16_t len=0)
{
	if (debug)
	{
		len = len == 0 ? strlen(str) : len;

		for (uint16_t i = 0; i <= len; i++)
		{
			if ((int)str[i] == 0)
			{
				CONSOLE.print("\\0");
			}
			else if ((int)str[i] == 1)
			{
				CONSOLE.print("[START OF HEADING]");
			}
			else if ((int)str[i] == 2)
			{
				CONSOLE.print("[START OF TEXT]");
			}
			else if ((int)str[i] == 3)
			{
				CONSOLE.print("[END OF TEXT]");
			}
			else if ((int)str[i] == 4)
			{
				CONSOLE.print("[END OF TRANSMISSION]");
			}
			else if ((int)str[i] == 5)
			{
				CONSOLE.print("[ENQUIRY]");
			}
			else if ((int)str[i] == 6)
			{
				CONSOLE.print("[ACK]");
			}
			else if ((int)str[i] == 7)
			{
				CONSOLE.print("[BELL]");
			}
			else if ((int)str[i] == 8)
			{
				CONSOLE.print("[BACKSPACE]");
			}
			else if ((int)str[i] == 9)
			{
				CONSOLE.print("[HORIZONTAL TAB]");
			}
			else if ((int)str[i] == 10)
			{
				CONSOLE.print("\\n");
			}
			else if ((int)str[i] == 11)
			{
				CONSOLE.print("[VERTICAL TAB]");
			}
			else if ((int)str[i] == 12)
			{
				CONSOLE.print("[FORM FEED]");
			}
			else if ((int)str[i] == 13)
			{
				CONSOLE.print("\\r");
			}
			else if ((int)str[i] == 14)
			{
				CONSOLE.print("[SHIFT OUT]");
			}
			else if ((int)str[i] == 15)
			{
				CONSOLE.print("[SHIFT IN]");
			}
			else if ((int)str[i] == 16)
			{
				CONSOLE.print("[DATA LINK ESCAPE]");
			}
			else if ((int)str[i] == 17)
			{
				CONSOLE.print("[DEVICE CONTROL 1]");
			}
			else if ((int)str[i] == 18)
			{
				CONSOLE.print("[DEVICE CONTROL 2]");
			}
			else if ((int)str[i] == 19)
			{
				CONSOLE.print("[DEVICE CONTROL 3]");
			}
			else if ((int)str[i] == 20)
			{
				CONSOLE.print("[DEVICE CONTROL 4]");
			}
			else if ((int)str[i] == 21)
			{
				CONSOLE.print("[NEGATIVE ACK]");
			}
			else if ((int)str[i] == 22)
			{
				CONSOLE.print("[SYNC IDLE]");
			}
			else if ((int)str[i] == 23)
			{
				CONSOLE.print("[END OF TRANSFER BLOCK]");
			}
			else if ((int)str[i] == 24)
			{
				CONSOLE.print("[CANCEL]");
			}
			else if ((int)str[i] == 25)
			{
				CONSOLE.print("[END OF MEDIUM]");
			}
			else if ((int)str[i] == 26)
			{
				CONSOLE.print("[SUBSTITUTE]");
			}
			else if ((int)str[i] == 27)
			{
				CONSOLE.print("[ESCAPE]");
			}
			else if ((int)str[i] == 28)
			{
				CONSOLE.print("[FILE SEPARATOR]");
			}
			else if ((int)str[i] == 29)
			{
				CONSOLE.print("[GROUP SEPARATOR]");
			}
			else if ((int)str[i] == 30)
			{
				CONSOLE.print("[RECORD SEPARATOR]");
			}
			else if ((int)str[i] == 31)
			{
				CONSOLE.print("[UNIT SEPARATOR]");
			}
			else if ((int)str[i] == 127)
			{
				CONSOLE.print("[DEL]");
			}
			else
			{
				CONSOLE.print(str[i]);
			}
		}

		CONSOLE.println();
	}
}


template<typename T>
inline void Log_All(const T* label, const char* str)
{
	if (debug)
	{
		CONSOLE.print(label);
		Log_All(str);
	}
}


template<typename T>
inline void Log_Time(const T *label, const unsigned long &stop, const unsigned long &start)
{
	if (debug)
	{
		CONSOLE.print(label);
		CONSOLE.print((stop - start) / 1000.0);
		CONSOLE.println(" seconds");
	}
}


inline void Time()
{
	static unsigned long time = millis();

	Log_Time(F("\nELAPSED TIME : "), millis(), time);
	time = millis();	
}


inline float Time(unsigned long start)
{
	return (millis() - start ) / 1000.0;
}


#endif // Debug_h 
