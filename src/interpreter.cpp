#include "../include/interpreter.h"


//RuntimeVal functions-----------------------------

static bool checkType(const RuntimeVal& value0, const RuntimeVal& value1, TokenKind type0, TokenKind type1) {
    return value0.type == type0 && value1.type == type1;
}

static std::string repeatString(const std::string& str, int num) {
    std::string newStr = str;
    while (--num) newStr += str;
    return newStr;
}

bool RuntimeVal::operator== (const RuntimeVal& val) {
    if (checkType(*this, val, NUMBER, NUMBER) ) 
        return val.numberValue == this->numberValue;
    else if (checkType(*this, val, STRING, STRING))
        return val.stringValue == this->stringValue;
    else {
        val.opErrorStack.push_back("Comparaison operation undefined behaviour.");
        return 0;
    }
}

std::vector<std::string> RuntimeVal::opErrorStack;

bool RuntimeVal::operator>= (const RuntimeVal& val) {
    if (checkType(*this, val, NUMBER, NUMBER))
        return  this->numberValue >= val.numberValue;
    else if (checkType(*this, val, STRING, STRING))
        return this->stringValue >= val.stringValue;
    else {
        val.opErrorStack.push_back("Comparaison operation undefined behaviour.");
        return 0;
    }
}

bool RuntimeVal::operator<= (const RuntimeVal& val) {
    if (checkType(*this, val, NUMBER, NUMBER))
        return  this->numberValue <= val.numberValue;
    else if (checkType(*this, val, STRING, STRING))
        return this->stringValue <= val.stringValue;
    else {
        val.opErrorStack.push_back("Comparaison operation undefined behaviour.");
        return 0;
    }
}

bool RuntimeVal::operator> (const RuntimeVal& val)  {
    if (checkType(*this, val, NUMBER, NUMBER))
        return  this->numberValue > val.numberValue;
    else if (checkType(*this, val, STRING, STRING))
        return val.stringValue > this->stringValue;
    else {
        val.opErrorStack.push_back("Comparaison operation undefined behaviour.");
        return 0;
    }
}
bool RuntimeVal::operator< (const RuntimeVal& val)  {
    if (checkType(*this, val, NUMBER, NUMBER))
        return this->numberValue < val.numberValue;
    else if (checkType(*this, val, STRING, STRING))
        return this->stringValue < val.stringValue;
    else {
        val.opErrorStack.push_back("Comparaison operation undefined behaviour.");
        return 0;
    }
}


RuntimeVal RuntimeVal::operator+ (const RuntimeVal& val) {
    RuntimeVal result;
    if (checkType(*this, val, NUMBER, NUMBER)) {
        result.setNumber(this->numberValue + val.numberValue);
        return result;
    }
    else if (checkType(*this, val, STRING, STRING)) {
        result.setString(this->stringValue + val.stringValue);
        return result;
    }
    else {
        val.opErrorStack.push_back("Operation between incompatible types");
        return result;
    }
}

RuntimeVal RuntimeVal::operator- (const RuntimeVal& val) {
    RuntimeVal result;
    if (checkType(*this, val, NUMBER, NUMBER)) {
        result.setNumber(this->numberValue - val.numberValue);
        return result;
    }
    else if (checkType(*this, val, STRING, STRING)) {
        val.opErrorStack.push_back("Operation between incompatible types");
        return result;
    }
    else {
        val.opErrorStack.push_back("Operation between incompatible types");
        return result;
    }
}
RuntimeVal RuntimeVal::operator* (const RuntimeVal& val) {
    RuntimeVal result;
    if (checkType(*this, val, NUMBER, NUMBER)) {
        result.setNumber(this->numberValue * val.numberValue);
       
    }
    else if (checkType(*this, val, STRING, STRING)) {
        val.opErrorStack.push_back("Operation between incompatible types");
        
    }
    else if (checkType(*this, val, NUMBER, STRING)) {
        int tmp = (int)this->numberValue;
        result.setString(repeatString(val.stringValue, tmp));
    }
    else if (checkType(*this, val, STRING, NUMBER)) {
        int tmp = (int)val.numberValue;
        result.setString(repeatString(this->stringValue, tmp));
    }
    else {
        val.opErrorStack.push_back("Operation between incompatible types");  
    }

    return result;
}
RuntimeVal RuntimeVal::operator/ (const RuntimeVal& val) {
    RuntimeVal result;
    if (checkType(*this, val, NUMBER, NUMBER)) {
        if (val.numberValue == 0.0) {
            val.opErrorStack.push_back("0 division is undefined");
            return result;
        }
        result.setNumber(this->numberValue / val.numberValue);
        return result;
    }
    else if (checkType(*this, val, STRING, STRING)) {
        val.opErrorStack.push_back("Operation between incompatible types");
        return result;
    }
    else {
        val.opErrorStack.push_back("Operation between incompatible types");
        return result;
    }
}


void RuntimeVal::setString(std::string value) {
    this->type        = STRING;
    this->stringValue = value;
}

void RuntimeVal::setNumber(double value) {
    this->type        = NUMBER;
    this->numberValue = value;
}

//Interpreter------------------------------------
void Console::printRuntimeValue(RuntimeVal value) {
    if (value.type == NUMBER) 
        std::cout << value.numberValue << '\n';
    else 
        std::cout << value.stringValue << '\n';
}

void Console::print(std::string value) {
    std::cout << value << '\n';
}


Interpreter::Interpreter(AST program) {
  this->program = program;
  this->breakLoopFlag = false;
  this->continueLoopFlag = false;
}

void Interpreter::run() { this->run(program.stmnts); }

void Interpreter::run(std::vector<Stmnt*> stmnts) {
   for (auto stmnt = stmnts.begin() ; stmnt != stmnts.end() ; stmnt++) {
      switch ((*stmnt)->type) {
         case VARSTMNT: {
            VarStmnt* varStmnt = (VarStmnt*) *stmnt;
            this->variables[varStmnt->ident.lexeme] = this->evaluate(varStmnt->value);
            break;
         } 
         case WRITESTMNT : {
            // TODO : String concatenation (eg : write "Hello" + "world" + x)
            WriteStmnt* writeStmnt = (WriteStmnt*) *stmnt;
            if (writeStmnt->expr != nullptr) {
               this->console.printRuntimeValue(this->evaluate(writeStmnt->expr));
            }
            else {
                this->console.print(writeStmnt->stringLiteral); //Deprecated, this branch is never entered now
            }
            break;
         }
         case READSTMNT : {
            ReadStmnt* readStmnt = (ReadStmnt*) *stmnt;
            std::string tmp;
            RuntimeVal valTmp;
            std::cin >> tmp;
            try {
                size_t tmp1;
                double tmp0 = std::stod(tmp, &tmp1);
                if (tmp1 >= tmp.size())
                    valTmp.setNumber(tmp0);
                else
                    valTmp.setString(tmp);        
            }
            catch (const std::invalid_argument exception) {
                valTmp.setString(tmp);    
            }
            this->variables[readStmnt->variable.lexeme] = valTmp;
            break;
         }
         case IFSTMNT : {
            IfStmnt* ifStmnt = (IfStmnt*) *stmnt;
            RuntimeVal comparisonResult = this->evaluateComparison(ifStmnt->expr);
            if (comparisonResult.numberValue) {
               this->run(ifStmnt->stmnts);
            }
            break;
         }
         case WHILESTMNT: {
            WhileStmnt* whileStmnt = (WhileStmnt*) *stmnt;
            RuntimeVal comparisonResult = this->evaluateComparison(whileStmnt->expr);
            if (!comparisonResult.numberValue) break;
            this->run(whileStmnt->stmnts);
            if (breakLoopFlag) {
               breakLoopFlag = false;
               break;
            }
            stmnt--; // decrementing the iterator in order to re evaluate the while comparison.

            break;
         }
         case CONTINUESTMNT : {
            continueLoopFlag = true;
            break;
         }
         case BREAKSTMNT : {
            breakLoopFlag = true;
            break;
         }
         default: {}
      }
      while (!RuntimeVal::opErrorStack.empty()) {
          _logger.warning(RuntimeVal::opErrorStack.back());
          RuntimeVal::opErrorStack.pop_back();
      }
   }
}

RuntimeVal Interpreter::evaluate(Expr* expr) {
   RuntimeVal result;
   switch(expr->type) {
      case BINARYEXPR : {
         result = this->evaluateBinary(expr);
         break;
      }
      default : {}
   }

   return result;
}
//DONE
RuntimeVal Interpreter::evaluateComparison(Expr* expr) {
   BinaryExpr* bexpr = (BinaryExpr*)expr;
   RuntimeVal resultVal;
   resultVal.type = NUMBER;
   double& result = resultVal.numberValue;

  RuntimeVal left = this->evaluateBinary(bexpr->left);
  RuntimeVal right = {};
  if (bexpr->right)
    right = this->evaluateComparison(bexpr->right);

   switch (bexpr->oper.kind) {
      case TokenKind::EQEQ : {
         result = left == right;
         break;
      }
      case TokenKind::GT : {
         result = left > right;
         break;
      }
      case TokenKind::GTEQ : {
         result = left >= right;
         break;
      }
      case TokenKind::LT : {
         result = left < right;
         break;
      }
      case TokenKind::LTEQ : {
         result = left <= right;
         break;
      }
      default: {
         resultVal = left;
      }
   }
   return resultVal;
}

RuntimeVal Interpreter::evaluateBinary(Expr *expr) {
  BinaryExpr *bexpr = (BinaryExpr *)expr;
  RuntimeVal result;

  RuntimeVal left = this->evaluateTerm(bexpr->left);
  RuntimeVal right = {};
  if (bexpr->right)
    right = this->evaluateBinary(bexpr->right);

  switch (bexpr->oper.kind) {
  case TokenKind::PLUS: {
    result = left + right;
    break;
  }
  case TokenKind::MINUS: {
    result = left - right;
    break;
  }
  default: {
    result = left;
  }
  }
  return result;
}

RuntimeVal Interpreter::evaluateTerm(Expr *expr) {
  BinaryExpr *bexpr = (BinaryExpr *)expr;
  RuntimeVal result;

  RuntimeVal left = this->evaluateUnary(bexpr->left);
  RuntimeVal right = {};
  if (bexpr->right)
    right = this->evaluateTerm(bexpr->right);

   switch (bexpr->oper.kind) {
      case TokenKind::ASTERISK : {
         result = left * right;
         break;
      }
      case TokenKind::SLASH : {
         result = left / right;
         break;
      }
      default: {
         result = left;
      }
   }
   return result;
}

RuntimeVal Interpreter::evaluateUnary(Expr *expr) {
  UnaryExpr *uexpr = (UnaryExpr *)expr;

   RuntimeVal right = this->evaluatePrimary(&uexpr->right);
   
   if (uexpr->oper.kind == TokenKind::MINUS)
      right.numberValue *= -1;

  return right;
}

RuntimeVal Interpreter::evaluatePrimary(PrimaryExpr* expr) {
   PrimaryExpr* pexpr = (PrimaryExpr*) expr;
   RuntimeVal result;
   
   if (pexpr->expr != nullptr)
      return this->evaluate(pexpr->expr);

   switch (pexpr->value.kind) {
      case TokenKind::NUMBER : {
         result.setNumber(std::stod(pexpr->value.lexeme));
         break;
      }

      case TokenKind::STRING: {
          result.setString(pexpr->value.lexeme);
          break;
      }

      case TokenKind::IDENT : {
         if (this->variables.find(pexpr->value.lexeme) == this->variables.end())
            _logger.panic("Use of ndeclared variable : " + pexpr->value.lexeme);
         return this->variables.find(pexpr->value.lexeme)->second;
         break;
      }
      default : {
         _logger.panic("Runtime error");
      }
   }

  return result;
}

void Interpreter::dumpVars() {
   _logger.debug("VARIABLES DUMP");
   for (const auto& var : variables) {
      std::cout << var.first << " : " ;
      this->console.printRuntimeValue(var.second);
   }
}
