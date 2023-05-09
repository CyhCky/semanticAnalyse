/*AST�ڵ��ඨ��*/

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
		string type;//assign��if��for...
		string statementType;//void ���� error
		int lineNum;//�к�
		bool isReturn;//�Ƿ��Ƿ���ֵ���

		Statement(){}
		~Statement(){}
};

class Program
{
	public:
		pair<string, int> programId;//�����ʶ�����к�
		vector<pair<string, int>> parameterList;//�����б��к�
		SubProgram * subProgram;//�ӳ���

		Program();
		~Program();
};

class SubProgram
{
	public:
		vector<Const*> constList;//���������б�
		vector<Variable*> variableList;//���������б�
		vector<SubprogramDefinition*> subprogramDefinitionList;//�ӳ�����Ӻ��������б�
		CompoundS* compound;//��������

		SubProgram();
		~SubProgram();
};

class Variable
{
	public:
		pair<string, int> variableId;//������ʶ�����к�
		Type * type;//��������

		Variable();
		Variable(pair<string, int>, Type*);
		~Variable();
};

class Const
{
	public:
		pair<string, int> constId;
		string type;//��������
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
		vector<Statement*> statementList;//����б� �к���begin��λ�þ���
		
		CompoundS();
		~CompoundS();
};

class SubprogramDefinition
{
public:
	pair<string, int> funtionID;//���̱�ʶ��
	vector<FormalParameter*> formalParameterList;//��ʽ�����б�
	pair<string, int> type;//type.first���ǿմ�����Ϊ���̣�����Ϊ������ȡֵΪinteger,real,boolean�ȵ�
	vector<Const*> constList;//���������б�
	vector<Variable*> variableList;//���������б�
	CompoundS* compound;

	SubprogramDefinition();
	~SubprogramDefinition();
};

class ProcedureCall:public Statement
{
	public:
		pair<string, int> procedureId;//���̱�ʶ��
		vector<Expression*> actualParameterList;//ʵ�ʲ����б��ɱ��ʽ���

		ProcedureCall();
		~ProcedureCall();
};

class FormalParameter
{
	public:
		pair<string, int> paraId;//�βα�ʶ�����к�
		string type;//�β�����һ��Ϊ�������ͣ��ĳ�string����
		int flag;//flagΪ0�Ǵ�ֵ���ã�Ϊ1��ʾ���õ���

		FormalParameter();
		~FormalParameter(){}
		FormalParameter(pair<string, int> theParaId, string theType, int theFlag);
};

class Type
{
	public:
		pair<string, int> type;//�������ͼ��к�
		int isArray;//0��ʾ������
		vector<pair<int, int>> arrayRangeList;//�������½�

		Type();
		~Type() {}
		Type(pair<string, int> , int , vector< pair<int, int>> arrayRangeList );//�������ͣ��Ƿ������飬�������½�

};

class AssignS:public Statement
{
	public:
		VariableReference * variableReference;//��ֵ
		Expression * expression;//��ֵ

		AssignS();
		~AssignS();
};


class IfS :public Statement
{
public:
	Expression * condition;
	Statement * execute;//��������ִ�����
	Statement * executeElse;//����������ִ�����

public:
	IfS();
	~IfS();
};

class ForS:public Statement
{
	public:
		pair<string, int> id;//ѭ������
		Expression *start;//��ʼֵ
		Expression *end;//��ֵֹ
		Statement * circleS;//ѭ�����

		ForS();
		~ForS();
};

class WhileS :public Statement
{
	public:
		Expression * condition;//�������ʽ
		Statement * execute;//ѭ�������

		WhileS();
		~WhileS();
};

class RepeatS :public Statement
{
    public:
		Expression * condition;//�������ʽ
		Statement * execute;//ѭ�������

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
		pair<string, int> variableId;//����������ʶ�����к�
		int flag;//1��ʾ���飬0��ʾ������
		vector<Expression*> expressionList;//��ά���ñ��ʽ�б�

		VariableReference();
		~VariableReference();

		int locFlag;//-1��ʾ��ֵ��1��ʾ��ֵ
		string kind;//array,var,constant,function call,function return reference
		string variableType;//integer,real,char,boolean,error,����error��ʾ����ĳһά�±���ʽ�����Ͳ�Ϊ"integer"���ʶ��������
};

class FunctionCall
{
	public:
		pair<string, int> functionId;//������ʶ��
		vector<Expression*>actualParameterList;//ʵ�ʲ����б��ɱ��ʽ���
		
		FunctionCall();
		~FunctionCall();

		string returnType;//error��ʾ������ʶ��������
};

class Expression
{
	public:
		string type;//���ʽ����
		VariableReference* variableReference;//����������������
		int intNum;
		float realNum;//������
		string strOfNum;//�����͸�������string��ʾ �����Ǵ�PASCAL-SԴ�����ַ���תΪ���������ٽ�������תΪ�ַ���������������⣬������Ҫ���³�ʼ�ַ���ֵ��
		char charValue;//�����ַ�
		FunctionCall * functionCall;//��������
		
		string operation;//���������
		string operationType;//����������,relop,mulop,addop,single�ȵ�
		Expression * operandf, *operands;

		int lineNum;//�к�

		Expression();
		~Expression();

		//����������
		int totalIntValue;
		bool totalIntValueValid;
		string expressionType;//
		
};




#endif