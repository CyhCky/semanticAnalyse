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

/**
 * @brief 设置传值调用变量信息
 * 
 * @param id 
 * @param line 行号
 * @param detailedType 具体类型
 */
void _SymbolRecord::setParaValRecord(string id,int line,string detailedType)
{
	type = "para_val";
	this->id = id;
	this->lineNumber = line;
	this->detailedType = detailedType;
}

/**
 * @brief 设置引用调用变量信息
 * 
 * @param id 
 * @param line 行号
 * @param detailedType 具体类型
 */
void _SymbolRecord::setParaVarRecord(string id, int line, string detailedType)
{
	type = "para_var";
	this->id = id;
	this->lineNumber = line;
	this->detailedType = detailedType;
}

/**
 * @brief 设置变量信息
 * 
 * @param id 
 * @param line 行号
 * @param detailedType 具体类型
 */
void _SymbolRecord::setVarRecord(string id, int line, string detailedType)
{
	type = "var";
	this->id = id;
	this->lineNumber = line;
	this->detailedType = detailedType;
}

/**
 * @brief 设置常量信息
 * 
 * @param id 
 * @param line 行号
 * @param detailedType 具体类型
 * @param value 常量值，使用string存储
 */
void _SymbolRecord::setConstRecord(string id, int line, string detailedType, string value)
{
	type = "const";
	this->id = id;
	this->lineNumber = line;
	this->detailedType = detailedType;
	this->constValue = value;
}

/**
 * @brief 设置数组记录信息
 * 
 * @param id 
 * @param line 行号
 * @param detailedType 具体类型
 * @param numDimensionsOfArray 数组维数
 * @param boundsOfArray 数组每维上下界
 */
void _SymbolRecord::setArrayRecord(string id, int line, string detailedType, int numDimensionsOfArray, vector<pair<int, int>> boundsOfArray)
{
	type = "array";
	this->id = id;
	this->lineNumber = line;
	this->detailedType = detailedType;
	this->numDimensionsOfArray = numDimensionsOfArray;
	this->boundsOfArray = boundsOfArray;
}

/**
 * @brief 设置函数记录信息
 * 
 * @param id 
 * @param line 行号
 * @param detailedType 具体类型
 * @param numOfFunc 函数输入参数个数
 * @param subSymbolTable 函数的子符号表
 */
void _SymbolRecord::setFuncRecord(string id, int line, string detailedType, int numOfFunc, _SymbolTable *subSymbolTable)
{
	type = "func";
	this->id = id;
	this->lineNumber = line;
	this->detailedType = detailedType;
	this->numOfFunc = numOfFunc;
	this->subSymbolTable = subSymbolTable;
}

/**
 * @brief 设置过程(procedure)记录信息
 * 
 * @param id 
 * @param line 行号
 * @param numOfFunc 具体类型
 * @param subSymbolTable 函数的子符号表
 */
void _SymbolRecord::setProcRecord(string id, int line, int numOfFunc, _SymbolTable *subSymbolTable)
{
	type = "proc";
	this->id = id;
	this->lineNumber = line;
	this->numOfFunc = numOfFunc;
	this->subSymbolTable = subSymbolTable;
}

/**
 * @brief 设置主函数输入参数信息
 * 
 * @param id 
 * @param line 行号
 */
void _SymbolRecord::setParaMainRecord(string id, int line)
{
	type = "para_main";
	this->id = id;
	this->lineNumber = line;
}

/**
 * @brief 插入传值调用变量记录至符号表中
 * 
 * @param id 
 * @param line 行号
 * @param detailedType 具体类型
 */
void _SymbolTable::insertParaValRecord(string id, int line, string detailedType)
{
	_SymbolRecord* tmpRecord = new _SymbolRecord;
	tmpRecord->setParaValRecord(id,line,detailedType);
	idToLoc[id] = recordList.size();
	recordList.push_back(*tmpRecord);
}

/**
 * @brief 插入引用调用参数记录至符号表中
 * 
 * @param id 
 * @param line 行号
 * @param detailedType 具体类型
 */
void _SymbolTable::insertParaVarRecord(string id, int line, string detailedType)
{
	_SymbolRecord* tmpRecord = new _SymbolRecord;
	tmpRecord->setParaVarRecord(id,line,detailedType);
	idToLoc[id] = recordList.size();
	recordList.push_back(*tmpRecord);
}

/**
 * @brief 插入变量信息记录至符号表中
 * 
 * @param id 
 * @param line 行号
 * @param detailedType 具体类型
 */
void _SymbolTable::insertVarRecord(string id, int line, string detailedType)
{
	_SymbolRecord* tmpRecord  = new _SymbolRecord;
	tmpRecord->setVarRecord(id, line, detailedType);
	idToLoc[id] = recordList.size();
	recordList.push_back(*tmpRecord);
}

/**
 * @brief 插入常量信息记录至符号表中
 * 
 * @param id 
 * @param line 行号
 * @param detailedType 具体类型
 * @param value 常量值，使用string类型存储
 */
void _SymbolTable::insertConstRecord(string id, int line, string detailedType, string value)
{
	_SymbolRecord *tmpRecord = new _SymbolRecord;
	tmpRecord->setConstRecord(id, line, detailedType, value);
	idToLoc[id] = recordList.size();
	recordList.push_back(*tmpRecord);
}

/**
 * @brief 插入数组信息记录进入符号表中
 * 
 * @param id 
 * @param line 行号
 * @param detailedType 具体类型
 * @param numDimensionsOfArray 数组维数
 * @param boundsOfArray 数组每维上下界
 */
void _SymbolTable::insertArrayRecord(string id, int line, string detailedType, int numDimensionsOfArray, vector<pair<int, int>> boundsOfArray)
{
	_SymbolRecord *tmpRecord = new _SymbolRecord;
	tmpRecord->setArrayRecord(id, line, detailedType, numDimensionsOfArray, boundsOfArray);
	idToLoc[id] = recordList.size();
	recordList.push_back(*tmpRecord);
}

/**
 * @brief 插入函数信息记录进入符号表中
 * 
 * @param id 
 * @param line 行号
 * @param detailedType 具体类型
 * @param numOfFunc 函数参数个数
 * @param subSymbolTable 函数的子符号表指针 
 */
void _SymbolTable::insertFuncRecord(string id, int line, string detailedType, int numOfFunc, _SymbolTable *subSymbolTable)
{
	_SymbolRecord *tmpRecord = new _SymbolRecord;
	tmpRecord->setFuncRecord(id,line,detailedType,numOfFunc,subSymbolTable);
	idToLoc[id] = recordList.size();
	recordList.push_back(*tmpRecord);
}

/**
 * @brief 插入过程信息记录至符号表中
 * 
 * @param id 
 * @param line 行号
 * @param numOfFunc 过程参数个数
 * @param subSymbolTable 过程的子符号表指针
 */
void _SymbolTable::insertProcRecord(string id, int line, int numOfFunc, _SymbolTable *subSymbolTable)
{
	_SymbolRecord *tmpRecord = new _SymbolRecord;
	tmpRecord->setProcRecord(id, line, numOfFunc, subSymbolTable);
	idToLoc[id] = recordList.size();
	recordList.push_back(*tmpRecord);
}


/**
 * @brief 插入主函数信息记录至符号表中
 * 
 * @param id 
 * @param line 行号
 */
void _SymbolTable::insertsParaMainRecord(string id, int line)
{
	_SymbolRecord *tmpRecord = new _SymbolRecord;
	tmpRecord->setParaMainRecord(id, line);
	idToLoc[id] = recordList.size();
	recordList.push_back(*tmpRecord);
}