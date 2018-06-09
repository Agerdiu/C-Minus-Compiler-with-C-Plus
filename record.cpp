#include "record.h"
unsigned int MyHash(string str)
{
	unsigned int hash = 1315423911;
	unsigned int i = 0;
	for (i = 0; i < str.length(); i++)
	{
		hash ^= ((hash << 5) + str[i] + (hash >> 2));
	}
	cout << hash << endl;
	return hash;
}
