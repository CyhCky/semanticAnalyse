/**
 * @file semanticAnalyse.cpp
 * @author Cyh__Cky (1245532616@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-30
 * 
 * 
 */
#include<string>
#include<algorithm>
#include<iostream>
#include<cstdio>
#include<set>
#include<sstream>
#include"semanticAnalyse.h"
#include"symbolTable.h"
#include"AST.h"

using namespace std;

_SymbolTable *mainSymbolTable;//指向主符号表的指针
_SymbolTable *curSymbolTable;//指向当前符号表的指针
vector<string> ErrInfoList;//错误信息列表
vector<string> WarnInfoList;//警告信息列表
int ErrorLimit = 0;

void semanticAnalyse(Program *ast_root,int ErrLimit)
{
	//构造主符号表，进行初始化
	mainSymbolTable = new _SymbolTable;
	mainSymbolTable->isMainTable = true;
	mainSymbolTable->fatherSymbolTable = NULL;


	//设置错误数量上限
	ErrorLimit = ErrLimit;
	
	//开始进行语义分析
	analyseProgram(ast_root);
}

void analyseProgram(Program *ast)//分析主程序
{
	if(ast == NULL)//若节点为NULL，输出错误信息
	{
		printf("[AnalyseProgram]point is NULL\n");
		return;
	}

	set<string> lib;
	lib.insert("read");
	lib.insert("write");
	lib.insert("writeln");
	lib.insert("exit");

	if(lib.find(ast->programId.first) != lib.end())//与四个基础函数重名
	{
		addMulDefErrInfo(ast->programId.first, 0, ast->programId.second, "");
	}
	//设置主符号表表名
	mainSymbolTable->tableName = ast->programId.first;
	mainSymbolTable->tableDetailedType = "proc";

	for(int i=0;i<ast->parameterList.size();i++)
	{
		if(ast->parameterList[i].first == ast->programId.first)//主程序参数与主程序名重名
		{
			addMulDefErrInfo(ast->programId.first,-1,ast->programId.second,"");
		}
		if(lib.find(ast->parameterList[i].first) != lib.end())//主程序参数与默认函数重名
		{
			addMulDefErrInfo(ast->parameterList[i].first,0,ast->parameterList[i].second,"");
		}
		_SymbolRecord *record = new _SymbolRecord;
		record->setParaMainRecord(ast->parameterList[i].first,ast->parameterList[i].second);//插入主程序参数
	}
	//插入4个默认函数
	_SymbolRecord *readRecord,*writeRecord,*writelnRecord,*exitRecord;
	readRecord = new _SymbolRecord;
	writeRecord = new _SymbolRecord;
	writelnRecord = new _SymbolRecord;
	exitRecord = new _SymbolRecord;
	readRecord->setProcRecord("read",0,-1,NULL);
	writeRecord->setProcRecord("write",0,-1,NULL);
	writelnRecord->setProcRecord("writeln", 0, -1, NULL);
	exitRecord->setProcRecord("exit",0,0,NULL);
	mainSymbolTable->insertRecord(readRecord);
	mainSymbolTable->insertRecord(writeRecord);
	mainSymbolTable->insertRecord(writelnRecord);
	mainSymbolTable->insertRecord(exitRecord);

	analyseSubProgram(ast->subProgram);
}

void analyseSubProgram(SubProgram *ast)
{
	if(ast == NULL)
	{
		printf("[AnalyseSubProgram] point is NULL\n");
		return;
	}
	for(int i=0;i<ast->constList.size();i++)//遍历子程序常量
	{
		analyseConsts(ast->constList[i]);
	}
	for(int i=0;i<ast->variableList.size();i++)//遍历子程序变量
	{
		analyseVariable(ast->variableList[i]);
	}
	for(int i=0;i<ast->subprogramDefinitionList.size();i++)//遍历子程序定义
	{
		analyseSubProgramDef(ast->subprogramDefinitionList[i]);
		curSymbolTable = mainSymbolTable;
	}

	analyseStatement((Statement*)(ast->compound));
}

void analyseConsts(Const *ast)
{
	if(ast == NULL)
	{
		printf("[AnalyseConst] point is NULL\n");
		return;
	}
	string id = ast->constId.first;
	// if(curSymbolTable->isVaildid(id))//与主函数，主函数参数重名
	// {
	// 	addMulDefErrInfo(id,-2,ast->constId.second,"");
	// 	return;
	// }
	_SymbolRecord tmpRecord;
	int result = curSymbolTable->convertIdToRecord(id,tmpRecord);
	_SymbolRecord *record = new _SymbolRecord;
	if(result > 0)//找到重名记录
	{
		if(tmpRecord.lineNumber == ast->constId.second) return;//找到自身就跳过，不进行报错
		addMulDefErrInfo(tmpRecord.id,tmpRecord.lineNumber,ast->constId.second,tmpRecord.type);
		return;
	}
	if(ast->type == "id")
	{
		_SymbolRecord preRecord;
		int res = curSymbolTable->convertIdToRecord(ast->valueId.first,preRecord);
		if(res == 0)//未找到定义信息
		{
			addUnDefErrInfo(ast->valueId.first,ast->valueId.second);
			return;
		}
		if(preRecord.type != "const")//找到的记录信息不是常量
		{
			addTypeErrInfo(ast->valueId.first,ast->valueId.second,"const",preRecord.type);
			return;
		}
		string val = preRecord.constValue;
		if(ast->isMinusShow == 1)//负数
		{
			if(val[0] == '-') val.erase(val.begin());
			else val = "-"+val;
		}
		record->setConstRecord(id,ast->constId.second,preRecord.type,val);
	}
	else
	{
		string value = ast->strOfVal;
		if(ast->isMinusShow == 1)//负数
		{
			if(value[0] == '-') value.erase(value.begin());
			else value = "-" + value;
		}
		record->setConstRecord(id,ast->constId.second,ast->type,value);
	}
	curSymbolTable->insertRecord(record);
}

void analyseVariable(Variable *ast)//对变量定义进行语义分析
{
	if(ast == NULL)
	{
		printf("[AnalyseVariable] point is NULL\n");
		return;
	}
	string id = ast->variableId.first;
	int line = ast->variableId.second;
	_SymbolRecord tmpRecord,*record=NULL;
	int result = curSymbolTable->convertIdToRecord(id,tmpRecord);
	if(result > 0)//变量定义重名
	{
		if(ast->variableId.second == tmpRecord.lineNumber) return;//找到自身，不进行报错
		addMulDefErrInfo(id,tmpRecord.lineNumber,line,tmpRecord.type);
		return;
	}
	if(tmpRecord.detailedType != "array")//非数组变量
	{
		record = new _SymbolRecord;
		record->setVarRecord(id, line, ast->type->type.first);
		curSymbolTable->insertRecord(record);
		return;
	}
	else//定义的是数组，需要对上下界进行判断
	{
		auto tmp = &(ast->type->arrayRangeList);
		for(int i=0;i<tmp->size();i++)
		{
			if((*tmp)[i].first > (*tmp)[i].second)//下界大于上界
			{
				addArrayRangeErrInfo(id,line,i+1,(*tmp)[i]);
				(*tmp)[i].second = (*tmp)[i].first;//错误恢复策略，将上界置成下界
			}
		}
		record = new _SymbolRecord;
		record->setArrayRecord(id, line, ast->type->type.first, 1+(tmp->size()), (*tmp));
		curSymbolTable->insertRecord(record);
	}
}



void analyseSubProgramDef(SubprogramDefinition *ast)
{
	if(ast == NULL)
	{
		printf("[Sub Program Define Error] point is NULL\n");
		return;
	}

	string id = ast->funtionID.first;
	int line = ast->funtionID.second;
	_SymbolRecord tmpRecord,*record;
	int result = curSymbolTable->convertIdToRecord(id,tmpRecord);
	if(result > 0)//若重名
	{
		addMulDefErrInfo(id,tmpRecord.lineNumber,line,tmpRecord.type);
		return;
	}

	string type;//判断该子程序是过程还是函数
	if(ast->type.first.length() == 0)//若为空串，则代表该子程序为过程
	{
		type = "proc";
	}
	else//否则为函数
	{
		type = "func";
	}

	curSymbolTable = new _SymbolTable(id,mainSymbolTable);

	record = new _SymbolRecord;
	if(type == "proc")
	{
		record->setProcRecord(id, line, (int)(ast->formalParameterList.size()), curSymbolTable);
		mainSymbolTable->insertRecord(record);
	}
	else if(type == "func")
	{
		record->setFuncRecord(id, line, ast->type.first, (int)(ast->formalParameterList.size()), curSymbolTable);
		mainSymbolTable->insertRecord(record);
	}
	curSymbolTable->tableDetailedType = type;

	for(int i=0;i<ast->formalParameterList.size();i++)//遍历形参列表
	{
		analyseFormalParameter(ast->formalParameterList[i]);
	}
	for(int i=0;i<ast->constList.size();i++)//遍历常量列表
	{
		analyseConsts(ast->constList[i]);
	}
	for(int i=0;i<ast->variableList.size();i++)//遍历变量列表
	{
		analyseVariable(ast->variableList[i]);
	}
	analyseStatement((Statement*)(ast->compound));

	if(type == "proc")//类型为过程，不必检查返回完备
		return;
	else if(type == "func")
	 	isFuncHasReturn(ast);
}


void analyseFormalParameter(FormalParameter *ast)
{
	if(ast == NULL)
	{
		printf("[Formal Parameter Error] point is NULL\n");
		return;
	}
	string id = ast->paraId.first;
	int line = ast->paraId.second;
	_SymbolRecord tmpRecord,*record;
	int result = curSymbolTable->convertIdToRecord(id, tmpRecord);
	if(curSymbolTable->isVaildid(id))
	{
		if(tmpRecord.lineNumber != ast->paraId.second)
		{
			addMulDefErrInfo(id, -2, line, "");	
			return;	
		} 
	}
	if(result > 0)//存在重名
	{
		addMulDefErrInfo(id, tmpRecord.lineNumber, line, tmpRecord.type);
		return;
	}
	record = new _SymbolRecord;
	if(ast->flag == 0)//flag为0代表传值调用，1代表引用调用
		record->setParaValRecord(id, line, ast->type);
	else
		record->setParaVarRecord(id, line, ast->type);
	curSymbolTable->insertRecord(record);
}

void analyseStatement(Statement *ast)
{
	if(ast == NULL)
	{
		printf("[Analyse Statement Error] point is NULL\n");
		return;
	}
	if(ast->type == "compound")//如果为复合语句
	{
		CompoundS *p = (CompoundS*) ast;
		for(int i=0;i<p->statementList.size();i++)
		{
			analyseStatement(p->statementList[i]);
		}
	}
	else if(ast->type == "repeat")//repeat语句
	{
		RepeatS *p = (RepeatS*) ast;
		string nowDetailedType = analyseExpression(p->condition);
		if(nowDetailedType != "boolean")//表达式类型不为boolean
		{
			addExpressionDetailedTypeErrInfo(p->condition, nowDetailedType, "boolean", "condition for repeat statement");
			p->statementType = "error";
		}
		else
			p->statementType = "void";
		analyseStatement(p->execute);
	}
	else if(ast->type == "while")//while语句(与repeat语句类似)
	{
		WhileS *p =(WhileS*) ast;
		string nowDetailedType = analyseExpression(p->condition);
		if(nowDetailedType != "boolean")//表达式类型不为boolean
		{
			addExpressionDetailedTypeErrInfo(p->condition, nowDetailedType, "boolean", "condition for while statement");
			p->statementType = "error";
		}
		else
			p->statementType = "void";
		analyseStatement(p->execute);
	}
	else if(ast->type == "for")//for语句
	{
		ForS *p = (ForS*) ast;
		_SymbolRecord tmpRecord,*record;
		int result = curSymbolTable->convertIdToRecord(p->id.first, tmpRecord);
		if(result > 0)//循环变量已被定义
		{
			string type = tmpRecord.type, detailedType = tmpRecord.detailedType;
			if(type != "para_val" && type != "para_var" && type != "var")
			{
				addTypeErrInfo(p->id.first, p->id.second, type, "var");
				return;
			}
			if(detailedType != "integer")
			{
				addDetailedTypeErrInfo(p->id.first, p->id.second, detailedType, "integer");
				return;
			}
		}
		else //循环变量未被定义
		{
			addUnDefErrInfo(p->id.first, p->id.second);
			return;
		}
		

		string startDetailedType = analyseExpression(p->start);
		string endDetailedType = analyseExpression(p->end);
		p->statementType = "void";
		if(startDetailedType != "integer")
		{
			addExpressionDetailedTypeErrInfo(p->start, startDetailedType, "integer", "start condition for for_statement");
			p->statementType = "error";
		}
		if(endDetailedType != "integer")
		{
			addExpressionDetailedTypeErrInfo(p->end, endDetailedType, "integer", "end condition for for_statement");
			p->statementType = "error";
		}

		analyseStatement(p->circleS);
	}
	else if(ast->type == "if")//if语句
	{
		IfS *p = (IfS*) ast;
		string conditionDetailedType = analyseExpression(p->condition);
		if(conditionDetailedType != "boolean")
		{
			addExpressionDetailedTypeErrInfo(p->condition, conditionDetailedType, "boolean", "condition for if statement");
			p->statementType = "error";
		}
		else p->statementType = "void";
		analyseStatement(p->execute);
		if(p->executeElse != NULL)//存在else分支
			analyseStatement(p->executeElse);
	}
	else if(ast->type == "assign")//赋值语句
	{
		AssignS *p = (AssignS*) ast;
		p->statementType = "void";
		p->variableReference->locFlag = -1;//-1代表为左值
		string leftDetailedType = analyseVariableReference(p->variableReference);
		if(p->variableReference->kind == "const")//赋值语句左边为常量
		{
			addStrErrorInfo("[Assign to Const Error] <Line: "+int2str(p->variableReference->variableId.second)+"> Const as left value of assign statement.");
			return;
		}
		string rightDetailedType = analyseExpression(p->expression);

		if(p->variableReference->kind ==  "function return reference")//若该语句为返回值语句
		{
			if(p->variableReference->variableType != rightDetailedType && (!(p->variableReference->variableType == "real" && rightDetailedType == "integer")))//左值右值类型不匹配
			{//左侧实数，右侧整数，属于支持的隐式类型转换
				addStrErrorInfo("[Return Detailed Type Error] <Line: "+ int2str(p->variableReference->variableId.second)\
									+"> Function return the wrong detailed type.");
				p->statementType = "error";
			}
		}
		else
		{
			p->isReturn = true;
			return;
		}

		if(leftDetailedType != rightDetailedType&&(!(leftDetailedType == "real" && rightDetailedType == "integer")))//左值右值类型不匹配
		{//赋值语句左侧实数，右侧整数，支持隐式类型转换，则不报错
			addAssignDetailedTypeDisMatchErrInfo(p->variableReference, p->expression);
			p->statementType = "error";
		}
		else
		{
			p->statementType = "void";
		}
	}
	else if(ast->type == "procedure")
	{
		ProcedureCall *p = (ProcedureCall*) ast;
		//通过符号表获得过程的记录
		_SymbolRecord procRecord;
		int result = curSymbolTable->convertIdToRecord(p->procedureId.first, procRecord);
		p->statementType = "void";
		if(result == 0)//未找到相应记录
		{
			addUnDefErrInfo(p->procedureId.first, p->procedureId.second);
			p->statementType = "error";
			return;
		}
		if(procRecord.type != "proc")//找到的记录不是过程
		{
			addTypeErrInfo(p->procedureId.first, p->procedureId.second, procRecord.type, "proc");
			p->statementType = "error";
			return;
		}
		if(procRecord.id == "exit")//语句为exit函数,参数能否存在取决于其在过程中还是函数中被调用
		{

			if(curSymbolTable->tableDetailedType == "proc")//为过程的符号表
			{
				if(p->actualParameterList.size() != 0)//过程中的exit不带参数
				{
					addInputNumErrInfo("exit", p->procedureId.second, p->actualParameterList.size(), 0, "proc");
					p->statementType = "error";
				}
				return;
			}
			else //为函数的符号表
			{
				if(p->actualParameterList.size() != 1)//函数中的exit带一个参数
				{
					addInputNumErrInfo("exit", p->procedureId.second, p->actualParameterList.size(), 1, "func");
					return;
				}
				string returnType = analyseExpression(p->actualParameterList[0]);
				if(returnType != curSymbolTable->tableDetailedType&&(!(curSymbolTable->tableDetailedType == "real" && returnType == "integer")))//返回值与函数类型不同
				{//隐式类型转换
					addStrErrorInfo("[Return Detailed Type Error] <Line: "+ int2str(p->procedureId.second)\
									+"> Function type is \""+curSymbolTable->tableDetailedType+"\" ,but return "\
									+returnType + ".");
					p->statementType = "error";
					
				}
				else
				{
					p->isReturn = true;
					return;
				}
			}
		}//exit函数(过程)处理结束
		if(procRecord.id == "read")//如果是read函数
		{
			if(p->actualParameterList.size() == 0)//read函数输入参数不能为0
			{
				addStrErrorInfo("[Read Parameter Missing Error] <Line: "+ int2str(p->procedureId.second)+"> "\
								+"Read must have at least 1 parameter." );
				p->statementType = "error";
			}
			for(int i=0;i<p->actualParameterList.size();i++)
			{
				if(!(p->actualParameterList[i]->type == "var" && (p->actualParameterList[i]->variableReference->kind == "var" || p->actualParameterList[i]->variableReference->kind == "array")))
				//read内参数类型不能被写入
				{
					addStrErrorInfo("[Read Parameter Type Error] <Line: "+ int2str(p->procedureId.second)+"> "\
								+"Parameter type must be var." );
				}
				if (p->actualParameterList[i]->expressionType == "boolean")
					addStrErrorInfo("[Read Parameter Detailed Type Error] <Line: "+int2str(p->procedureId.second)+"> "\
								+"Parameter detailed type can not be boolean.");
				string type = analyseExpression(p->actualParameterList[i]);
				if(type == "error")
					p->statementType = "error";
				
			}
			return;
		}//read处理结束
		if(ast->type == "write")
		{
			if(p->actualParameterList.size() == 0)//write函数输入参数不能为0,与read函数同理
			{
				addStrErrorInfo("[Write Parameter Missing Error] <Line: "+ int2str(p->procedureId.second)+"> "\
								+"Write must have at least 1 parameter." );
				p->statementType = "error";
			}
		}
		if(procRecord.numOfFunc == -1)//使用-1来代表变参过程
		{
			for(int i=0;i<p->actualParameterList.size();i++)
			{
				string type = analyseExpression(p->actualParameterList[i]);
				if(type == "error")
				{
					p->statementType = "error";
				}
			}
			return;
		}
		if(procRecord.numOfFunc != p->actualParameterList.size())//过程定义时参数与调用时参数个数不匹配
		{
			addInputNumErrInfo(p->procedureId.first, p->procedureId.second, p->actualParameterList.size(), procRecord.numOfFunc, "proc");
			p->statementType = "error";
			return;
		}
		vector<pair<string,string>> parameterTable = mainSymbolTable->findAllFormalParaType(p->procedureId.first);
		for(int i=0;i<p->actualParameterList.size();i++)//检测过程的参数定义与调用时是否匹配
		{
			string actualDetailedType = analyseExpression(p->actualParameterList[i]);
			if(parameterTable[i].first == "para_var" &&\
				!(p->actualParameterList[i]->type == "var"&&(p->actualParameterList[i]->variableReference->kind == "var" || p->actualParameterList[i]->variableReference->kind == "array")))
			{//表达式不能为引用类型的实参
				addStrErrorInfo("[Reference Actual Parameter Error] <Line :"+ int2str(p->actualParameterList[i]->lineNum)+"> "\
								+"The "+int2str(i+1)+" parameter type error.");
				continue;
			}
			if(parameterTable[i].first == "para_val")//为传值调用
			{
				if(actualDetailedType != parameterTable[i].second&& (!(actualDetailedType == "integer" && parameterTable[i].second == "integer")))//类型不匹配
				{//隐式类型转换
					addExpressionDetailedTypeErrInfo(p->actualParameterList[i], actualDetailedType, parameterTable[i].second, \
					int2str(i+1)+"th parameter");
					p->statementType = "error";
				}
				
					
			}
			else //为引用调用，具体类型必须相同
			{
				if(actualDetailedType != parameterTable[i].second)
				{
					addExpressionDetailedTypeErrInfo(p->actualParameterList[i], actualDetailedType, parameterTable[i].second, \
						int2str(i+1)+"th parameter");
					p->statementType = "error";
				}
			}

		}
	}//procedure处理结束
	else 
	{
		printf("[Analyse Statement Error] statement type error.");
		return;
	}
}


string analyseExpression(Expression *ast)
{
	if(ast == NULL)
	{
		printf("[Analyse Expression Error] point is NULL\n");
		return "";
	}
	if(ast->type == "var")//如果是变量/常量/数组
	{
		string varDetailedType = analyseVariableReference(ast->variableReference);
		if(varDetailedType == "integer" && ast->variableReference->kind == "const")
		{
			_SymbolRecord tmpRecord;
			int result = curSymbolTable->convertIdToRecord(ast->variableReference->variableId.first, tmpRecord);
			if(result == 0)//无此记录
			{
				printf("[Analyse Expression Error] point of record is NULL\n");
				return "";
			}
			else if(tmpRecord.type != "const")//查到类型不为常量
			{
				printf("[Analyse Expression Error] The record type must be const\n");
				return "";
			}
			ast->totalIntValue = str2int(tmpRecord.constValue);
			ast->totalIntValueValid = true;
		}
		return ast->expressionType = varDetailedType;
	}
	else if(ast->type == "integer")
	{
		ast->totalIntValue = ast->intNum;
		ast->totalIntValueValid = true;
		return ast->expressionType = "integer";
	}
	else if(ast->type == "real")
	{
		return ast->expressionType = "real";
	}
	else if(ast->type == "char")
	{
		return ast->expressionType = "char";
	}
	else if(ast->type == "function")
	{
		return ast->expressionType = analyseFunctionCall(ast->functionCall);
	}
	else if(ast->type == "compound")
	{
		if(ast->operationType == "relop")
		{
			string epType1 = analyseExpression(ast->operandf);
			string epType2 = analyseExpression(ast->operands);
			if ((epType1 == epType2 && epType1 != "error") || (epType1 == "integer" && epType2 == "real") || (epType1 == "real" && epType2 == "integer"))
				return ast->expressionType = "boolean";
			else
			{
				if(epType1 != epType2 && epType1 != "error" && epType2 != "error") 
					addOperandExpressionDetailedTypeDisMatchErrInfo(ast->operandf, ast->operands);
				return ast->expressionType = "error";
			}
		}
		else if (ast->operation == "not") 
		{
			string type = analyseExpression(ast->operandf);
			if (type == "boolean")//只允许boolean类型取反
				return ast->expressionType = "boolean";
			else
			{
				if(type != "error" && type != "boolean")//不为boolean类型，且之前未报错过
					addSingleOperandExpressionDetailedTypeDisMatchErrInfo(ast->operandf, "boolean");
				return ast->expressionType = "error";
			}
		}
		else if (ast->operation == "minus") //负号
		{
			string epType = analyseExpression(ast->operandf);
			if(epType == "integer" && ast->operandf->totalIntValueValid)
			{
				ast->totalIntValue = - ast->operandf->totalIntValue;
				ast->totalIntValueValid = true;
			}
			if (epType == "integer" || epType == "real")
				return ast->expressionType = epType;
			else
			{
				if(epType != "error" && epType !="integer" && epType !="real") //为不能取反的类型
					addSingleOperandExpressionDetailedTypeDisMatchErrInfo(ast->operandf, "integer or real");
				return ast->expressionType = "error";
			}
		}
		else if (ast->operation == "bracket")//括号
		{
			ast->expressionType = analyseExpression(ast->operandf);
			if(ast->expressionType == "integer" && ast->operandf->totalIntValueValid)
			{
				ast->totalIntValue = ast->operandf->totalIntValue;
				ast->totalIntValueValid = true;
			}
			return ast->expressionType;
		}
		else if (ast->operation == "+" || ast->operation == "-" || ast->operation == "*" || ast->operation == "/") 
		{
			string epType1 = analyseExpression(ast->operandf);
			string epType2 = analyseExpression(ast->operands);

			if (ast->operation=="/" && epType2 == "integer" && ast->operands->totalIntValueValid && ast->operands->totalIntValue == 0)
			{//除0判断
				addDiv0ErrorInfo(ast->operands, ast->operation);
			}
			if(epType1 == "integer" && epType2 == "integer" && ast->operandf->totalIntValueValid && ast->operands->totalIntValueValid)
			{
				ast->totalIntValueValid = true;
				if(ast->operation == "+")
					ast->totalIntValue = ast->operandf->totalIntValue + ast->operands->totalIntValue;
				else if(ast->operation == "-")
					ast->totalIntValue = ast->operandf->totalIntValue - ast->operands->totalIntValue;
				else if(ast->operation == "*")
					ast->totalIntValue = ast->operandf->totalIntValue * ast->operands->totalIntValue;
				else 
					ast->totalIntValue = ast->operandf->totalIntValue / ast->operands->totalIntValue;
			}
			if((epType1=="integer" || epType1=="real") && (epType2=="integer" || epType2=="real"))//实数整数的隐式转换
			{
				if(epType1=="integer" && epType2=="integer")
					return ast->expressionType="integer";
				return ast->expressionType="real";
			}
			if(epType1 != "error" && epType1 != "integer" && epType1 != "real") //操作数不能进行四则运算
				addSingleOperandExpressionDetailedTypeDisMatchErrInfo(ast->operandf, "integer or real");
			if(epType2 != "error" && epType2 != "integer" && epType2 != "real") //操作数不能进行四则运算
				addSingleOperandExpressionDetailedTypeDisMatchErrInfo(ast->operands, "integer or real");
			return ast->expressionType = "error";
		}
		else if (ast->operation == "and" || ast->operation == "or")//与或运算
		{
			string epType1 = analyseExpression(ast->operandf);
			string epType2 = analyseExpression(ast->operands);
			if (epType1 == "boolean" && epType2 == "boolean")
				return ast->expressionType = "boolean";
			if (epType1 != "error" && epType1 != "boolean") //不为boolean类型，不能进行与或运算
				addSingleOperandExpressionDetailedTypeDisMatchErrInfo(ast->operandf, "boolean");
			if (epType2 != "error" && epType2 != "boolean") //不为boolean类型，不能进行与或运算
				addSingleOperandExpressionDetailedTypeDisMatchErrInfo(ast->operands, "boolean");
			return ast->expressionType = "error";
		}
		else
		{
			printf("[Analyse Expression Error] opeartion not found\n");
			return "error";
		}
	}
	else return "error";
}

string analyseVariableReference(VariableReference *ast)
{
	if(ast==NULL){
		printf("[Analyse VariantReference Error] point is NULL\n");
		return "";
	}
	_SymbolRecord tmpRecord;
	int result = curSymbolTable->convertIdToRecord(ast->variableId.first, tmpRecord);
	if (result == 0) {//未找到记录
		addUnDefErrInfo(ast->variableId.first, ast->variableId.second);
		return ast->variableType = "error";//无法找到变量引用的类型
	}
	//首先必须是函数
	//作为左值必须是当前函数名
	//作为右值则可以是任意函数名（递归调用也可以）
	//是左值还是右值、是过程还是函数、是当前符号表对应的子程序名称、还是别的子程序名称

	if (ast->flag == 0) {//如果是非数组
		if (result == 2 && ast->variableId.first == curSymbolTable->tableName) 
		{
			if (tmpRecord.detailedType == "proc") //查找到过程
			{
				addStrErrorInfo("[Invalid reference Error] <Line " + int2str(ast->variableId.second)\
								 + "> Procedure name \"" + tmpRecord.id + "\" can't be referenced");
				return ast->variableType = "error";
			}
			//如果是左值，那么是返回语句，不需要关注参数
			//如果是右值，那么是递归调用，需要关注参数，即检查形式参数个数是否是0个
			if (ast->locFlag == -1)  //查找到函数,如果是左值
			{
				ast->kind = "function return reference";
				return ast->variableType = tmpRecord.detailedType;
			}
			//如果是右值
			if (tmpRecord.numOfFunc != 0) //如果形参个数不为0
			{
				addInputNumErrInfo(ast->variableId.first, ast->variableId.second, tmpRecord.numOfFunc, 0, "func");
				return ast->variableType = tmpRecord.detailedType;
			}
			//如果形参个数为0
			//这样对应到具体程序中，实际上是无参函数的递归调用
			ast->kind = "function call";
			return ast->variableType = tmpRecord.detailedType;
		}
		if (tmpRecord.type == "func")//如果是函数 则一定是在主符号表中查到的
		{
			ast->kind = "function";
			//被识别为variantReference的函数调用一定不含实参，所以需要检查形参个数
			if (ast->locFlag == -1)//不能作为左值
			{
				addStrErrorInfo("[Invalid Reference Error] <Line " + int2str(ast->variableId.second) + "> function name \"" + tmpRecord.id + "\" can't be referenced as l-value.");
				return ast->variableType = "error";
			}
			else //如果是右值
			{
				if (tmpRecord.numOfFunc != 0) //如果形参个数不为0
				{
					addInputNumErrInfo(ast->variableId.first, ast->variableId.second, tmpRecord.numOfFunc, 0, "func");
					return ast->variableType = tmpRecord.detailedType;
				}
			}
			return ast->variableType = tmpRecord.detailedType;
		}
		//checked
		if (!(tmpRecord.type == "para_val" || tmpRecord.type == "para_var" || tmpRecord.type == "var" || tmpRecord.type == "const")) 
		{
			addStrErrorInfo("[Invalid Reference Error] <Line " + int2str(ast->variableId.second) + "> \"" + ast->variableId.first + "\" is a " + tmpRecord.type + ", it can't be referenced.");
			return ast->variableType = "error";
		}
		ast->kind = "var";
		if (tmpRecord.type == "const")
			ast->kind = "constant";
		return ast->variableType = tmpRecord.detailedType;
	}
	else //如果是数组
	{
		if (ast->flag == 1) 
		{
			if (tmpRecord.type != "array") //如果符号表中记录的不是数组
			{
				addTypeErrInfo(ast->variableId.first, ast->variableId.second, "array", tmpRecord.type);
				return ast->variableType = "error";
			}
			ast->kind = "array";
			if (ast->expressionList.size() != tmpRecord.numDimensionsOfArray) //如果引用时的下标维数和符号表所存不一致
			{
				addInputNumErrInfo(ast->variableId.first, ast->variableId.second, (int)(ast->expressionList.size()), tmpRecord.numDimensionsOfArray, "array");
				ast->variableType = "error";
				return tmpRecord.detailedType;
			}
			ast->variableType = tmpRecord.detailedType;
			for (int i = 0; i < ast->expressionList.size(); i++) 
			{
				string detailedType = analyseExpression(ast->expressionList[i]);
				if (detailedType != "integer") //检查下标表达式的类型是否是整型
				{
					addExpressionDetailedTypeErrInfo(ast->expressionList[i], detailedType, "integer", int2str(i+1)+"th index of array \"" + ast->variableId.first + "\"");
					ast->variableType = "error";
				}
				if(ast->expressionList[i]->totalIntValueValid)//检查越界
				{
					_SymbolTable *st;
					if(result == 1) st = curSymbolTable;
					else st = mainSymbolTable;
					if(!(st->isIndexInRange(tmpRecord.id, i+1, ast->expressionList[i]->totalIntValue)))//数组下标越界
					{
						addIndexOutOfRangeErrInfo(ast->expressionList[i], ast->variableId.first, i+1, tmpRecord.boundsOfArray[i]);
						ast->variableType = "error";
					}
				}
			}
			return tmpRecord.detailedType;
			
		}
		else 
		{
			printf("[Analyse VariantReference Error] Flag of variantReference is not 0 or 1\n");
			return ast->variableType = "error";
		}
	}
	
}

string analyseFunctionCall(FunctionCall *ast)
{
	if(ast==NULL){
		cout << "[Analyse FunctionCall] point is null" << endl;
		return "";
	}
	_SymbolRecord tmpRecord;
	int result = curSymbolTable->convertIdToRecord(ast->functionId.first, tmpRecord);
	if (result == 0) //未定义
	{
		addUnDefErrInfo(ast->functionId.first, ast->functionId.second);
		return ast->returnType = "error";
	}
	if (tmpRecord.type != "func") //不是函数
	{
		addTypeErrInfo(ast->functionId.first, ast->functionId.second, tmpRecord.type, "function");
		return ast->returnType = "error";
	}
	if (tmpRecord.numOfFunc == -1) //-1代表变参函数
	{
		for (int i = 0; i < ast->actualParameterList.size(); i++) 
			string actualDetailType = analyseExpression(ast->actualParameterList[i]);
		return ast->returnType = tmpRecord.detailedType;
	}
	if (ast->actualParameterList.size() != tmpRecord.numOfFunc) //参数个数不一致
	{
		addInputNumErrInfo(ast->functionId.first, ast->functionId.second, (int)(ast->actualParameterList.size()), tmpRecord.numOfFunc, "func");
		return ast->returnType = tmpRecord.detailedType;
	}
	vector<pair<string,string>> parameterTable = mainSymbolTable->findAllFormalParaType(ast->functionId.first);
	for (int i = 0; i < ast->actualParameterList.size(); i++) //检查各位置的实参和形参类型是否一致 形参在符号表中的定位
	{
		string actualDetailedType = analyseExpression(ast->actualParameterList[i]);
			if(parameterTable[i].first == "para_var" &&\
				!(ast->actualParameterList[i]->type == "var"&&(ast->actualParameterList[i]->variableReference->kind == "var" || ast->actualParameterList[i]->variableReference->kind == "array")))
			{//表达式不能为引用类型的实参
				addStrErrorInfo("[Reference Actual Parameter Error] <Line :"+ int2str(ast->actualParameterList[i]->lineNum)+"> "\
								+"The "+int2str(i+1)+" parameter type error.");
				continue;
			}
			if(parameterTable[i].first == "para_val")//为传值调用
			{
				if(actualDetailedType != parameterTable[i].second)//类型不匹配
				{
					if(actualDetailedType == "integer" && parameterTable[i].second == "integer");//隐式类型转换
					else
					{
						addExpressionDetailedTypeErrInfo(ast->actualParameterList[i], actualDetailedType, parameterTable[i].second, \
						int2str(i+1)+"th parameter");
					}
				}
				
					
			}
			else //为引用调用，具体类型必须相同
			{
				if(actualDetailedType != parameterTable[i].second)
				{
					addExpressionDetailedTypeErrInfo(ast->actualParameterList[i], actualDetailedType, parameterTable[i].second, \
						int2str(i+1)+"th parameter");
				}
			}
	}
	return ast->returnType = tmpRecord.detailedType;
}

/*----------------以下为返回值完备检查函数--------------------------------*/


bool isStatementHasReturn(Statement *ast)
{
	if (ast == NULL) 
	{
		printf("[Statement Not Exist Return] point is NULL\n");
		return false;
	}
	if (ast->type == "compound") 
	{
		CompoundS *compound = (CompoundS*)ast;
		for (int i = 0; i < compound->statementList.size(); i++)
		{
			if (isStatementHasReturn(compound->statementList[i]))
				return true;
		}
		return false;
	}
	else if (ast->type == "repeat") 
	{
		RepeatS *repeatStatement = (RepeatS*) ast;
		return isStatementHasReturn(repeatStatement->execute);
	}
	else if (ast->type == "while") 
	{
		WhileS *whileStatement = (WhileS*)ast;
		return isStatementHasReturn(whileStatement->execute);
	}
	else if (ast->type == "for") 
	{
		ForS *forStatement = (ForS*)ast;
		return isStatementHasReturn(forStatement->circleS);
	}
	else if (ast->type == "if") 
	{
		IfS *ifStatement = (IfS*)ast;
		if (ifStatement->execute == NULL) 
		{
			cout << "[If Statement Not Exist Return] the point is null";
			return false;
		}
		if (ifStatement->executeElse == NULL)
			return false;
		else
			return isStatementHasReturn(ifStatement->execute) && isStatementHasReturn(ifStatement->executeElse);
	}
	else if (ast->type == "assign") //判断是否是返回值语句
	{ 
		AssignS *assignStatement = (AssignS*) ast;
		return assignStatement->isReturn; //在之前的语义分析中，已经保存是否是返回值语句,即fun:=expression的情况
	}
	else if (ast->type == "procedure") //判断是否是返回值语句
	{ 
		ProcedureCall *procedureCall = (ProcedureCall*) ast;
		return procedureCall->isReturn; //在之前的语义分析中，已经保存是否是返回值语句,即exit()的情况
	}
	else 
	{
		cout << "[Statement Not Exist Return] statement type error" << endl;
		return false;
	}
}

void isCompoundHasReturn(CompoundS *ast, string funcId, int funcLine)
{
	if (ast == NULL) 
	{
		printf("[Compound Not Exist Return] point is NULL\n");
		return;
	}
	bool flag = false;
	for (int i = 0; i < ast->statementList.size() && !flag; i++) 
	{
		if (isStatementHasReturn(ast->statementList[i]))
			flag = true;
	}
	if (!flag) 
	{
		string warningInfo="";
		warningInfo = warningInfo +"[Return value statement missing!] "\
							+"<Line " + int2str(funcLine) + "> "\
							+"Incomplete return value statement of function \"" + funcId + "\".";
		WarnInfoList.push_back(warningInfo);
	}
}

void isFuncHasReturn(SubprogramDefinition *ast)
{
	if (ast == NULL) 
	{
		printf("[Function Not Exist Return] pointer is NULL\n");
		return;
	}
	isCompoundHasReturn(ast->compound, ast->funtionID.first, ast->funtionID.second);
}


/*---------------以下为错误信息输出实现--------------*/
void addErrAndCheckLimit(string errInfo)
{
	ErrInfoList.push_back(errInfo);

	if(ErrorLimit == 0)//未设置错误信息上限
		return;
	else//设置了上限
	{
		if(ErrInfoList.size() == ErrorLimit)
		{
			printf("More than %d errors, compiler abort\n",ErrorLimit);
		}
	}
}



void addStrErrorInfo(string s)
{
	addErrAndCheckLimit(s);
}

void addMulDefErrInfo(string preId, int preLine, int curLine, string type)
{
	string errInfo="[MultiDefineError] <Line: "+int2str(curLine)+">";
	if(preLine == 0)//与默认4函数重名
		errInfo += "\"" + preId + "\"" + "has already defined as lib program.";
	else if(preLine == -1)//与程序名重名
		errInfo += "\"" + preId + "\"" + "is the same name as program.";
	else if(preLine == -2)//与某物重名
		errInfo += "\"" + preId + "\"" + "use the existed name.";
	else
		errInfo += "\"" + preId + "\"" + "has already degined as " + type + "at Line: " + int2str(preLine);
	errInfo += ".";
	addErrAndCheckLimit(errInfo);
}


void addUnDefErrInfo(string id, int line)
{
	string errInfo = "[UndefinedError] <Line: " + int2str(line) + "> " + id + \
					"has not been defined. ";
	addErrAndCheckLimit(errInfo);
}


void addInputNumErrInfo(string id, int line, int nowNum, int trueNum, string funcType)
{
	string errInfo;
	if(funcType == "array")//数组下标数量错误
	{
		errInfo = "[Array Index Number Error] <Line: " + int2str(line) + "> Array:" + id + "should have "\
				+ int2str(trueNum) + "indices but not" + int2str(nowNum);
	}
	else if(funcType == "proc")//过程下标数量错误
	{
		errInfo = "[Procedure Parameter Number Error] <Line: " + int2str(line) + "> Procedure: " + id + "should have "\
				+ int2str(trueNum) + "parameters but not" + int2str(nowNum);
	}
	else if(funcType == "func")//函数下标数量错误
	{
		errInfo = "[Function Parameter Number Error] <Line: " + int2str(line) + "> Function: " + id + "should have "\
				+ int2str(trueNum) + "parameters but not " + int2str(nowNum);
	}
	else
	{
		printf("[Input Number Error] usage error\n");
		return;
	}
	errInfo += ".";
	addErrAndCheckLimit(errInfo);
}



void addDetailedTypeErrInfo(string id, int line, string nowDetailedType, string trueDetailedType)
{
	string errInfo = "[Detailed Type Error] <Line: " + int2str(line) + ">"\
					+"\"" + id + "\"" + "should be " + trueDetailedType + "but not " +nowDetailedType + ".";
	addErrAndCheckLimit(errInfo);
}


void addTypeErrInfo(string id, int line, string nowType, string trueType)
{
	string errInfo = "[Type Error] <Line: " + int2str(line) + ">"\
					+"\"" + id + "\"" + "should be " + trueType + "but not " + nowType + ".";
	addErrAndCheckLimit(errInfo);
}

void addAssignDetailedTypeDisMatchErrInfo(VariableReference *left, Expression *right)
{
	string errInfo = "[Assign Detailed Type Error] <Left Line: " + int2str(left->variableId.second) +">, "\
					+"<Right Line: " + int2str(right->lineNum) + "> "\
					+"left " + left->variableId.first + "type is " + left->variableType\
					+ "but right type is " + right->expressionType + ".";
	addErrAndCheckLimit(errInfo);
}

void addIndexOutOfRangeErrInfo(Expression *ast, string id, int dimNum, pair<int, int> range)
{
	string errInfo = "[Array Index Out Of Range Error] <Line: " + int2str(ast->lineNum) + "> "\
					+"The value of expression out of array " + id + " " + int2str(dimNum) + "th index is "\
					+int2str(range.first) + "to " +int2str(range.second) + ".";

	addErrAndCheckLimit(errInfo);
}


void addArrayRangeErrInfo(string id, int line, int dimNum, pair<int, int> range)
{
	string errInfo = "[Array Index Range Error] <Line: " + int2str(line) + "> "\
					+" the lower bound (" + int2str(range.first) + ") of " + id\
					+" is larger than the upper bound (" + int2str(range.second) + ").";
	addErrAndCheckLimit(errInfo);
}


void addSingleOperandExpressionDetailedTypeDisMatchErrInfo(Expression *ast, string trueDetailedType)
{
	string errInfo = "[Operand Expression Type Error] <Line: " + int2str(ast->lineNum) + "> "\
					+" Expression type should be " + trueDetailedType + " but not " + ast->expressionType + ".";
	addErrAndCheckLimit(errInfo);
}


void addOperandExpressionDetailedTypeDisMatchErrInfo(Expression *exp1,Expression *exp2)
{
	string errInfo = "[Operand Expression Type Error] <Left Line: " + int2str(exp1->lineNum) \
					+", Right Line: " + int2str(exp2->lineNum) + "> "\
					+"Left type is " + exp1->expressionType\
					+ " while right type is " + exp2->expressionType + ".";
	addErrAndCheckLimit(errInfo);
}


void addDiv0ErrorInfo(Expression *ast, string opt)
{
	string errInfo = "[Div 0 Error] <Line: " + int2str(ast->lineNum) + "> "\
					+"The second operand of operation " + "\"" + opt + "\"" +"can not be 0.";
	addErrAndCheckLimit(errInfo);
}

void addExpressionDetailedTypeErrInfo(Expression *ast, string nowDetailedType, string trueDetailedType, string info)
{
	string errInfo = "[Expression Detailed Type Error] <Line:" +int2str(ast->lineNum) + "> "\
					+" Expression used for " + info + " should be " + trueDetailedType\
					+" but not " + nowDetailedType;
	addErrAndCheckLimit(errInfo);
}

/*-----------------以下为工具函数实现-------------------*/
int str2int(string s)
{
	int n = s.length();
	int start_pos = 0, ans = 0, f = 1;
	if(s[0]=='-')
	{
		start_pos = 1;
		f = -1;
	}
	for(int i=start_pos;i<n;i++)
	{
		ans = ans*10 + (s[i]-'0');
	}
	return ans*f;
}


string int2str(int a)
{
	stringstream s;
	s << a;
	return s.str();
}