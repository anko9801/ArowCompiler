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
	}else if (llvm::isa<CastAST>(this)) {
		return llvm::dyn_cast<CastAST>(this)->getType();
	}else if (llvm::isa<NumberAST>(this)) {
		return llvm::dyn_cast<NumberAST>(this)->getType();
	}else if (llvm::isa<BooleanAST>(this)) {
		return llvm::dyn_cast<BooleanAST>(this)->getType();
	}else if (llvm::isa<NoneAST>(this)) {
		return llvm::dyn_cast<NoneAST>(this)->getType();
	}
	return Types(Type_null);
}
