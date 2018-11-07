#ifndef CODEGEN_HPP 
#define CODEGEN_HPP


#include<cstdio>
#include<cstdlib>
#include<map>
#include<string>
#include<vector>
#include<llvm/ADT/APInt.h>
#include<llvm/Constants.h>
#include<llvm/ExecutionEngine/ExecutionEngine.h>
#include<llvm/ExecutionEngine/JIT.h>
#include<llvm/Linker.h>
#include<llvm/LLVMContext.h>
#include<llvm/Module.h>
#include<llvm/Metadata.h>
#include<llvm/Support/Casting.h>
#include<llvm/IRBuilder.h>
#include<llvm/Function.h>
#include<llvm/Support/IRReader.h>
#include<llvm/MDBuilder.h>
#include<llvm/ValueSymbolTable.h>
#include"APP.hpp"
#include"AST.hpp"
using namespace llvm;


/**
  * コード生成クラス
  */
class CodeGen{
	private:
		Function *CurFunc;		//現在コード生成中のFunction
		Module *Mod;				//生成したModule を格納
		IRBuilder<> *Builder;	//LLVM-IRを生成するIRBuilder クラス

	public:
		CodeGen();
		~CodeGen();
		bool doCodeGen(TranslationUnitAST &tunit, std::string name, std::string link_file, bool with_jit);
		Module &getModule();


	private:
		bool generateTranslationUnit(TranslationUnitAST &tunit, std::string name);
		Function *generateFunctionDefinition(FunctionAST *func, Module *mod);
		Function *generatePrototype(PrototypeAST *proto, Module *mod);
		Value *generateFunctionStatement(FunctionStmtAST *func_stmt);
		Value *generateVariableDeclaration(VariableDeclAST *vdecl);
		Value *generateStatement(BaseAST *stmt);
		Value *generateBinaryExpression(BinaryExprAST *bin_expr);
		Value *generateCallExpression(CallExprAST *call_expr);
		Value *generateJumpStatement(JumpStmtAST *jump_stmt);
		Value *generateVariable(VariableAST *var);
		Value *generateNumber(int value);
		Value *generateBoolean(bool value);
		Value *generateIfExpr(IfExprAST *if_expr);
		bool linkModule(Module *dest, std::string file_name);
};


#endif
