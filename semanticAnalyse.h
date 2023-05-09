/**
 * @file semanticAnalyse.h
 * @author Cyh__Cky (1245532616@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-30
 * 
 * 
 */
#include<cstdio>
#include<cstring>
#include<iostream>
#include<algorithm>
#include"AST.h"
#include"symbolTable.h"
using namespace std;


/*----------以下为语义分析各模块函数--------------*/

/**
 * @brief 语义分析入口处
 * 
 * @param ast_root 
 */
void semanticAnalyse(Program *ast,int ErrLimit);

/**
 * @brief 对程序进行语义分析
 * 
 * @param ast_root 
 */
void analyseProgram(Program *ast_root);


/**
 * @brief 对子程序进行语义分析（函数和过程）
 * 
 * @param ast 
 */
void analyseSubProgram(SubProgram *ast);


/**
 * @brief 对子程序的定义进行语义分析
 * 
 * @param ast 
 */
void analyseSubProgramDef(SubprogramDefinition *ast);


/**
 * @brief 对变量调用进行语义分析
 * 
 * @param ast 
 */
string analyseVariableReference(VariableReference *ast);


/**
 * @brief 对变量定义进行语义分析
 * 
 * @param ast 
 */
void analyseVariable(Variable *ast);


/**
 * @brief 对常量定义进行语义分析
 * 
 * @param ast 
 */
void analyseConsts(Const *ast);


/**
 * @brief 对形式参数进行语义分析
 * 
 * @param ast 
 */
void analyseFormalParameter(FormalParameter *ast);


/**
 * @brief 对语句进行语义分析
 * 
 * @param ast 
 */
void analyseStatement(Statement *ast);

/**
 * @brief 对函数调用进行语义分析
 * 
 * @param ast_root 
 */
string analyseFunctionCall(FunctionCall *ast);

/**
 * @brief 对表达式进行语义分析
 * 
 * @param ast 
 * @return 表达式的具体类型
 */
string analyseExpression(Expression *ast);

/*-------------------------以下为返回值完备检查函数---------------------------------------------------*/


/**
 * @brief 语句是否为返回值完备
 * 
 * @param ast 
 * @return true 
 * @return false 
 */
bool isStatementHasReturn(Statement *ast);


/**
 * @brief 语句块是否为返回值完备
 * 
 * @param ast 
 * @param funcId 
 * @param funcLine 
 */
void isCompoundHasReturn(CompoundS *ast,string funcId,int funcLine);



/**
 * @brief 函数是否为返回完备
 * 
 * @param ast 
 */
void isFuncHasReturn(SubprogramDefinition *ast);


/*----------------------------------------------以下为错误信息处理函数---------------------------------------------*/


/**
 * @brief 将输入字符串直接放入错误信息中
 * 
 * @param s 
 */
void addStrErrorInfo(string s);

/**
 * @brief 重定义错误
 * 
 * @param preId 
 * @param preLine 
 * @param curLine 
 * @param type 
 */
void addMulDefErrInfo(string preId,int preLine,int curLine,string type);


/**
 * @brief 未定义错误
 * 
 * @param id 
 * @param line 
 */
void addUnDefErrInfo(string id,int line);


/**
 * @brief 输入参数个数错误
 * 
 * @param id 
 * @param line 
 * @param nowNum 
 * @param trueNum 
 * @param funcType 取值为"func","proc","array"
 */
void addInputNumErrInfo(string id,int line,int nowNum,int trueNum,string funcType);


/**
 * @brief 具体类型错误(int,real等)
 * 
 * @param id 
 * @param line 
 * @param nowDetailedType 
 * @param trueType 
 */
void addDetailedTypeErrInfo(string id,int line,string nowDetailedType,string trueDetailedType);


/**
 * @brief 类型错误(变量，数组等)
 * 
 * @param id 
 * @param line 
 * @param nowType 
 * @param trueType 
 */
void addTypeErrInfo(string id, int line, string nowType, string trueType);

/**
 * @brief 赋值语句左右值类型不匹配错误
 * 
 * @param left 
 * @param right 
 */
void addAssignDetailedTypeDisMatchErrInfo(VariableReference *left,Expression *right);

/**
 * @brief 数组下标越界错误
 * 
 * @param ast 
 * @param id 
 * @param dimNum 
 */
void addIndexOutOfRangeErrInfo(Expression *ast,string id,int dimNum,pair<int,int>range);


/**
 * @brief 数组定义时上下界错误(下界大于上界)
 * 
 * @param id 
 * @param line 
 * @param dimNum 
 */
void addArrayRangeErrInfo(string id,int line,int dimNum,pair<int,int>range);


/**
 * @brief 操作符的输入参数具体类型出错(例如对real取mod)
 * 
 * @param ast 
 * @param trueDetailedType 
 */
void addSingleOperandExpressionDetailedTypeDisMatchErrInfo(Expression *ast,string trueDetailedType);



/**
 * @brief 运算符两侧操作数类型不一致
 * 
 * @param exp1 
 * @param exp2 
 */
void addOperandExpressionDetailedTypeDisMatchErrInfo(Expression *exp1,Expression *exp2);


/**
 * @brief 除0错误
 * 
 * @param ast 
 * @param opt 
 */
void addDiv0ErrorInfo(Expression *ast, string opt);


/**
 * @brief 表达式返回类型不匹配
 * 
 * @param ast 
 * @param nowDetailedType 
 * @param trueDetailedType 
 * @param info 
 */
void addExpressionDetailedTypeErrInfo(Expression *ast, string nowDetailedType, string trueDetailedType, string info);


void addErrAndCheckLimit(string errInfo);

/*---------以下为工具函数-------------*/


/**
 * @brief string to int
 * 
 * @param s 
 * @return int 
 */
int str2int(string s);



/**
 * @brief int to string
 * 
 * @param a 
 * @return string 
 */
string int2str(int a);
