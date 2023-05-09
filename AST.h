/*AST节点类定义*/

#ifndef AST_H
#define AST_H

#include <vector>
#include <string>
using namespace std;

class Program;
class SubProgram;
class Variable;
class Const;
class CompoundS;
class SubprogramDefinition;
class Procedure;
class FormalParameter;
class Type;
class AssignS;
class IfS;
class ForS;
class WhileS;
class RepeatS;
class Statement;
class VariableReference;
class FunctionCall;
class Expression;
class ReadS;
class WriteS;

class Statement
{
	public:
		string type;//assign、if、for...
		string statementType;//void 或者 error
		int lineNum;//行号
		bool isReturn;//是否是返回值语句

		Statement(){}
		~Statement(){}
};

class Program
{
	public:
		pair<string, int> programId;//程序标识符和行号
		vector<pair<string, int>> parameterList;//参数列表及行号
		SubProgram * subProgram;//子程序

		Program();
		~Program();
};

class SubProgram
{
	public:
		vector<Const*> constList;//常数定义列表
		vector<Variable*> variableList;//变量定义列表
		vector<SubprogramDefinition*> subprogramDefinitionList;//子程序和子函数定义列表
		CompoundS* compound;//主程序体

		SubProgram();
		~SubProgram();
};

class Variable
{
	public:
		pair<string, int> variableId;//变量标识符及行号
		Type * type;//变量类型

		Variable();
		Variable(pair<string, int>, Type*);
		~Variable();
};

class Const
{
	public:
		pair<string, int> constId;
		string type;//常数类型
		pair<string, int> valueId;
		char charValue;
		int intValue;
		float realValue;
		string strOfVal;
		bool isMinusShow;

		Const(){}
		~Const(){}
};

class CompoundS:public Statement
{
	public:
		vector<Statement*> statementList;//语句列表 行号由begin的位置决定
		
		CompoundS();
		~CompoundS();
};

class SubprogramDefinition
{
public:
	pair<string, int> funtionID;//过程标识符
	vector<FormalParameter*> formalParameterList;//形式参数列表
	pair<string, int> type;//type.first若是空串，则为过程，否则为函数，取值为integer,real,boolean等等
	vector<Const*> constList;//常数定义列表
	vector<Variable*> variableList;//变量定义列表
	CompoundS* compound;

	SubprogramDefinition();
	~SubprogramDefinition();
};

class ProcedureCall:public Statement
{
	public:
		pair<string, int> procedureId;//过程标识符
		vector<Expression*> actualParameterList;//实际参数列表，由表达式组成

		ProcedureCall();
		~ProcedureCall();
};

class FormalParameter
{
	public:
		pair<string, int> paraId;//形参标识符和行号
		string type;//形参类型一定为基本类型，改成string即可
		int flag;//flag为0是传值调用，为1表示引用调用

		FormalParameter();
		~FormalParameter(){}
		FormalParameter(pair<string, int> theParaId, string theType, int theFlag);
};

class Type
{
	public:
		pair<string, int> type;//基本类型及行号
		int isArray;//0表示非数组
		vector<pair<int, int>> arrayRangeList;//数组上下界

		Type();
		~Type() {}
		Type(pair<string, int> , int , vector< pair<int, int>> arrayRangeList );//数据类型，是否是数组，数组上下界

};

class AssignS:public Statement
{
	public:
		VariableReference * variableReference;//左值
		Expression * expression;//右值

		AssignS();
		~AssignS();
};


class IfS :public Statement
{
public:
	Expression * condition;
	Statement * execute;//满足条件执行语句
	Statement * executeElse;//不满足条件执行语句

public:
	IfS();
	~IfS();
};

class ForS:public Statement
{
	public:
		pair<string, int> id;//循环变量
		Expression *start;//初始值
		Expression *end;//终止值
		Statement * circleS;//循环语句

		ForS();
		~ForS();
};

class WhileS :public Statement
{
	public:
		Expression * condition;//条件表达式
		Statement * execute;//循环体语句

		WhileS();
		~WhileS();
};

class RepeatS :public Statement
{
    public:
		Expression * condition;//条件表达式
		Statement * execute;//循环体语句

		RepeatS();
		~RepeatS();
};

class ReadS :public Statement
{
public:
	vector<VariableReference*> variableList;
	
	ReadS();
	~ReadS();
};

class WriteS :public Statement
{
public:
	vector<Expression*> expressionList;

	WriteS();
	~WriteS();
};

class VariableReference
{
	public:
		pair<string, int> variableId;//变量或常量标识符和行号
		int flag;//1表示数组，0表示非数组
		vector<Expression*> expressionList;//各维引用表达式列表

		VariableReference();
		~VariableReference();

		int locFlag;//-1表示左值，1表示右值
		string kind;//array,var,constant,function call,function return reference
		string variableType;//integer,real,char,boolean,error,其中error表示数组某一维下标表达式的类型不为"integer"或标识符不存在
};

class FunctionCall
{
	public:
		pair<string, int> functionId;//函数标识符
		vector<Expression*>actualParameterList;//实际参数列表，由表达式组成
		
		FunctionCall();
		~FunctionCall();

		string returnType;//error表示函数标识符不存在
};

class Expression
{
	public:
		string type;//表达式类型
		VariableReference* variableReference;//变量、常量、数组
		int intNum;
		float realNum;//浮点数
		string strOfNum;//整数和浮点数的string表示 （考虑从PASCAL-S源程序将字符串转为浮点数，再将浮点数转为字符串会带来精度问题，所以需要存下初始字符串值）
		char charValue;//常量字符
		FunctionCall * functionCall;//函数调用
		
		string operation;//具体操作符
		string operationType;//操作符类型,relop,mulop,addop,single等等
		Expression * operandf, *operands;

		int lineNum;//行号

		Expression();
		~Expression();

		//语义分析相关
		int totalIntValue;
		bool totalIntValueValid;
		string expressionType;//
		
};




#endif