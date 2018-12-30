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


void BaseAST::printAST(int nest = 0){
	for(int i=0; i < nest; i++) fprintf(stderr, "	");
	if(llvm::isa<FunctionStmtAST>(this)){
		fprintf(stderr, "FunctionStatement\n");
		for (int i = 0;;i++)if (llvm::dyn_cast<FunctionStmtAST>(this)->getStatement(i))llvm::dyn_cast<FunctionStmtAST>(this)->getStatement(i)->printAST(nest+1);else break;
	}else if(llvm::isa<VariableDeclAST>(this))
		fprintf(stderr, "VariableDeclaration(%s)\n", llvm::dyn_cast<VariableDeclAST>(this)->getType().printType().c_str());
	else if(llvm::isa<CastExprAST>(this)){
		fprintf(stderr, "CastExpression(%s -> %s)\n", llvm::dyn_cast<CastExprAST>(this)->getSource()->getType().printType().c_str(), llvm::dyn_cast<CastExprAST>(this)->getDestType().printType().c_str());
		llvm::dyn_cast<CastExprAST>(this)->getSource()->printAST(nest+1);
	}else if(llvm::isa<BinaryExprAST>(this)){
		fprintf(stderr, "BinaryExpression(%s)\n", llvm::dyn_cast<BinaryExprAST>(this)->getType().printType().c_str());
		llvm::dyn_cast<BinaryExprAST>(this)->getLHS()->printAST(nest+1);
		llvm::dyn_cast<BinaryExprAST>(this)->getRHS()->printAST(nest+1);
	}else if(llvm::isa<CallExprAST>(this)){
		fprintf(stderr, "CallExpression(%s)\n", llvm::dyn_cast<CallExprAST>(this)->getType().printType().c_str());
		for (int i = 0;;i++)if (llvm::dyn_cast<CallExprAST>(this)->getArgs(i))llvm::dyn_cast<CallExprAST>(this)->getArgs(i)->printAST(nest+1);else break;
	}else if(llvm::isa<JumpStmtAST>(this)){
		fprintf(stderr, "JumpStatement(%s)\n", llvm::dyn_cast<JumpStmtAST>(this)->getType().printType().c_str());
		llvm::dyn_cast<JumpStmtAST>(this)->getExpr()->printAST(nest+1);
	}else if(llvm::isa<VariableAST>(this))
		fprintf(stderr, "Variable(%s)\n", llvm::dyn_cast<VariableAST>(this)->getType().printType().c_str());
	else if(llvm::isa<IfExprAST>(this)){
		fprintf(stderr, "IfExpression\n");
		llvm::dyn_cast<IfExprAST>(this)->getCond()->printAST(nest);
		for (int i = 0;;i++)if (llvm::dyn_cast<IfExprAST>(this)->getThen(i))llvm::dyn_cast<IfExprAST>(this)->getThen(i)->printAST(nest+1);else break;
		for (int i = 0;;i++)if (llvm::dyn_cast<IfExprAST>(this)->getElse(i))llvm::dyn_cast<IfExprAST>(this)->getElse(i)->printAST(nest+1);else break;
	}
	else if(llvm::isa<WhileExprAST>(this)) {
		fprintf(stderr, "WhileExpression\n");
		llvm::dyn_cast<WhileExprAST>(this)->getCond()->printAST(nest);
		for (int i = 0;;i++)if (llvm::dyn_cast<WhileExprAST>(this)->getLoop(i))llvm::dyn_cast<WhileExprAST>(this)->getLoop(i)->printAST(nest+1);else break;
	}
	else if(llvm::isa<ValueAST>(this))
		fprintf(stderr, "Value(%s)\n", llvm::dyn_cast<ValueAST>(this)->getType().printType().c_str());
	else
		fprintf(stderr, "unknown\n");
}