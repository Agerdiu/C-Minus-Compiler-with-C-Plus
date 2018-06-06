#pragma once
#include <iostream>
#include<string>
#include<vector>
#include "block.h"
unsigned int JSHash(string str)
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
class VarHashMap
{
private:
	int tablesize;
	varNode **table;
public:
	VarHashMap(int tablesize)
	{
		this->tablesize = tablesize;
		table = new varNode*[tablesize];
		for (int i = 0; i < tablesize; i++)
		{
			table[i] = NULL;
		}
	}

	bool find(string varname)
	{
		
		int hash = (JSHash(varname) % this->tablesize);
		while (table[hash] != NULL && table[hash]->name != varname)
		{
			hash = (hash + 1) % this->tablesize;
		}
		if (table[hash] == NULL) 	return false;
		else  return true;
	}

	varNode get(string varname)
	{
		int hash = (JSHash(varname) % this->tablesize);
		while (table[hash] != NULL && table[hash]->name != varname)
		{
			hash = (hash + 1) % this->tablesize;
		}
		return *table[hash];
	}
	void insert(varNode var)
	{
		int hash = (JSHash(var.name) % this->tablesize);
		while (table[hash] != NULL && table[hash]->name != var.name)
		{
			hash = (hash + 1) % this->tablesize;
		}
		if (table[hash] != NULL)
		{
			delete table[hash];
		}
		cout << "Ready!" << endl;
		table[hash] = new varNode;
		*table[hash] = var;
		return;
	}
	~VarHashMap()
	{
		for (int i = 0; i < this->tablesize; i++)
		{
			if (table[i] != NULL)
			{
				delete table[i];
			}
		}
		delete[] table;
	}
};