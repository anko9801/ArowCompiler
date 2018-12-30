#include "AST.hpp"


/**
  * PrototypeAST(関数宣言追加)メソッド
  * @param  VariableDeclAST
  * @retirm true
  */
bool TranslationUnitAST::addPrototype(PrototypeAST *proto){
		Prototypes.push_back(proto);
		return true;
}


/**
  * FunctionAST(関数定義追加)メソッド
  * @param  VariableDeclAST
  * @retirm true
  */
bool TranslationUnitAST::addFunction(FunctionAST *func){
		Functions.push_back(func);
		return true;
}

bool TranslationUnitAST::empty(){
	if(Prototypes.size()==0 && Functions.size()==0)
		return true;
	else
		return false;
}


/**
  * デストラクタ
  */
TranslationUnitAST::~TranslationUnitAST(){
	for(size_t i = 0;i < Prototypes.size(); i++){
		SAFE_DELETE(Prototypes[i]);
	}
	Prototypes.clear();

	for(size_t i = 0;i < Functions.size(); i++){
		SAFE_DELETE(Functions[i]);
	}
	Functions.clear();
}


FunctionAST::~FunctionAST(){
	SAFE_DELETE(Proto);
	SAFE_DELETE(Body);
}


StatementsAST::~StatementsAST(){
	for(size_t i=0; i<Statements.size(); i++){
		SAFE_DELETE(Statements[i]);
	}
	Statements.clear();

	for(size_t i=0; i<VarDecls.size(); i++){
		SAFE_DELETE(VarDecls[i]);
	}
	VarDecls.clear();
}


FunctionStmtAST::~FunctionStmtAST(){
	for(size_t i = 0;i < StmtLists.size();i++){
		SAFE_DELETE(StmtLists[i]);
	}
	StmtLists.clear();
}


BinaryExprAST::~BinaryExprAST(){
	SAFE_DELETE(LHS);
	SAFE_DELETE(RHS);
}


IfExprAST::~IfExprAST() {
	SAFE_DELETE(Cond);
}


WhileExprAST::~WhileExprAST() {
	SAFE_DELETE(Cond);
}


CallExprAST::~CallExprAST(){
	// if (Args.size() != 0) {
	// 	for(size_t i = 0;i < Args.size();i++){
	// 		SAFE_DELETE(Args[i]);
	// 	}
	// }
	// Args.clear();
}


JumpStmtAST::~JumpStmtAST() {
	// SAFE_DELETE(Expr);
}


VariableAST::~VariableAST() {
	// SAFE_DELETE(Var);
}


// AST::~AST() {
// 	SAFE_DELETE();
// }


Types BaseAST::getType() {
	if (llvm::isa<VariableDeclAST>(this)) {
		return llvm::dyn_cast<VariableDeclAST>(this)->getType();
	}else if (llvm::isa<JumpStmtAST>(this)) {
		return llvm::dyn_cast<JumpStmtAST>(this)->getType();
	}else if (llvm::isa<BinaryExprAST>(this)) {
		return llvm::dyn_cast<BinaryExprAST>(this)->getType();
	}else if (llvm::isa<CallExprAST>(this)) {
		return llvm::dyn_cast<CallExprAST>(this)->getType();
	}else if (llvm::isa<VariableAST>(this)) {
		return llvm::dyn_cast<VariableAST>(this)->getType();
	}else if (llvm::isa<CastExprAST>(this)) {
		return llvm::dyn_cast<CastExprAST>(this)->getDestType();
	}else if (llvm::isa<ValueAST>(this)) {
		return llvm::dyn_cast<ValueAST>(this)->getType();
	}
	return Types(Type_null);
}


bool BaseAST::setType(Types type) {
	if (llvm::isa<VariableDeclAST>(this)) {
		return llvm::dyn_cast<VariableDeclAST>(this)->setType(type);
	}else if (llvm::isa<JumpStmtAST>(this)) {
		return llvm::dyn_cast<JumpStmtAST>(this)->setType(type);
	}else if (llvm::isa<BinaryExprAST>(this)) {
		return llvm::dyn_cast<BinaryExprAST>(this)->setType(type);
	}else if (llvm::isa<CallExprAST>(this)) {
		return llvm::dyn_cast<CallExprAST>(this)->setType(type);
	}else if (llvm::isa<VariableAST>(this)) {
		return llvm::dyn_cast<VariableAST>(this)->setType(type);
	}else if (llvm::isa<CastExprAST>(this)) {
		return llvm::dyn_cast<CastExprAST>(this)->setDestType(type);
	}else if (llvm::isa<ValueAST>(this)) {
		return llvm::dyn_cast<ValueAST>(this)->setType(type);
	}
	return false;
}
