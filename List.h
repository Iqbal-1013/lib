#ifndef List_h
#define List_h


#include "Arduino.h"


template<typename T>
class ListItem
{

public:

	T item;
	ListItem<T> *next = NULL;

	ListItem(T item, ListItem<T> *next = NULL) : item(item), next(next)
	{}
};



template<typename T>
class List
{

public:

	uint16_t size = 0;
	ListItem<T> *start = NULL;

	List()
	{}

	~List()
	{
		Clear();
	}


	// Adds item at the index or at the end if index is greater than size.
	boolean Add(T item, uint16_t index=0)
	{
		if (index < 0)
		{
			return false;
		}

		ListItem<T> *ptr = start;
		ListItem<T> *prev_ptr = NULL;

		for (uint16_t i = 0; i < index; i++)
		{
			if (ptr == NULL)
			{
				break;
			}

			prev_ptr = ptr;
			ptr = ptr->next;
		}

		auto list_item = new ListItem<T>(item, ptr);

		if (ptr == NULL || index == 0)
		{
			start = list_item;
		}

		if (prev_ptr != NULL)
		{
			prev_ptr->next = list_item;
		}


		size++;

		return true;
	}

	// Removes item at the index
	boolean Remove(uint16_t index)
	{
		if (index < 0)
		{
			return false;
		}

		ListItem<T> *ptr = start;
		ListItem<T> *prev_ptr = NULL;

		for (uint16_t i = 0; i<index; i++)
		{
			if (ptr == NULL)
			{
				break;
			}

			prev_ptr = ptr;
			ptr = ptr->next;
		}

		if (ptr == NULL)
		{
			return false;
		}

		if (index == 0)
		{
			start = ptr->next;
		}

		if (prev_ptr != NULL)
		{
			prev_ptr->next = ptr->next;
		}

		delete ptr;
		size--;

		return true;
	}

	void Clear()
	{
		while (size > 0)
		{
			Remove(0);
		}
	}

	T Get(uint16_t index)
	{
		auto ptr = start;

		for (uint16_t i = 0; i<index; i++)
		{
			if (ptr == NULL)
			{
				break;
			}

			ptr = ptr->next;
		}

		if (ptr != NULL)
		{
			return ptr->item;
		}

		return NULL;
	}
};




#endif // !List_h
