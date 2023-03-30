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

void _SymbolRecord::setArrayRecord(string id, int line, string detailedType, int numDimensionsOfArray, vector<pair<int, int>> boundsOfArray)
{
	type = "array";
	this->id = id;
	this->lineNumber = line;
	this->detailedType = detailedType;
	this->numDimensionsOfArray = numDimensionsOfArray;
	this->boundsOfArray = boundsOfArray;
}

void _SymbolRecord::setFuncRecord(string id, int line, string detailedType, int numOfFunc, _SymbolTable *subSymbolTable)
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

void _SymbolTable::insertParaValRecord(string id, int line, string detailedType)
{
	_SymbolRecord* tmpRecord = new _SymbolRecord;
	tmpRecord->setParaValRecord(id,line,detailedType);
	idToLoc[id] = recordList.size();
	recordList.push_back(*tmpRecord);
}

void _SymbolTable::insertParaVarRecord(string id, int line, string detailedType)
{
	_SymbolRecord* tmpRecord = new _SymbolRecord;
	tmpRecord->setParaVarRecord(id,line,detailedType);
	idToLoc[id] = recordList.size();
	recordList.push_back(*tmpRecord);
}

void _SymbolTable::insertVarRecord(string id, int line, string detailedType)
{
	_SymbolRecord* tmpRecord  = new _SymbolRecord;
	tmpRecord->setVarRecord(id, line, detailedType);
	idToLoc[id] = recordList.size();
	recordList.push_back(*tmpRecord);
}


void _SymbolTable::insertConstRecord(string id, int line, string detailedType, string value)
{
	_SymbolRecord *tmpRecord = new _SymbolRecord;
	tmpRecord->setConstRecord(id, line, detailedType, value);
	idToLoc[id] = recordList.size();
	recordList.push_back(*tmpRecord);
}


void _SymbolTable::insertArrayRecord(string id, int line, string detailedType, int numDimensionsOfArray, vector<pair<int, int>> boundsOfArray)
{
	_SymbolRecord *tmpRecord = new _SymbolRecord;
	tmpRecord->setArrayRecord(id, line, detailedType, numDimensionsOfArray, boundsOfArray);
	idToLoc[id] = recordList.size();
	recordList.push_back(*tmpRecord);
}


void _SymbolTable::insertFuncRecord(string id, int line, string detailedType, int numOfFunc, _SymbolTable *subSymbolTable)
{
	_SymbolRecord *tmpRecord = new _SymbolRecord;
	tmpRecord->setFuncRecord(id,line,detailedType,numOfFunc,subSymbolTable);
	idToLoc[id] = recordList.size();
	recordList.push_back(*tmpRecord);
}


void _SymbolTable::insertProcRecord(string id, int line, int numOfFunc, _SymbolTable *subSymbolTable)
{
	_SymbolRecord *tmpRecord = new _SymbolRecord;
	tmpRecord->setProcRecord(id, line, numOfFunc, subSymbolTable);
	idToLoc[id] = recordList.size();
	recordList.push_back(*tmpRecord);
}



void _SymbolTable::insertsParaMainRecord(string id, int line)
{
	_SymbolRecord *tmpRecord = new _SymbolRecord;
	tmpRecord->setParaMainRecord(id, line);
	idToLoc[id] = recordList.size();
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

bool _SymbolTable::convertIdToRecord(string id, _SymbolRecord &record)
{
	map<string,int>::iterator t = idToLoc.find(id);
	if(t != idToLoc.end())//未找到变量
	{
		return false;
	}
	record = recordList[t->second];//将找到的记录传回
	return true;//成功找到对应记录
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
	if(isRecordExistForId(id))//该符号表下存在同名id
	{
		_SymbolRecord tmpRecord;
		convertIdToRecord(id,tmpRecord);
		if(tmpRecord.type == "array" && dimension <= tmpRecord.numDimensionsOfArray)//找到对应数组且存在查询维度
		{
			auto bounds = tmpRecord.boundsOfArray[dimension-1];
			if(bounds.first <= index && index <= bounds.second)	
			{
				return true;
			}
		}
	}
	return false;
}