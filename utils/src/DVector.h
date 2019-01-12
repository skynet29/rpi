
#ifndef DVECTOR_H
#define DVECTOR_H

#include <string.h>

typedef unsigned int uint;

template <class T>
class DVector
{
public:
	DVector(uint initSize = 1, uint growSize = 1)
	{
		size = initSize;
		this->growSize = growSize;
		data = new T[size];
		count = 0;
	}
	
	~DVector()
	{
		delete [] data;
	}
	
	bool Add(T item)
	{		
		if (count == size)
		{
			if (growSize == 0)
				return false;
				
			Grow();
		}
				
		data[count++] = item;
		return true;
	}
	

	
	bool RemoveAt(uint idx)
	{
		if (idx < count)
		{
			
			if (idx != count-1)
				memmove(&data[idx], &data[idx+1], (count-idx-1) * sizeof(T));
			count--;
			return true;
		}
		return false;
	}
	
	void RemoveAll()
	{
		count = 0;
	}
	
	uint GetCount() {return count;}
	
	T operator[](uint idx) {return data[idx];}

private:
	void Grow()
	{
		size += growSize;
		T* temp = new T[size];
		memcpy(temp, data, count * sizeof(T));
		delete data;
		data = temp;
	}

	T* data;
	uint count;
	uint size;
	uint growSize;
};

#endif
