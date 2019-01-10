#ifndef SEMANTICS_HPP
#define SEMANTICS_HPP

#include<algorithm>
#include<cstdio>
#include<cstdlib>
#include<string>
#include<vector>
#include"APP.hpp"
#include"AST.hpp"
#include"lexer.hpp"
#include"parser.hpp"

typedef class Semantics {
	private:
		TranslationUnitAST *TU;
		std::vector<PrototypeAST*> PrototypeTable;
		std::vector<FunctionAST*> FunctionTable;

	public:
		Semantics(Parser parser);
		~Semantics(){SAFE_DELETE(TU);}
		bool doAnalysis();
		bool analysisStatement(BaseAST stmt);

}Semantics;

#endif