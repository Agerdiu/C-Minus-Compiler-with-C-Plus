#pragma once
#include<cstdlib>
#include<string>
#include<set>
#include<vector>
#include<iostream>
#include<fstream>
#include<map>
using namespace std;
bool Rflag[18] = { true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true };
string RName[18] = { "t0","t1","t2","t3","t4","t5","t6","t7","t8","t9","s0","s1","s2","s3","s4","s5","s6","s7" };
set <string> variables;
map <string, string> V_RTable;
void FindVars(string filename);
string TransByLines(string lines);
void SplitString(const string& s, vector<string>& v, const string& c);
void ShowVars();
string RAlloc(string var);
void SetRflag(string RegName, bool value);
void ShowRegsSituations();
string MyReplace(string model, char a, char b);
bool GetRflag(string RegName);