#ifndef CODEGEN_HPP 
#define CODEGEN_HPP


#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include "llvm/ADT/APInt.h"
#include "llvm/IR/Constants.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/Linker/Linker.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Metadata.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/IR/MDBuilder.h"
#include "llvm/IR/ValueSymbolTable.h"
#include "APP.hpp"
#include "AST.hpp"
using namespace llvm;


/**
  * コード生成クラス
  */
class CodeGen{
	private:
		Function *CurFunc;		//現在コード生成中のFunction
		Module *Mod;				//生成したModule を格納
		IRBuilder<> *Builder;	//LLVM-IRを生成するIRBuilder クラス
		BasicBlock *CurBB;
		std::map<std::string/*Variable*/, Value*> VariableTable;
		BasicBlock *Error;

	public:
		CodeGen();
		~CodeGen();
		bool doCodeGen(TranslationUnitAST &tunit, std::string name, std::string link_file, bool with_jit);
		Module &getModule();
		LLVMContext GlobalContext;


	private:
		Type *generateType(Types type);
		bool generateTranslationUnit(TranslationUnitAST &tunit, std::string name);
		Function *generateFunctionDefinition(FunctionAST *func, Module *mod);
		Function *generatePrototype(PrototypeAST *proto, Module *mod);
		Value *generateFunctionStatement(FunctionStmtAST *func_stmt);
		Value *generateVariableDeclaration(VariableDeclAST *vdecl);
		Value *generateArray();
		Value *generateStatement(BaseAST *stmt);
		Value *generateIfStmt(IfStmtAST *if_expr);
		Value *generateWhileStmt(WhileStmtAST *while_expr);
		Value *generateCondition(BaseAST *Cond);
		Value *generateCompare(BaseAST *Cond);
		Value *generateExpression(BaseAST *expr);
		Value *generateBinaryExpression(BinaryExprAST *bin_expr);
		Value *generateSingleExpression(SingleExprAST *sin_expr);
		Value *generateCallExpression(CallExprAST *call_expr);
		Value *generateJumpStatement(JumpStmtAST *jump_stmt);
		Value *generateCastExpression(Value *src, Types SrcType, Types DestType);
		Value *generateVariable(VariableAST *var);
		Value *generateValue(ValueAST *val);
		bool linkModule(Module *dest, std::string file_name);
};


#endif
