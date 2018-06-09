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
		for (vector<string>::size_type i = 0; i != toks.size(); i++)
		{
			int Varflag = toks[i].find("temp");
			if (!Varflag)
				variables.insert(toks[i]);
		}
	}
	ShowVars();
	return;
}
string RAlloc(string var)
{
	if (variables.count(var) != 0)
	{
		variables.erase(var);
	}		
	if (V_RTable.count(var) != 0) return "$" + V_RTable.find(var)->second;
	else
	{
		set <string> VarwithR;
		map<string , string>::iterator iter = V_RTable.begin();
		for (iter = V_RTable.begin(); iter != V_RTable.end(); iter++)
			    VarwithR.insert(iter->first);
		set <string> ::iterator iter2 = VarwithR.begin();
		//清理过去的临时变量
		for (iter2 = VarwithR.begin(); iter2 != VarwithR.end(); iter2++)
		{
			string varname = *iter2;
			set <string> TTBE;
			if (!varname.find("temp") && variables.count(varname) == 0)
			{
				for (iter = V_RTable.begin(); iter != V_RTable.end(); iter++)
				{
					if (iter->first.compare(varname) == 0)
					{
						SetRflag(varname, true);
						TTBE.insert(varname);
					}
				}
				set <string> ::iterator iter3 = TTBE.begin();
				for (iter3 = TTBE.begin(); iter3 !=TTBE.end(); iter3++)
					V_RTable.erase(*iter3);
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
void ShowRegsSituations()
{
	for (int i = 0; i <= 17; i++)
	{
		if (Rflag[i] == true)		cout << RName[i] + " True" << endl;
		else   cout << RName[i] + " False" << endl;
	}
	map<string, string>::iterator iter = V_RTable.begin();
	for (iter = V_RTable.begin(); iter != V_RTable.end(); iter++)
		cout << iter->first << iter->second << endl;
	return;
}

string TransByLines(string lines)
{
	vector<string> toks;
	SplitString(lines, toks, " "); //单词拆分
	if (toks[0].compare("LABEL")== 0)  cout << toks[1] << ":" << endl;
	if (toks[0].compare("GOTO") == 0)
		return "j " + toks[1];
	if (toks[0].compare("RETURN") == 0)
		return "move $v0," + RAlloc(toks[1]) + "\njr $ra";
	if (toks[0].compare("FUNCTION") == 0)
		return "" + toks[1] + ":";
	if (toks[0].compare("CALL") == 0)
	{
		if (toks[toks.size() - 1].compare("read") == 0 || toks[toks.size() - 1].compare("print") == 0)
			return "addi $sp,$sp,-4\nsw $ra,0($sp)\njal " + toks[toks.size() - 1] + "\nlw $ra,0($sp)\nmove " + RAlloc(toks[0]) + ",$v0\naddi $sp,$sp,4";
		else
			return "addi $sp,$sp,-24\nsw $t0,0($sp)\nsw $ra,4($sp)\nsw $t1,8($sp)\nsw $t2,12($sp)\nsw $t3,16($sp)\nsw $t4,20($sp)\njal " + toks[toks.size() - 1] + "\nlw $a0,0($sp)\nlw $ra,4($sp)\nlw $t1,8($sp)\nlw $t2,12($sp)\nlw $t3,16($sp)\nlw $t4,20($sp)\naddi $sp,$sp,24\nmove " + RAlloc(toks[0]) + " $v0";
	}
	if (toks[0].compare("ARG") == 0)
		return "move $t0,$a0\nmove $a0," + RAlloc(toks[toks.size() - 1]);
	if (toks[0].compare("PARAM") == 0)
	{
		cout << "PARAM" << endl;
		map<string, string>::iterator iter = V_RTable.begin();
		for (iter = V_RTable.begin(); iter != V_RTable.end(); iter++)
			if (iter->first.compare(toks[toks.size() - 1]) == 0)
				iter->second = "a0";
		return " ";
	}
	if (toks[1].compare(":=") == 0)
	{
		if (toks.size() == 3)
			if (toks[toks.size() - 1][0] == '#')
				return "li "+RAlloc(toks[0])+","+ MyReplace(toks[toks.size() - 1],'#',' ');
			else
				return "move " + RAlloc(toks[0]) + "," + RAlloc(toks[2]);
		if(toks.size() == 5)
			if (toks[3].compare("+") == 0)
				if (toks[toks.size() - 1][0] == '#')
					 return "addi " + RAlloc(toks[0]) + "," + RAlloc(toks[2]) + "," + MyReplace(toks[toks.size() - 1], '#', ' ');
				 else 
					 return "add " + RAlloc(toks[0]) + "," + RAlloc(toks[2]) + "," + RAlloc(toks[toks.size() - 1]);
			else if(toks[3].compare("-") == 0)
				if (toks[toks.size() - 1][0] == '#')
					return "addi " + RAlloc(toks[0]) + "," + RAlloc(toks[2]) + ",-" + MyReplace(toks[toks.size() - 1], '#', ' ');
				else
					return "sub " + RAlloc(toks[0]) + "," + RAlloc(toks[2]) + "," + RAlloc(toks[toks.size() - 1]);
			else if (toks[3].compare("*") == 0)
					return "mul " + RAlloc(toks[0]) + "," + RAlloc(toks[2]) + "," + RAlloc(toks[toks.size() - 1]);
			else if (toks[3].compare("/") == 0)
				    return "div " + RAlloc(toks[2]) + "," + RAlloc(toks[toks.size() - 1]) + "\n" +"mflo" + RAlloc(toks[0]);
			else if (toks[3].compare("<") == 0)
				return "slt " + RAlloc(toks[0]) + "," + RAlloc(toks[2]) + "," + RAlloc(toks[toks.size() - 1]);
			else if (toks[3].compare(">") == 0)
				return "slt " + RAlloc(toks[0]) + "," + RAlloc(toks[toks.size() - 1]) + "," + RAlloc(toks[2]);
		if (toks[2].compare("CALL") == 0)
		{
			if (toks[3].compare("read") == 0 || toks[3].compare("print") == 0)
				return "addi $sp,$sp,-4\nsw $ra,0($sp)\njal " + toks[toks.size() - 1] + "\nlw $ra,0($sp)\nmove " + RAlloc(toks[0]) + ",$v0\naddi $sp,$sp,4";
			else
				return "addi $sp,$sp,-24\nsw $t0,0($sp)\nsw $ra,4($sp)\nsw $t1,8($sp)\nsw $t2,12($sp)\nsw $t3,16($sp)\nsw $t4,20($sp)\njal " + toks[toks.size() - 1] + "\nlw $a0,0($sp)\nlw $ra,4($sp)\nlw $t1,8($sp)\nlw $t2,12($sp)\nlw $t3,16($sp)\nlw $t4,20($sp)\naddi $sp,$sp,24\nmove " + RAlloc(toks[0]) + " $v0";
		}
	}
	if (toks[0].compare("IF") == 0)
		if (toks[2].compare("==") == 0)
			return "beq " + RAlloc(toks[1]) + "," + RAlloc(toks[3]) + "," + toks[toks.size() - 1];
        else if (toks[2].compare("!=") == 0)
            return "bne " + RAlloc(toks[1]) + "," + RAlloc(toks[3]) + "," + toks[toks.size() - 1];
        else if (toks[2].compare(">") == 0)
            return "bgt " + RAlloc(toks[1]) + "," + RAlloc(toks[3]) + "," + toks[toks.size() - 1];
        else if (toks[2].compare("<") == 0)
            return "blt " + RAlloc(toks[1]) + "," + RAlloc(toks[3]) + "," + toks[toks.size() - 1];
        else if (toks[2].compare(">=") == 0)
            return "bge " + RAlloc(toks[1]) + "," + RAlloc(toks[3]) + "," + toks[toks.size() - 1];
        else if (toks[2].compare("<=") == 0)
            return "ble " + RAlloc(toks[1]) + "," + RAlloc(toks[3]) + "," + toks[toks.size() - 1];
	return " ";
}
string MyReplace(string model, char a, char b)
{
	for (int i = 0; i < model.size(); i++)
		if (model[i] == a) model[i] = b;
	return model;
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
	map<string, string>::iterator iter = V_RTable.begin();
	for (iter = V_RTable.begin(); iter != V_RTable.end(); iter++)
		if (iter->first.compare(RegName) == 0)
		{
			RegName = iter->second;
			break;
		}
	for (int i = 0; i <= 17; i++)
	{
		if (RName[i].compare(RegName)==0)
			Rflag[i] = value;
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
	ifstream IR("IR.txt");
	if (!IR) {
		cout << "No such IR file!" << endl;
	}
	string  lines;
	while (getline(IR, lines))
	{
		cout << "IR:  "<<lines << endl;
		cout << "ASM:  " << TransByLines(lines) << endl;
	}
	return 0;
}

