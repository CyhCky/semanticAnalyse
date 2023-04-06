/**
 * @file symbolTable.cpp
 * @author Cyh__Cky (1245532616@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-29
 * 
 * 
 */
#include"symbolTable.h"
using namespace std;

void _SymbolRecord::setParaValRecord(string id,int line,string detailedType)
{
	type = "para_val";
	this->id = id;
	this->lineNumber = line;
	this->detailedType = detailedType;
}


void _SymbolRecord::setParaVarRecord(string id, int line, string detailedType)
{
	type = "para_var";
	this->id = id;
	this->lineNumber = line;
	this->detailedType = detailedType;
}

void _SymbolRecord::setVarRecord(string id, int line, string detailedType)
{
	type = "var";
	this->id = id;
	this->lineNumber = line;
	this->detailedType = detailedType;
}

void _SymbolRecord::setConstRecord(string id, int line, string detailedType, string value)
{
	type = "const";
	this->id = id;
	this->lineNumber = line;
	this->detailedType = detailedType;
	this->constValue = value;
}

void _SymbolRecord::setArrayRecord(string id, int line, string detailedType, int numDimensionsOfArray,\
									vector<pair<int, int>> boundsOfArray)
{
	type = "array";
	this->id = id;
	this->lineNumber = line;
	this->detailedType = detailedType;
	this->numDimensionsOfArray = numDimensionsOfArray;
	this->boundsOfArray = boundsOfArray;
}

void _SymbolRecord::setFuncRecord(string id, int line, string detailedType,\
									 int numOfFunc, _SymbolTable *subSymbolTable)
{
	type = "func";
	this->id = id;
	this->lineNumber = line;
	this->detailedType = detailedType;
	this->numOfFunc = numOfFunc;
	this->subSymbolTable = subSymbolTable;
}

void _SymbolRecord::setProcRecord(string id, int line, int numOfFunc, _SymbolTable *subSymbolTable)
{
	type = "proc";
	this->id = id;
	this->lineNumber = line;
	this->numOfFunc = numOfFunc;
	this->subSymbolTable = subSymbolTable;
}

void _SymbolRecord::setParaMainRecord(string id, int line)
{
	type = "para_main";
	this->id = id;
	this->lineNumber = line;
}

void _SymbolTable::insertRecord(_SymbolRecord* tmpRecord)
{
	idToLoc[tmpRecord->id] = recordList.size();
	recordList.push_back(*tmpRecord);
}

_SymbolTable::_SymbolTable(string tableName,_SymbolTable* fatherSymbolTable)
{
	this->tableName = tableName;
	this->fatherSymbolTable = fatherSymbolTable;
}

bool _SymbolTable::isRecordExistForId(string id)
{
	if(idToLoc.find(id) == idToLoc.end()) 
		return false;
	return true;
}

int _SymbolTable::convertIdToRecord(string id, _SymbolRecord &record)
{
	map<string,int>::iterator t = idToLoc.find(id);
	if(t != idToLoc.end())//未找到变量
	{
		if(isMainTable == true) 
			return false;//若已经是主符号表，则找不到对应记录
		t = fatherSymbolTable->idToLoc.find(id);//查找父符号表
		if(t != fatherSymbolTable->idToLoc.end())//父符号表中也无对应记录
			return false;
		record = fatherSymbolTable->recordList[t->second];
			return 2;//成功在父符号表中找到对应记录
	}
	record = recordList[t->second];//将找到的记录传回
	return 1;//成功找到对应记录
}

bool _SymbolTable::isVaildid(string id)
{
	if(id == tableName)//与符号表名称重名
	{
		return false;
	}
	if(isRecordExistForId(id))//在该层符号表中查到重名变量
	{
		return false;
	}
	else if(isMainTable)//该表为主符号表
	{
		return true;
	}
	else //该表为子符号表，需要查询主符号表
	{
		if(id == fatherSymbolTable->tableName)//与主符号表名重名
		{
			return false;
		}
		if(!fatherSymbolTable->isRecordExistForId(id))//未在主符号表中找到任意重名变量
		{
			return true;
		}
		else 
		{
			_SymbolRecord tmpRecord;
			fatherSymbolTable->convertIdToRecord(id,tmpRecord);
			string recordType = tmpRecord.type;
			if(recordType == "var"||recordType == "const"||recordType =="array")//判断重名对象是否是常量或变量或数组名称
			{
				return true;
			}
			else return false;
		}
	}
}

bool _SymbolTable::isIndexInRange(string id, int dimension, int index)
{
	_SymbolRecord tmpRecord;
	int t = convertIdToRecord(id,tmpRecord);
	if(t == 0)
	{
		return false;
	}
	if(tmpRecord.type == "array" && dimension <= tmpRecord.numDimensionsOfArray)//找到对应数组且存在查询维度
	{
		auto bounds = tmpRecord.boundsOfArray[dimension-1];
		if(bounds.first <= index && index <= bounds.second)	
		{
			return true;
		}
	}
	return false;
}

vector<pair<string,string>> _SymbolTable::findAllFormalParaType(string id)
{
	vector<pair<string,string>> q;
	_SymbolRecord tmpRecord;
	int t = convertIdToRecord(id,tmpRecord);
	if(t == 0)//未找到记录
	{
		string err = "err", noRecord = "noRecord";
		q.push_back(make_pair(err,noRecord));
		return q;
	}
	if(tmpRecord.type!="func"&&tmpRecord.type!="proc")//记录不为函数/过程类型
	{
		string err = "err", wrongType = "wrongType";
		q.push_back(make_pair(err,wrongType));
		return q;
	}
	_SymbolTable* nowSymbolTable = tmpRecord.subSymbolTable;
	for(int i = 0;i < tmpRecord.numOfFunc;i++)
	{
		q.push_back(make_pair(nowSymbolTable->recordList[i].type,\
								  nowSymbolTable->recordList[i].detailedType));
	}
	return q;
}
