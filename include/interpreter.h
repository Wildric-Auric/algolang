#pragma once

#include <unordered_map>
#include "ast.h"
#include "logger.h"



class RuntimeVal {
public:
	void setString(std::string value);
	void setNumber(double value);
	double      numberValue = 0.0;
	std::string stringValue = "";
	TokenKind   type = NUMBER;

	bool operator== (const RuntimeVal& val);
	bool operator>= (const RuntimeVal& val);
	bool operator<= (const RuntimeVal& val);
	bool operator> (const RuntimeVal& val);
	bool operator< (const RuntimeVal& val);

	RuntimeVal operator+ (const RuntimeVal& val);
	RuntimeVal operator- (const RuntimeVal& val);
	RuntimeVal operator* (const RuntimeVal& val);
	RuntimeVal operator/ (const RuntimeVal& val);

	static std::vector<std::string> opErrorStack;
};

class Console { 
public:
	void printRuntimeValue(RuntimeVal value);
	void print(std::string value);
};



class Interpreter {
public:
   Interpreter(AST program);
   
   void run();
   void run(std::vector<Stmnt*> stmnts);
   void traverse();
   RuntimeVal evaluate(Expr *expr);
   RuntimeVal evaluateComparison(Expr* expr);
   RuntimeVal evaluateBinary(Expr* expr);
   RuntimeVal evaluateTerm(Expr* expr);
   RuntimeVal evaluateUnary(Expr* expr);
   RuntimeVal evaluatePrimary(PrimaryExpr* expr);
   void dumpVars();
   
private:
   AST program;
   std::unordered_map<std::string, RuntimeVal> variables;
   Console console;
   Logger _logger;
   int conditionFlag;
   bool breakLoopFlag;
   bool continueLoopFlag; 
};
