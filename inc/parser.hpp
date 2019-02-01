#ifndef PARSER_HPP
#define PARSER_HPP

#include<algorithm>
#include<cstdio>
#include<cstdlib>
#include<string>
#include<vector>
#include"APP.hpp"
#include"AST.hpp"
#include"lexer.hpp"


/**
  * 構文解析・意味解析クラス
  */
typedef class Parser{
	public:

	private:
		TokenStream *Tokens;
		TranslationUnitAST *TU;
		StatementsAST *InsertPoint;
		bool warning = false;
		std::string error;
		Types CurFuncType;

		// 意味解析用各種識別子表
		std::vector<VariableDeclAST*> VariableTable;
		std::vector<PrototypeAST*> PrototypeTable;
		std::vector<FunctionAST*> FunctionTable;

	protected:

	public:
		Parser(std::string filename);
		~Parser() {SAFE_DELETE(TU);SAFE_DELETE(Tokens);}
		bool doParse();
		TranslationUnitAST &getAST();
		bool setFuncType(Types type) {CurFuncType = type;return true;}
		Types getFuncType() {return CurFuncType;}

		VariableDeclAST *getVariable(size_t i) {
			if (i < VariableTable.size()) {
				return VariableTable[i];
			}
			return NULL;
		}
		PrototypeAST *getPrototype(size_t i) {
			if (i < PrototypeTable.size()) {
				return PrototypeTable[i];
			}
			return NULL;
		}
		FunctionAST *getFunction(size_t i) {
			if (i < FunctionTable.size()) {
				return FunctionTable[i];
			}
			return NULL;
		}

		bool SetInsertPoint(StatementsAST* Block) {
			InsertPoint = Block;
			return true;
		}

		bool addStatement(BaseAST* stmt, int branch = 0) {
			InsertPoint->addStatement(stmt);
			return true;
		}

		prim_type str2Type(std::string Type) {
			if (Type == "int")
				return Type_int;
			else if (Type == "uint")
				return Type_uint;
			else if (Type == "float")
				return Type_float;
			else if (Type == "bool")
				return Type_bool;
			else if (Type == "char")
				return Type_char;
			else if (Type == "void")
				return Type_void;
			else
				return Type_null;
		}

	private:
		/**
		  各種構文解析メソッド
		  */
		bool visitTranslationUnit();
		bool visitModule();
		bool visitExternalDeclaration(TranslationUnitAST *tunit);
		Types visitTypes();
		PrototypeAST *visitFunctionDeclaration();
		FunctionAST *visitFunctionDefinition();
		PrototypeAST *visitPrototype();
		FunctionStmtAST *visitFunctionStatement(PrototypeAST *proto);
		bool visitImportFile();
		VariableDeclAST *visitVariableDeclaration();
		BaseAST *visitArrayExpression(Types Type, int Size);
		StatementsAST *visitStatements(StatementsAST *InsertPoint, std::vector<VariableDeclAST*> vars);
		BaseAST *visitStatement();
		BaseAST *visitJumpStatement();
		BaseAST *visitIfStatement();
		BaseAST *visitWhileStatement();
		BaseAST *visitMatchExpression();
		IfStmtAST *visitPatternExpression(BaseAST *Eval);
		BaseAST *visitAssignmentExpression(Types type);
		BaseAST *visitCondition(BaseAST *lhs, Types type);
		BaseAST *visitExpression(BaseAST *lhs, Types type);
		BaseAST *visitAdditiveExpression(BaseAST *lhs, Types type);
		BaseAST *visitMultiplicativeExpression(BaseAST *lhs, Types type);
		BaseAST *visitCastExpression();
		BaseAST *visitImplicitCastNumber(BaseAST *src, Types impl_type);
		BaseAST *visitImplicitCast(BaseAST *src, Types impl_type);
		BaseAST *visitPostfixExpression();
		BaseAST *visitPrimaryExpression();

		bool isExpectedToken(TokenType type) {
			int bkup = Tokens->getCurIndex();
			while (Tokens->getCurString() == "\n") {
				if (Tokens->getCurType() == TOK_EOF) break;
				if (Tokens->getCurType() == type) return true;
				Tokens->getNextToken();
			}
			if (Tokens->getCurType() == type) return true;
			else{
				Tokens->applyTokenIndex(bkup);
				return false;
			}
		}

		bool isExpectedToken(std::string str) {
			int bkup = Tokens->getCurIndex();
			while (Tokens->getCurString() == "\n") {
				if (Tokens->getCurType() == TOK_EOF) break;
				if (Tokens->getCurString() == str) return true;
				Tokens->getNextToken();
			}
			if (Tokens->getCurString() == str) return true;
			else{
				Tokens->applyTokenIndex(bkup);
				return false;
			}
		}
	protected:


}Parser;

#endif
