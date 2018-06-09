#include "tools.h"
#include <cstdlib>

string inttostr(int n) {
	char buf[10];
	sprintf(buf, "%d", n);
	string str = buf;
	return str;
}

int strtoint(string s) {
	int n;
	n = atoi(s.c_str());
	return n;
}