/**
 * @file symbolTable.h
 * @author Cyh__Cky (1245532616@qq.com)
 * @brief 符号表定义
 * @version 0.13
 * @date 2023-03-28
 */
#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include<cstring>
#include<string>
#include<vector>
#include<map>
using namespace std;
class _SymbolTable;
/**
 * @brief 符号表中的单条记录
 * 
 */
class _SymbolRecord
{
public:
    string type;//"para_val"代表传值参数,"para_var"代表引用参数,"var"代表变量,"const"代表常量
				//"array"代表数组类型,"func"代表函数,"proc"代表过程
				//"para_main"代表主程序的参数
	string id;
	int lineNumber;//表示该条记录对应出现位置的行号
	string detailedType;//取值为"integer","real","boolean","char"等,代表变量/常量/数组被定义的类型 或 函数的返回值类型
	string constValue;//以字符串形式存储常量值
	int numDimensionsOfArray;//用于存储数组类型的维数
	vector < pair <int,int> >boundsOfArray;//用于存储数组类型每个维度的上下界,前者代表下界，后者代表上界
	int numOfFunc;//用于存储函数/过程的输入参数个数
	_SymbolTable* subSymbolTable;//指向存储函数/过程等对应的子符号表
	

	void setParaValRecord(string id,int line,string detailedType);
	void setParaVarRecord(string id,int line,string detailedtype);
	void setVarRecord(string id,int line,string detailedtype);
	void setConstRecord(string id,int line,string detailedtype,string value);
	void setArrayRecord(string id,int line,string detailedtype,int numDimensionsOfArray,vector<pair<int,int>> boundsOfArray);
	void setFuncRecord(string id,int line,string detailedtype,int numOfFunc,_SymbolTable* subSymbolTable);
	void setProcRecord(string id,int line,int numOfFunc,_SymbolTable* subSymbolTable);
	void setVoidRecord(string id,int line);
	_SymbolRecord()
	{
		boundsOfArray.clear();
		subSymbolTable=NULL;
	}
	~_SymbolRecord(){}

};
/**
 * @brief 符号表的类定义
 * 
 */
class _SymbolTable
{
public:
	string tableName;//该符号表名称
	bool isMainTable;//是否为主符号表,是为1,否为0
	vector< _SymbolRecord > recordList;//存储记录的列表
	map<string,int> idToLoc;//使用id快速定位符号表中对应记录存放位置

	void insertParaValRecord(string id,int line,string detailedType);
	void insertParaVarRecord(string id,int line,string detailedtype);
	void insertVarRecord(string id,int line,string detailedtype);
	void insertConstRecord(string id,int line,string detailedtype,string value);
	void insertArrayRecord(string id,int line,string detailedtype,int numDimensionsOfArray,vector<pair<int,int>> boundsOfArray);
	void insertFuncRecord(string id,int line,string detailedtype,int numOfFunc,_SymbolTable* subSymbolTable);
	void insertProcRecord(string id,int line,int numOfFunc,_SymbolTable* subSymbolTable);
	void insertSubSymbolTable(string id,_SymbolTable* subSymbolTable);
	void insertVoidRecord(string id,int line);
	_SymbolTable()
	{
		isMainTable = false;
		recordList.clear();
	}
	~_SymbolTable(){}
};
#endif