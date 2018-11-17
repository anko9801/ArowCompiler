#ifndef PARSER_HPP
#define PARSER_HPP

#include<algorithm>
#include<cstdio>
#include<cstdlib>
#include<map>
#include<string>
#include<vector>
#include"APP.hpp"
#include"AST.hpp"
#include"lexer.hpp"
//using namespace llvm;


/**
  * 構文解析・意味解析クラス
  */
typedef class Parser{
	public:

	private:
		TokenStream *Tokens;
		TranslationUnitAST *TU;

		//意味解析用各種識別子表
		std::vector<VariableDeclAST*> VariableTable;
		std::map<std::string, int> PrototypeTable;
		std::map<std::string, int> FunctionTable;

	protected:

	public:
		Parser(std::string filename);
		~Parser(){SAFE_DELETE(TU);SAFE_DELETE(Tokens);}
		bool doParse();
		TranslationUnitAST &getAST();

	private:
		/**
		  各種構文解析メソッド
		  */
		bool visitTranslationUnit();
		bool visitExternalDeclaration(TranslationUnitAST *tunit);
		PrototypeAST *visitFunctionDeclaration();
		FunctionAST *visitFunctionDefinition();
		PrototypeAST *visitPrototype();
		FunctionStmtAST *visitFunctionStatement(PrototypeAST *proto);
		VariableDeclAST *visitVariableDeclaration();
		BaseAST *visitArrayExpression(std::string Type, int Size);
		BaseAST *visitStatement(std::string func_type);
		BaseAST *visitJumpStatement(std::string Type);
		BaseAST *visitIfExpression();
		BaseAST *visitAssignmentExpression();
		BaseAST *visitAdditiveExpression(BaseAST *lhs, std::string Type);
		BaseAST *visitMultiplicativeExpression(BaseAST *lhs, std::string Type);
		BaseAST *visitPostfixExpression(std::string Type);
		BaseAST *visitPrimaryExpression(std::string Type);


	protected:


}Parser;

#endif
