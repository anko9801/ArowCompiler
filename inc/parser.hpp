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

		void addLib(TranslationUnitAST* TU,
				Types ReturnType, std::string FuncName,
				std::initializer_list<Types> param_type) {
			std::vector<Seq> param_list;
			for (Types i : param_type) param_list.push_back(Seq(i, "i"));
			TU->addPrototype(new PrototypeAST(ReturnType, FuncName, param_list));
			PrototypeTable.push_back(Func(ReturnType, FuncName, param_list));
		}

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

		prim_type str2Type(std::string Type) {
			if(Type == "int")
				return Type_int;
			else if(Type == "bool")
				return Type_bool;
			else if(Type == "float")
				return Type_float;
			else if(Type == "char")
				return Type_char;
			else
				return Type_null;
		}

		std::string printType(Types type) {
			if(type.Type == prim_type::Type_int)
				return "int";
			else if(type.Type == prim_type::Type_bool)
				return "bool";
			else if(type.Type == prim_type::Type_float)
				return "float";
			else if(type.Type == prim_type::Type_char)
				return "char";
			else if(type.Type == prim_type::Type_null)
				return "null";
		}

	private:
		/**
		  各種構文解析メソッド
		  */
		bool visitTranslationUnit();
		bool visitExternalDeclaration(TranslationUnitAST *tunit);
		Types visitTypes();
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
