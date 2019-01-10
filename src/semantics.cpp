#include "semantics.hpp"

Semantics::Semantics(Parser *parser) {
	TU = parser->TU;
	PrototypeTable = parser->PrototypeTable;
}

bool Semantics::doAnalysis() {
	for(int i=0; ; i++) {
		FunctionAST *func = TU->getFunction(i);
		if(!func)
			break;
		PrototypeAST *proto = func->getPrototype();
		fprintf(stderr, "%s(", proto->getName().c_str());
		for (int i = 0; ;i++)
			if (proto->getParamType(i) == Types(Type_null)) break;
			else fprintf(stderr, "%s -> ", proto->getParamType(i).printType().c_str()); 
		fprintf(stderr, "%s)\n", proto->getType().printType().c_str());
		for (int i = 0; ;i++)
			if (func->getBody()->getStatement(i)) {
				analysisStatement(func->getBody()->getStatement(i));
				func->getBody()->getStatement(i)->printAST(1);
			}
			else break;
	}
	return true;
}

bool Semantics::analysisStatement(BaseAST stmt) {
	if(llvm::isa<VariableDeclAST>(stmt)) {	
	}else if(llvm::isa<CastExprAST>(stmt)) {
	}else if(llvm::isa<BinaryExprAST>(stmt)) {
	}else if(llvm::isa<CallExprAST>(stmt)) {
	}else if(llvm::isa<JumpStmtAST>(stmt)) {
	}else if(llvm::isa<VariableAST>(stmt)) {
	}else if(llvm::isa<IfStmtAST>(stmt)) {
	}else if(llvm::isa<WhileStmtAST>(stmt)) {
	}else if(llvm::isa<ValueAST>(stmt)) {
	}else if(llvm::isa<PlaceholderAST>(stmt)) {
	}else{}
}