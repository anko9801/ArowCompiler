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


