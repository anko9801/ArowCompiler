#ifndef PARSER_HPP
#define PARSER_HPP

#include<algorithm>
#include<cstdio>
#include<cstdlib>
//#include<map>
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
		std::vector<Func> PrototypeTable;
		std::vector<Func> FunctionTable;

	protected:

	public:
		Parser(std::string filename);
		~Parser(){SAFE_DELETE(TU);SAFE_DELETE(Tokens);}
		bool doParse();
		TranslationUnitAST &getAST();

		int find(std::vector<Func> vec, Func func) {
			auto itr = std::find(vec.begin(), vec.end(), func);
			size_t index = std::distance(vec.begin(), itr);
			return index;
		}

		int find(std::vector<Func> vec, Seq seq) {
			auto first = vec.begin();
			auto last = vec.end();
			for (; first != last; ++first) {
				if (first->function_seq == seq) {
					auto itr = first;
					size_t index = std::distance(vec.begin(), itr);
					return index;
				}
			}
		}

		int find(std::vector<Func> vec, std::string name) {
			auto first = vec.begin();
			auto last = vec.end();
			for (; first != last; ++first) {
				if (first->function_seq.Name == name) {
					auto itr = first;
					size_t index = std::distance(vec.begin(), itr);
					return index;
				}
			}
		}

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
		BaseAST *visitArrayExpression(Types Type, int Size);
		BaseAST *visitStatement(Types func_type);
		BaseAST *visitJumpStatement(Types Type);
		BaseAST *visitIfExpression();
		BaseAST *visitWhileExpression();
		BaseAST *visitAssignmentExpression(Types Type);
		BaseAST *visitAdditiveExpression(BaseAST *lhs, Types Type);
		BaseAST *visitMultiplicativeExpression(BaseAST *lhs, Types Type);
		BaseAST *visitPostfixExpression(Types Type);
		BaseAST *visitPrimaryExpression(Types Type);


	protected:


}Parser;

#endif
