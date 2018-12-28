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
			for (int i = 0;;i++) {
				if (!PrototypeTable[i]) {
					for (int i = 0;;i++) {
						if (!FunctionTable[i]) {
							fprintf(stderr, "%d:%d: error: undefined function %s\n", Tokens->getLine(), __LINE__, Callee.c_str());
							return Types(Type_null);
						}
						if (FunctionTable[i]->getName() == Callee) {
							proto = FunctionTable[i]->getPrototype();
							break;
						}
					}
					fprintf(stderr, "%d:%d: error: undefined function %s\n", Tokens->getLine(), __LINE__, Callee.c_str());
					return Types(Type_null);
				}
				if (PrototypeTable[i]->getName() == Callee) {
					proto = PrototypeTable[i];
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

		void printAST(BaseAST* stmt, int nest = 0){
			for(int i=0; i < nest; i++) fprintf(stderr, "	");
			if(!stmt) {
				fprintf(stderr, "break\n");
				return ;
			}
			else if(llvm::isa<FunctionStmtAST>(stmt)){
				fprintf(stderr, "FunctionStatement\n");
				for (int i = 0;;i++)if (llvm::dyn_cast<FunctionStmtAST>(stmt)->getStatement(i))printAST(llvm::dyn_cast<FunctionStmtAST>(stmt)->getStatement(i), nest+1);else break;
			}else if(llvm::isa<VariableDeclAST>(stmt))
				fprintf(stderr, "VariableDeclaration\n");
			else if(llvm::isa<CastExprAST>(stmt))
				fprintf(stderr, "CastExpression\n");
			else if(llvm::isa<BinaryExprAST>(stmt))
				fprintf(stderr, "BinaryExpression\n");
			else if(llvm::isa<CallExprAST>(stmt))
				fprintf(stderr, "CallExpression\n");
			else if(llvm::isa<JumpStmtAST>(stmt))
				fprintf(stderr, "JumpStatement\n");
			else if(llvm::isa<VariableAST>(stmt))
				fprintf(stderr, "Variable\n");
			else if(llvm::isa<IfExprAST>(stmt)){
				fprintf(stderr, "IfExpression\n");
				printAST(llvm::dyn_cast<IfExprAST>(stmt)->getCond(), nest);
				for (int i = 0;;i++)if (llvm::dyn_cast<IfExprAST>(stmt)->getThen(i))printAST(llvm::dyn_cast<IfExprAST>(stmt)->getThen(i), nest+1);else break;
				for (int i = 0;;i++)if (llvm::dyn_cast<IfExprAST>(stmt)->getElse(i))printAST(llvm::dyn_cast<IfExprAST>(stmt)->getElse(i), nest+1);else break;
			}
			else if(llvm::isa<WhileExprAST>(stmt)) {
				fprintf(stderr, "WhileExpression\n");
				printAST(llvm::dyn_cast<WhileExprAST>(stmt)->getCond(), nest);
				for (int i = 0;;i++)if (llvm::dyn_cast<WhileExprAST>(stmt)->getLoop(i))printAST(llvm::dyn_cast<WhileExprAST>(stmt)->getLoop(i), nest+1);else break;
			}
			else if(llvm::isa<ValueAST>(stmt))
				fprintf(stderr, "Value\n");
			else
				fprintf(stderr, "unknown\n");
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
		StatementsAST *visitStatements(BaseAST *InsertPoint, int branch);
		BaseAST *visitStatement();
		BaseAST *visitJumpStatement();
		BaseAST *visitIfExpression();
		BaseAST *visitWhileExpression();
		BaseAST *visitAssignmentExpression();
		BaseAST *visitExpression(BaseAST *lhs);
		BaseAST *visitAdditiveExpression(BaseAST *lhs);
		BaseAST *visitMultiplicativeExpression(BaseAST *lhs);
		BaseAST *visitCastExpression();
		BaseAST *visitPostfixExpression();
		BaseAST *visitPrimaryExpression();


	protected:


}Parser;

#endif
