#include "AST.hpp"


/**
  * デストラクタ
  */
TranslationUnitAST::~TranslationUnitAST(){
	for(int i=0; i<Prototypes.size(); i++){
		SAFE_DELETE(Prototypes[i]);
	}
	Prototypes.clear();

	for(int i=0; i<Functions.size(); i++){
		SAFE_DELETE(Functions[i]);
	}
	Functions.clear();
}


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
FunctionAST::~FunctionAST(){
	SAFE_DELETE(Proto);
	SAFE_DELETE(Body);
}


StatementsAST::~StatementsAST(){
	for(int i=0; i<Statements.size(); i++){
		SAFE_DELETE(Statements[i]);
	}
	for(int i=0; i<VarDecls.size(); i++){
		SAFE_DELETE(VarDecls[i]);
	}
	Statements.clear();
	VarDecls.clear();
}

/**
  * デストラクタ
  */
FunctionStmtAST::~FunctionStmtAST(){
	//delete statements
	for(int i=0; i<StmtLists.size(); i++){
		SAFE_DELETE(StmtLists[i]);
	}
	StmtLists.clear();
}


/**
  * デストラクタ
  */
CallExprAST::~CallExprAST(){
	for(int i=0; i<Args.size(); i++){
		SAFE_DELETE(Args[i]);
	}
}


Types BaseAST::getType() {
	if (!this) {
		fprintf(stderr, "error: Type is nothing\n");
		return Types(Type_null);
	}
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
	if (!this) {
		fprintf(stderr, "error: Type is nothing\n");
		return false;
	}
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
