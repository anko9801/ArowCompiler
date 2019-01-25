#include "AST.hpp"

std::string Types::printType() {
	std::string type;
	if (this->getPrimType() == Type_int)
		type = "int";
	else if (this->getPrimType() == Type_uint)
		type = "uint";
	else if (this->getPrimType() == Type_bool)
		type = "bool";
	else if (this->getPrimType() == Type_float)
		type = "float";
	else if (this->getPrimType() == Type_char)
		type = "char";
	else if (this->getPrimType() == Type_null)
		type = "null";
	else if (this->getPrimType() == Type_number)
		type = "number";
	else if (this->getPrimType() == Type_all)
		type = "all";
	else if (this->getPrimType() == Type_void)
		type = "void";
	else
		type = "yannaiyo";
	type += std::to_string(this->getBits());
	if (this->getNonNull()) type += "?";
	return type;
}

bool Types::operator== (const Types &rhs) const {
	if (Type == Type_all || rhs.Type == Type_all) return true;
	if (Type == Type_none || rhs.Type == Type_none) return true;
	if (Type == Type_number && (rhs.Type == Type_int || rhs.Type == Type_uint || rhs.Type == Type_float || rhs.Type == Type_bool)) return true;
	if ((Type == Type_int || Type == Type_uint || Type == Type_float || Type == Type_bool) && rhs.Type == Type_number) return true;
	if (Type == rhs.Type) return true;
	else return false;
}

bool Types::operator!= (const Types &rhs) const {
	if (Type == Type_all || rhs.Type == Type_all) return false;
	if (Type == Type_none || rhs.Type == Type_none) return false;
	if (Type == Type_number && (rhs.Type == Type_int || rhs.Type == Type_uint || rhs.Type == Type_float)) return false;
	if ((Type == Type_int || Type == Type_uint || Type == Type_float) && rhs.Type == Type_number) return false;
	if (Type == rhs.Type) return false;
	return !(Type == rhs.Type);
}

bool Seq::operator< (const Seq &rhs) const {
	if (Name < rhs.Name) return true;
	else return false;
}

bool Seq::operator== (const Seq &rhs) const {
	if (Type == rhs.Type && Name == rhs.Name) return true;
	else return false;
}


Types BaseAST::getType() {
	if (llvm::isa<VariableDeclAST>(this)) {
		return llvm::dyn_cast<VariableDeclAST>(this)->getType();
	}else if (llvm::isa<JumpStmtAST>(this)) {
		return llvm::dyn_cast<JumpStmtAST>(this)->getType();
	}else if (llvm::isa<BinaryExprAST>(this)) {
		return llvm::dyn_cast<BinaryExprAST>(this)->getType();
	}else if (llvm::isa<SingleExprAST>(this)) {
		return llvm::dyn_cast<SingleExprAST>(this)->getType();
	}else if (llvm::isa<CallExprAST>(this)) {
		return llvm::dyn_cast<CallExprAST>(this)->getType();
	}else if (llvm::isa<VariableAST>(this)) {
		return llvm::dyn_cast<VariableAST>(this)->getType();
	}else if (llvm::isa<CastExprAST>(this)) {
		return llvm::dyn_cast<CastExprAST>(this)->getType();
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
	}else if (llvm::isa<SingleExprAST>(this)) {
		return llvm::dyn_cast<SingleExprAST>(this)->setType(type);
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


void BaseAST::printAST(int nest = 0) {
	for (int i=0; i < nest; i++) fprintf(stderr, "	");
	if (llvm::isa<FunctionStmtAST>(this)) {
		fprintf(stderr, "FunctionStatement\n");
		for (int i = 0;;i++)if (llvm::dyn_cast<FunctionStmtAST>(this)->getStatement(i))llvm::dyn_cast<FunctionStmtAST>(this)->getStatement(i)->printAST(nest+1);else break;
	}else if (llvm::isa<VariableDeclAST>(this))
		fprintf(stderr, "VariableDeclaration(%s)\n", llvm::dyn_cast<VariableDeclAST>(this)->getType().printType().c_str());
	else if (llvm::isa<CastExprAST>(this)) {
		fprintf(stderr, "Cast(%s -> %s)\n", llvm::dyn_cast<CastExprAST>(this)->getSource()->getType().printType().c_str(), llvm::dyn_cast<CastExprAST>(this)->getDestType().printType().c_str());
		llvm::dyn_cast<CastExprAST>(this)->getSource()->printAST(nest+1);
	}else if (llvm::isa<BinaryExprAST>(this)) {
		fprintf(stderr, "Binary %s(%s)\n", llvm::dyn_cast<BinaryExprAST>(this)->getOp().c_str(), llvm::dyn_cast<BinaryExprAST>(this)->getType().printType().c_str());
		llvm::dyn_cast<BinaryExprAST>(this)->getLHS()->printAST(nest+1);
		llvm::dyn_cast<BinaryExprAST>(this)->getRHS()->printAST(nest+1);
	}else if (llvm::isa<SingleExprAST>(this)) {
		fprintf(stderr, "Single %s(%s)\n", llvm::dyn_cast<SingleExprAST>(this)->getOp().c_str(), llvm::dyn_cast<SingleExprAST>(this)->getType().printType().c_str());
		llvm::dyn_cast<SingleExprAST>(this)->getLHS()->printAST(nest+1);
	}else if (llvm::isa<CallExprAST>(this)) {
		fprintf(stderr, "CallFunction(");
		for (int i = 0; ;i++)
			if (llvm::dyn_cast<CallExprAST>(this)->getProto()->getParamType(i) == Types(Type_null)) break;
			else fprintf(stderr, "%s -> ", llvm::dyn_cast<CallExprAST>(this)->getProto()->getParamType(i).printType().c_str()); 
		fprintf(stderr, "%s)\n", llvm::dyn_cast<CallExprAST>(this)->getProto()->getType().printType().c_str());
		for (int i = 0;;i++)if (llvm::dyn_cast<CallExprAST>(this)->getArgs(i))llvm::dyn_cast<CallExprAST>(this)->getArgs(i)->printAST(nest+1);else break;
	}else if (llvm::isa<JumpStmtAST>(this)) {
		fprintf(stderr, "Return(%s)\n", llvm::dyn_cast<JumpStmtAST>(this)->getType().printType().c_str());
		llvm::dyn_cast<JumpStmtAST>(this)->getExpr()->printAST(nest+1);
	}else if (llvm::isa<VariableAST>(this))
		fprintf(stderr, "Variable(%s)\n", llvm::dyn_cast<VariableAST>(this)->getType().printType().c_str());
	else if (llvm::isa<IfStmtAST>(this)) {
		fprintf(stderr, "If ");
		llvm::dyn_cast<IfStmtAST>(this)->getCond()->printAST(nest-1);
		for (int i = 0;;i++)if (llvm::dyn_cast<IfStmtAST>(this)->getThen(i))llvm::dyn_cast<IfStmtAST>(this)->getThen(i)->printAST(nest+1);else break;
		for (int i = 0;;i++)if (llvm::dyn_cast<IfStmtAST>(this)->getElse(i))llvm::dyn_cast<IfStmtAST>(this)->getElse(i)->printAST(nest+1);else break;
	}
	else if (llvm::isa<WhileStmtAST>(this)) {
		fprintf(stderr, "While ");
		llvm::dyn_cast<WhileStmtAST>(this)->getCond()->printAST(nest-1);
		for (int i = 0;;i++)if (llvm::dyn_cast<WhileStmtAST>(this)->getLoop(i))llvm::dyn_cast<WhileStmtAST>(this)->getLoop(i)->printAST(nest+1);else break;
	}
	else if (llvm::isa<ValueAST>(this))
		fprintf(stderr, "Value(%s)\n", llvm::dyn_cast<ValueAST>(this)->getType().printType().c_str());
	else if (llvm::isa<PlaceholderAST>(this))
		fprintf(stderr, "Placeholder\n");
	else
		fprintf(stderr, "unknown\n");
}


/**
  * デストラクタ
  */
TranslationUnitAST::~TranslationUnitAST() {
	for (size_t i = 0;i < Prototypes.size(); i++) {
		SAFE_DELETE(Prototypes[i]);
	}
	Prototypes.clear();

	for (size_t i = 0;i < Functions.size(); i++) {
		SAFE_DELETE(Functions[i]);
	}
	Functions.clear();

	for (size_t i = 0;i < Imports.size(); i++) {
		SAFE_DELETE(Imports[i]);
	}
	Imports.clear();
}


FunctionAST::~FunctionAST() {
	SAFE_DELETE(Proto);
	if (Body)
		SAFE_DELETE(Body);
}


StatementsAST::~StatementsAST() {
	for (size_t i = 0;i < Statements.size();i++) {
		SAFE_DELETE(Statements[i]);
	}
	Statements.clear();
}


BinaryExprAST::~BinaryExprAST() {
	SAFE_DELETE(LHS);
	SAFE_DELETE(RHS);
}

SingleExprAST::~SingleExprAST() {
	SAFE_DELETE(LHS);
}