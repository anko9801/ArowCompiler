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
			std::string arg_name = "i";
			for (Types i : param_type) {arg_name.push_back('i');param_list.push_back(Seq(i, "i"));}
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
			// }else if(llvm::isa<MatchExprAST>(InsertPoint)) {
			// 	llvm::dyn_cast<MatchExprAST>(InsertPoint)->addLoop(stmt);
			// 	return true;
			}else{
				fprintf(stderr, "error: unknown InsertPoint\n");
			}
			return false;
		}


		prim_type str2Type(std::string Type) {
			if(Type == "int")
				return Type_int;
			else if(Type == "uint")
				return Type_uint;
			else if(Type == "float")
				return Type_float;
			else if(Type == "bool")
				return Type_bool;
			else if(Type == "char")
				return Type_char;
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
		StatementsAST *visitStatements(BaseAST *InsertPoint, int branch);
		BaseAST *visitStatement();
		BaseAST *visitJumpStatement();
		BaseAST *visitIfExpression();
		BaseAST *visitWhileExpression();
		BaseAST *visitMatchExpression();
		BaseAST *visitAssignmentExpression(Types type);
		BaseAST *visitExpression(BaseAST *lhs, Types type);
		BaseAST *visitAdditiveExpression(BaseAST *lhs, Types type);
		BaseAST *visitMultiplicativeExpression(BaseAST *lhs, Types type);
		BaseAST *visitCastExpression();
		BaseAST *visitImplicitCastNumber(BaseAST *src, Types impl_type);
		BaseAST *visitImplicitCastNonNull(BaseAST *src, Types impl_type);
		BaseAST *visitImplicitCastBits(BaseAST *src, Types impl_type);
		BaseAST *visitPostfixExpression();
		BaseAST *visitPrimaryExpression();

		bool isExpectedToken(TokenType type) {
			int bkup=Tokens->getCurIndex();
			while (Tokens->getCurString() == "\n") {
				if(Tokens->getCurType() == TOK_EOF) break;
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
			int bkup=Tokens->getCurIndex();
			while (Tokens->getCurString() == "\n") {
				if(Tokens->getCurType() == TOK_EOF) break;
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
