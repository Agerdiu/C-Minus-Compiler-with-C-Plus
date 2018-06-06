#include "hash.h"
using namespace std;
int main(int argc, char * argv[])
{
	JSHash("abc");
	JSHash("test");
	int size = 128;
	VarHashMap hash1(128);
	varNode newvar;
	newvar.name = "test";
	newvar.type = "int";
	newvar.num = 1;
	if (hash1.find("num") == false)
		cout << "False" << endl;
	hash1.insert(newvar);
	return 0;
}