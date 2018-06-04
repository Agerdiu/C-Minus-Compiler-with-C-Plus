#include<cstdlib>
#include "IRtoASM.h"
void FindVars(string filename)
{
	ifstream IR(filename);
	if (!IR) {
		cout << "No such IR file!" << endl;
	}
	string  lines;
	while (getline(IR, lines))
	{
		vector<string> toks;
		SplitString(lines, toks, " "); //单词拆分
		cout << "Read from file: " << lines << endl;
		for (vector<string>::size_type i = 0; i != toks.size(); i++)
		{
			int Varflag = toks[i].find("temp");
			if (!Varflag)
				variables.insert(toks[i]);
		}
	}
	V_RTable.insert(pair<string, string>("temp2", "t0"));
	RAlloc("temp2");
	ShowVars();
	return;
}
string RAlloc(string var)
{
	if (variables.find(var) != variables.end()) variables.erase(variables.find(var));
	if (V_RTable.count(var) != 0) return "$" + V_RTable.find(var)->second;
	else
	{
		set <string> VarwithR;
		map<string , string>::iterator iter = V_RTable.begin();
		for (iter = V_RTable.begin(); iter != V_RTable.end(); iter++)
			    VarwithR.insert(iter->first);
		set <string> ::iterator iter2 = VarwithR.begin();
		for (iter2 = VarwithR.begin(); iter2 != VarwithR.end(); iter2++)
		{
			string varname = *iter2;
			cout << varname << endl;
			if(varname.find("temp") && variables.count(*iter2)==0)
				for (iter = V_RTable.begin(); iter != V_RTable.end(); iter++)
				{
					if (iter->first.compare(varname) == 0)
					{
						SetRflag(*iter2, true);
						V_RTable.erase(*iter2);
					}
				}
		}
		for (int i = 0; i <= 17; i++)
		{
			if (Rflag[i] == true)
			{
				V_RTable.insert(pair<string, string>(var, RName[i]));
				Rflag[i] = false;
				return "$" + RName[i];
			}
		}
	}
	return "Error!";
}
void TransByLines(string lines)
{
	vector<string> toks;
	SplitString(lines, toks, " "); //单词拆分
	if (toks[0].compare("LABEL")== 0)  cout << toks[1] << ':' << endl;
}
void ShowVars()
{
	set<string>::iterator iter = variables.begin();

	while (iter != variables.end())
	{
		cout << *iter << endl;
		++iter;
	}
	return;

}
void SetRflag(string RegName, bool value)
{
	for (int i = 0; i <= 17; i++)
	{
		if (RName[i] == RegName)
			Rflag[i] == value;
	}
	return;
}
bool GetRflag(string RegName)
{
	for (int i = 0; i <= 17; i++)
	{
		if (RName[i] == RegName)
			return Rflag[i];
	}
}
void SplitString(const string& s, vector<string>& v, const string& c)
{
	string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));
		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}
int main()
{
	FindVars("IR.txt");
	return 0;
}

