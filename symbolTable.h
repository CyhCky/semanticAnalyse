/**
 * @file symbolTable.h
 * @author Cyh__Cky (1245532616@qq.com)
 * @brief 符号表定义
 * @version 0.3
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
	
	/**
	 * @brief 设置传值调用变量信息
	 * 
	 * @param id 
	 * @param line 行号
	 * @param detailedType 具体类型
	 */
	void setParaValRecord(string id,int line,string detailedType);

	/**
	 * @brief 设置引用调用变量信息
	 * 
	 * @param id 
	 * @param line 行号
	 * @param detailedType 具体类型
	 */
	void setParaVarRecord(string id,int line,string detailedType);

	/**
	 * @brief 设置变量信息
	 * 
	 * @param id 
	 * @param line 行号
	 * @param detailedType 具体类型
	 */
	void setVarRecord(string id,int line,string detailedType);


	/**
	 * @brief 设置常量信息
	 * 
	 * @param id 
	 * @param line 行号
	 * @param detailedType 具体类型
	 * @param value 常量值，使用string存储
	 */
	void setConstRecord(string id,int line,string detailedType,string value);

	/**
	 * @brief 设置数组记录信息
	 * 
	 * @param id 
	 * @param line 行号
	 * @param detailedType 具体类型
	 * @param numDimensionsOfArray 数组维数
	 * @param boundsOfArray 数组每维上下界
	 */
	void setArrayRecord(string id,int line,string detailedType,int numDimensionsOfArray,vector<pair<int,int>> boundsOfArray);

	/**
	 * @brief 设置函数记录信息
	 * 
	 * @param id 
	 * @param line 行号
	 * @param detailedType 具体类型
	 * @param numOfFunc 函数输入参数个数
	 * @param subSymbolTable 函数的子符号表
	 */
	void setFuncRecord(string id,int line,string detailedType,int numOfFunc,_SymbolTable* subSymbolTable);

	/**
	 * @brief 设置过程(procedure)记录信息
	 * 
	 * @param id 
	 * @param line 行号
	 * @param numOfFunc 具体类型
	 * @param subSymbolTable 函数的子符号表
	 */
	void setProcRecord(string id,int line,int numOfFunc,_SymbolTable* subSymbolTable);

	/**
	 * @brief 设置主函数输入参数信息
	 * 
	 * @param id 
	 * @param line 行号
	 */
	void setParaMainRecord(string id,int line);

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
	_SymbolTable* fatherSymbolTable;//指向父符号表的指针
	bool isMainTable;//是否为主符号表,是为1,否为0
	vector< _SymbolRecord > recordList;//存储记录的列表
	map<string,int> idToLoc;//使用id快速定位符号表中对应记录存放位置

	/**
	 * @brief 插入传值调用变量记录至符号表中
	 * 
	 * @param id 
	 * @param line 行号
	 * @param detailedType 具体类型
	 */
	void insertParaValRecord(string id,int line,string detailedType);


	/**
	 * @brief 插入引用调用参数记录至符号表中
	 * 
	 * @param id 
	 * @param line 行号
	 * @param detailedType 具体类型
	 */
	void insertParaVarRecord(string id,int line,string detailedType);

	/**
	 * @brief 插入变量信息记录至符号表中
	 * 
	 * @param id 
	 * @param line 行号
	 * @param detailedType 具体类型
	 */
	void insertVarRecord(string id,int line,string detailedType);

	/**
	 * @brief 插入常量信息记录至符号表中
	 * 
	 * @param id 
	 * @param line 行号
	 * @param detailedType 具体类型
	 * @param value 常量值，使用string类型存储
	 */
	void insertConstRecord(string id,int line,string detailedType,string value);

	/**
	 * @brief 插入数组信息记录进入符号表中
	 * 
	 * @param id 
	 * @param line 行号
	 * @param detailedType 具体类型
	 * @param numDimensionsOfArray 数组维数
	 * @param boundsOfArray 数组每维上下界
	 */
	void insertArrayRecord(string id,int line,string detailedType,int numDimensionsOfArray,vector<pair<int,int>> boundsOfArray);
	
	/**
	 * @brief 插入函数信息记录进入符号表中
	 * 
	 * @param id 
	 * @param line 行号
	 * @param detailedType 具体类型
	 * @param numOfFunc 函数参数个数
	 * @param subSymbolTable 函数的子符号表指针 
	 */
	void insertFuncRecord(string id,int line,string detailedType,int numOfFunc,_SymbolTable* subSymbolTable);

	/**
	 * @brief 插入过程信息记录至符号表中
	 * 
	 * @param id 
	 * @param line 行号
	 * @param numOfFunc 过程参数个数
	 * @param subSymbolTable 过程的子符号表指针
	 */
	void insertProcRecord(string id,int line,int numOfFunc,_SymbolTable* subSymbolTable);

	/**
	 * @brief 插入主函数信息记录至符号表中
	 * 
	 * @param id 
	 * @param line 行号
	 */
	void insertsParaMainRecord(string id,int line);
	_SymbolTable()
	{
		isMainTable = false;
		recordList.clear();
	}
	_SymbolTable(string tableName,_SymbolTable* fatherSymbolTable);
	~_SymbolTable(){}

	/**
	 * @brief 使用id判断对应记录是否存在
	 * 
	 * @param id 
	 * @return true 
	 * @return false 
	 */
	bool isRecordExistForId(string id);

	/**
	 * @brief 使用id得到对应的记录
	 * 
	 * @param id 
	 * @param record 记录返回位置，引用调用
	 * @return true 成功得到对应记录
	 * @return false 未成功找到对应记录
	 */
	bool convertIdToRecord(string id,_SymbolRecord &record);

	/**
	 * @brief 判断一个id在该符号表中是否合法,合法返回true,非法返回false
	 * 
	 * @param id 
	 * @return true 
	 * @return false 
	 */
	bool isVaildid(string id);

	/**
	 * @brief 判断数组下标是否合法
	 * 
	 * @param id 数组id
	 * @param index 下标
	 * @return true 
	 * @return false 
	 */
	bool isIndexInRange(string id,int dimension,int index);

    //查找并返回指定id的子函数或者子过程的所有形参大致类型类型，即para_val或para_var，返回类型为字符串列表
	vector<string> findAllFormalParaRoughType(string id);
    //查找并返回指定id的子函数或者子过程的所有形参具体类型，即取值为"integer","real","boolean","char"等，返回类型为字符串列表
	vector<string> findAllFormalParaDetailedType(string id);
};
#endif