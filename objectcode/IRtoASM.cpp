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
	ShowVars();
	return;
}
string RAlloc(string var)
{
	if (variables.count(var) != 0)
	{
		cout << var << "Show up repeatedly!" << endl;
		variables.erase(var);
	}		
	if (V_RTable.count(var) != 0) return "$" + V_RTable.find(var)->second;
	else
	{
		cout << "stage1!" << endl;
		set <string> VarwithR;
		map<string , string>::iterator iter = V_RTable.begin();
		for (iter = V_RTable.begin(); iter != V_RTable.end(); iter++)
			    VarwithR.insert(iter->first);
		set <string> ::iterator iter2 = VarwithR.begin();
		cout << "stage2!" << endl; //清理过去的临时变量
		for (iter2 = VarwithR.begin(); iter2 != VarwithR.end(); iter2++)
		{
			string varname = *iter2;
			cout << varname << endl;
			set <string> TTBE;
			if (!varname.find("temp") && variables.count(varname) == 0)
			{
				for (iter = V_RTable.begin(); iter != V_RTable.end(); iter++)
				{
					cout << iter->first << endl;
					if (iter->first.compare(varname) == 0)
					{
						SetRflag(varname, true);
						cout << varname << "Reset!" << endl;
						TTBE.insert(varname);
					}
				}
				set <string> ::iterator iter3 = TTBE.begin();
				for (iter3 = TTBE.begin(); iter3 !=TTBE.end(); iter3++)
					V_RTable.erase(*iter3);
			}		
		}
		cout << "stage3!" << endl;
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

void TransByLines(string lines)
{
	vector<string> toks;
	SplitString(lines, toks, " "); //单词拆分
	if (toks[0].compare("LABEL")== 0)  cout << toks[1] << ':' << endl;
	if (toks[1].compare(":=") == 0)
	{
		if (toks.size() == 3)
			if (toks[toks.size() - 1][0] == '#')
				return ;
	}
	/*
	if line[1]==':=':
        if len(line)==3:
            if line[-1][0]=='#':
                return '\tli %s,%s'%(Get_R(line[0]),line[-1].replace('#',''))
            else:
                return '\tmove %s,%s'%(Get_R(line[0]),Get_R(line[2]))
        if len(line)==5:
            if line[3]=='+':
                if line[-1][0]=='#':
                    return '\taddi %s,%s,%s'%(Get_R(line[0]),Get_R(line[2]),line[-1].replace('#',''))
                else:
                    return '\tadd %s,%s,%s'%(Get_R(line[0]),Get_R(line[2]),Get_R(line[-1]))
            elif line[3]=='-':
                if line[-1][0]=='#':
                    return '\taddi %s,%s,-%s'%(Get_R(line[0]),Get_R(line[2]),line[-1].replace('#',''))
                else:
                    return '\tsub %s,%s,%s'%(Get_R(line[0]),Get_R(line[2]),Get_R(line[-1]))
            elif line[3]=='*':
                return '\tmul %s,%s,%s'%(Get_R(line[0]),Get_R(line[2]),Get_R(line[-1]))
            elif line[3]=='/':
                return '\tdiv %s,%s\n\tmflo %s'%(Get_R(line[2]),Get_R(line[-1]),Get_R(line[0]))
            elif line[3]=='<':
                return '\tslt %s,%s,%s'%(Get_R(line[0]),Get_R(line[2]),Get_R(line[-1]))
            elif line[3]=='>':
                return '\tslt %s,%s,%s'%(Get_R(line[0]),Get_R(line[-1]),Get_R(line[2]))    
        if line[2]=='CALL':
            if line[3]=='read' or line[3]=='print':
                return '\taddi $sp,$sp,-4\n\tsw $ra,0($sp)\n\tjal %s\n\tlw $ra,0($sp)\n\tmove %s,$v0\n\taddi $sp,$sp,4'%(line[-1],Get_R(line[0]))
            else:
                return '\taddi $sp,$sp,-24\n\tsw $t0,0($sp)\n\tsw $ra,4($sp)\n\tsw $t1,8($sp)\n\tsw $t2,12($sp)\n\tsw $t3,16($sp)\n\tsw $t4,20($sp)\n\tjal %s\n\tlw $a0,0($sp)\n\tlw $ra,4($sp)\n\tlw $t1,8($sp)\n\tlw $t2,12($sp)\n\tlw $t3,16($sp)\n\tlw $t4,20($sp)\n\taddi $sp,$sp,24\n\tmove %s $v0'%(line[-1],Get_R(line[0]))
    if line[0]=='GOTO':
        return '\tj %s'%line[1]
    if line[0]=='RETURN':
            return '\tmove $v0,%s\n\tjr $ra'%Get_R(line[1])
    if line[0]=='IF':
        if line[2]=='==':
            return '\tbeq %s,%s,%s'%(Get_R(line[1]),Get_R(line[3]),line[-1])
        if line[2]=='!=':
            return '\tbne %s,%s,%s'%(Get_R(line[1]),Get_R(line[3]),line[-1])
        if line[2]=='>':
            return '\tbgt %s,%s,%s'%(Get_R(line[1]),Get_R(line[3]),line[-1])
        if line[2]=='<':
            return '\tblt %s,%s,%s'%(Get_R(line[1]),Get_R(line[3]),line[-1])
        if line[2]=='>=':
            return '\tbge %s,%s,%s'%(Get_R(line[1]),Get_R(line[3]),line[-1])
        if line[2]=='<=':
            return '\tble %s,%s,%s'%(Get_R(line[1]),Get_R(line[3]),line[-1])
    if line[0]=='FUNCTION':
        return '%s:'%line[1]
    if line[0]=='CALL':
        if line[-1]=='read' or line[-1]=='print':
            return '\taddi $sp,$sp,-4\n\tsw $ra,0($sp)\n\tjal %s\n\tlw $ra,0($sp)\n\taddi $sp,$sp,4'%(line[-1])
        else:
            return '\taddi $sp,$sp,-24\n\tsw $t0,0($sp)\n\tsw $ra,4($sp)\n\tsw $t1,8($sp)\n\tsw $t2,12($sp)\n\tsw $t3,16($sp)\n\tsw $t4,20($sp)\n\tjal %s\n\tlw $a0,0($sp)\n\tlw $ra,4($sp)\n\tlw $t1,8($sp)\n\tlw $t2,12($sp)\n\tlw $t3,16($sp)\n\tlw $t4,20($sp)\n\taddi $sp,$sp,24\n\tmove %s $v0'%(line[-1],Get_R(line[0]))
    if line[0]=='ARG':
        return '\tmove $t0,$a0\n\tmove $a0,%s'%Get_R(line[-1])
    if line[0]=='PARAM':
        table[line[-1]]='a0'
		*/
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
	variables.insert("temp1");
	cout << RAlloc("temp1") << endl;
	cout << RAlloc("a") << endl;
	ShowVars();
	ShowRegsSituations();
	return 0;
}

