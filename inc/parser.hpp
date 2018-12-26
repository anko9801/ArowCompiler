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
		BaseAST *InsertPoint;
		bool warning = false;
		Types CurFuncType;
		bool setFuncType(Types type){CurFuncType = type;return true;}
		Types getFuncType(){return CurFuncType;}

		// 意味解析用各種識別子表
		std::vector<VariableDeclAST*> VariableTable;
		std::vector<PrototypeAST*> PrototypeTable;
		std::vector<FunctionAST*> FunctionTable;

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
			PrototypeAST *proto = new PrototypeAST(ReturnType, FuncName, param_list);
			TU->addPrototype(proto);
			PrototypeTable.push_back(proto);
		}

		bool SetInsertPoint(BaseAST* Block) {
			InsertPoint = Block;
			return true;
		}

		bool addStatement(BaseAST* stmt, int branch = 0) {
			if (llvm::isa<FunctionStmtAST>(InsertPoint)) {
				llvm::dyn_cast<FunctionStmtAST>(InsertPoint)->addStatement(stmt);
				return true;
			}else if(llvm::isa<IfExprAST>(InsertPoint)) {
				if (branch == 0) {
					llvm::dyn_cast<IfExprAST>(InsertPoint)->addThen(stmt);
				}else{
					llvm::dyn_cast<IfExprAST>(InsertPoint)->addElse(stmt);
				}
				return true;
			}else if(llvm::isa<WhileExprAST>(InsertPoint)) {
				llvm::dyn_cast<WhileExprAST>(InsertPoint)->addLoop(stmt);
				return true;
			}else{
				fprintf(stderr, "error: unknown InsertPoint\n");
			}
			return false;
		}

		Types confirm(std::string Callee, std::vector<BaseAST*> args) {
			PrototypeAST *proto;
			for (int i = 0;i < PrototypeTable.size();i++) {
				if (!PrototypeTable[i]) {
					fprintf(stderr, "%d:%d: error: undefined function %s\n", Tokens->getLine(), __LINE__, Callee.c_str());
					return Types(Type_null);
				}
				if (PrototypeTable[i]->getName() == Callee) {
					proto = PrototypeTable[i];
					break;
				}
			}

			for (int i = 0;i < FunctionTable.size();i++) {
				if (!FunctionTable[i]) {
					fprintf(stderr, "%d:%d: error: undefined function %s\n", Tokens->getLine(), __LINE__, Callee.c_str());
					return Types(Type_null);
				}
				if (FunctionTable[i]->getName() == Callee) {
					proto = FunctionTable[i]->getPrototype();
					break;
				}
			}

			for(int i=0;i<args.size();i++) {
				if (!(args[i]->getType() == proto->getParam()[i].Type)) {
					SAFE_DELETE(args[i]);
					fprintf(stderr, "%d:%d: error: no match for function param '%s'\n", Tokens->getLine(), __LINE__, Callee.c_str());
					return Types(Type_null);
				}
			}

			return proto->getType();
			/*if(find(PrototypeTable, Callee) != PrototypeTable.size()){
				index = find(PrototypeTable, Callee);
				fprintf(stderr, "%d %d\n", index, PrototypeTable.size());
				PrototypeAST *func = PrototypeTable[index];
				for(int i=0;i<args.size();i++) {
					if (!(args[i]->getType() == func.param[i].Type)) {
						SAFE_DELETE(args[i]);
						fprintf(stderr, "%d:%d: error: no match for function param '%s'\n", Tokens->getLine(), __LINE__, Callee.c_str());
						return Func();
					}
				}
				return func;
			}else if(find(FunctionTable, Callee) != FunctionTable.size()){
				index = find(FunctionTable, Callee);
				fprintf(stderr, "%d %d\n", index, FunctionTable.size());
				Func func = FunctionTable[index];
				for(int i=0;i<args.size();i++) {
					if (args[i]->getType() == func.param[i].Type) {}else{
						SAFE_DELETE(args[i]);
						fprintf(stderr, "%d:%d: error: no match for function param '%s'\n", Tokens->getLine(), __LINE__, Callee.c_str());
						return Func();
					}
				}
				return func;
			}else{
				fprintf(stderr, "%d:%d: error: undefined function %s\n", Tokens->getLine(), __LINE__, Callee.c_str());
				return Func();
			}*/
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
			if(type.Type == Type_int)
				return "int";
			else if(type.Type == Type_bool)
				return "bool";
			else if(type.Type == Type_float)
				return "float";
			else if(type.Type == Type_char)
				return "char";
			else if(type.Type == Type_null)
				return "null";
			else
				return "yannaiyo";
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
		std::vector<BaseAST*> visitStatements(BaseAST *InsertPoint, int branch);
		BaseAST *visitStatement();
		BaseAST *visitJumpStatement();
		BaseAST *visitIfExpression();
		BaseAST *visitWhileExpression();
		BaseAST *visitAssignmentExpression();
		BaseAST *visitExpression(BaseAST *lhs);
		BaseAST *visitAdditiveExpression(BaseAST *lhs);
		BaseAST *visitMultiplicativeExpression(BaseAST *lhs);
		BaseAST *visitCastExpression(BaseAST *lhs);
		BaseAST *visitPostfixExpression();
		BaseAST *visitPrimaryExpression();


	protected:


}Parser;

#endif
