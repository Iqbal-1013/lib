#ifndef Timer_h
#define Timer_h


namespace Timer
{ 
	unsigned long start = 0;


	void Start()
	{
		start = millis();
	}

	void Start(unsigned long &start)
	{
		start = millis();
	}

	unsigned long Duration()
	{
		return millis() - start;
	}

	unsigned long Duration(unsigned long &start)
	{
		return millis() - start;
	}
}







#endif // !Timer_h
