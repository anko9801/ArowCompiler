#include "parser.hpp"

bool Debbug = false;

Parser::Parser(std::string filename) {
	Tokens = LexicalAnalysis(filename);
}

/**
  * 構文解析
  * @return 解析成功：true　解析失敗：false
  */
bool Parser::doParse() {
	if (!Tokens) {
		fprintf(stderr, "error at lexer\n");
		return false;
	}else{
		TU = new TranslationUnitAST();
		return visitModule();
	}
}


/**
  * AST取得
  * @return TranslationUnitへの参照
  */
TranslationUnitAST &Parser::getAST() {
	if (TU)
		return *TU;
	else
		return *(new TranslationUnitAST());
}


/**
  * Module用構文解析メソッド
  * @return 解析成功：true　解析失敗：false
  */
bool Parser::visitModule() {
	//ExternalDecl
	while (true) {
		if (!visitExternalDeclaration(TU)) {
			SAFE_DELETE(TU);
			return false;
		}
		if (isExpectedToken(TOK_EOF))
			break;
	}
	return true;
}

bool Parser::visitImportFile() {
	if (!isExpectedToken("import"))
		return false;
	Tokens->getNextToken();

	if (Debbug) fprintf(stderr, "%s:%d:%d: %s %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, __func__, Tokens->getCurString().c_str());

	if (!isExpectedToken(TOK_IDENTIFIER))
		return false;
	std::string filename = Tokens->getCurString();
	Tokens->getNextToken();

	if (!isExpectedToken("."))
		return false;
	Tokens->getNextToken();
	
	if (isExpectedToken("arow")) {
		Tokens->getNextToken();

		TokenStream *Tokens_old = Tokens;

		Tokens = LexicalAnalysis(filename + ".arow");
		visitModule();
		Tokens = Tokens_old;
	}else if (isExpectedToken("ll")) {
		Tokens->getNextToken();
		TU->addImport(new ImportAST(filename + ".ll"));
	}else{
		return false;
	}
	if (isExpectedToken(";") || isExpectedToken("\n")) {
		Tokens->getNextToken();
		return true;
	}
	return false;
}


/**
  * ExternalDeclaration用構文解析クラス
  * 解析したPrototyupeとFunctionASTをTranslationUitに追加
  * @param TranslationUnitAST
  * @return true 
  */
bool Parser::visitExternalDeclaration(TranslationUnitAST *tunit) {
	//FunctionDeclaration
	PrototypeAST *proto = visitFunctionDeclaration();
	if (proto) {
		tunit->addPrototype(proto);
		return true;
	}
	
	//FunctionDefinition
	FunctionAST *func_def = visitFunctionDefinition();
	if (func_def) {
		tunit->addFunction(func_def);
		return true;
	}

	if (visitImportFile())
		return true;

	return false;
}


/**
  * FunctionDeclaration用構文解析メソッド
  * @return 解析成功：PrototypeAST　解析失敗：NULL
  */
PrototypeAST *Parser::visitFunctionDeclaration() {
	int bkup = Tokens->getCurIndex();

	PrototypeAST *proto = visitPrototype();
	if (!proto)
		return NULL;

	//prototype;
	if (isExpectedToken(";") || isExpectedToken("\n")) {
		PrototypeTable.push_back(proto);
		Tokens->getNextToken();
		return proto;
	}else{
		SAFE_DELETE(proto);
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
}


/**
  * FunctionDefinition用構文解析メソッド
  * @return 解析成功：FunctionAST　解析失敗：NULL
  */
FunctionAST *Parser::visitFunctionDefinition() {
	int bkup = Tokens->getCurIndex();

	PrototypeAST *proto = visitPrototype();
	if (!proto)
		return NULL;

	FunctionAST *func = new FunctionAST(proto);
	for (size_t i = 0;i < FunctionTable.size();i++) {
		if (FunctionTable[i] == func) {
			fprintf(stderr, "Function：%s is redefined\n", proto->getName().c_str());
			SAFE_DELETE(proto);
			return NULL;
		}
	}

	//VariableTable.clear();
	FunctionTable.push_back(func);
	FunctionStmtAST *func_stmt = visitFunctionStatement(proto);
	if (func_stmt) {
		return new FunctionAST(proto, func_stmt);
	}else{
		SAFE_DELETE(proto);
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
}


/**
  * Prototype用構文解析メソッド
  * @return 解析成功：PrototypeAST　解析失敗：NULL
  */
PrototypeAST *Parser::visitPrototype() {
	Types func_type;
	std::string func_name;
	Types var_type;

	//bkup index
	int bkup = Tokens->getCurIndex();

	//type_specifier
	func_type = visitTypes();
	if (func_type.getPrimType() == Type_null) {
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}

	//IDENTIFIER
	if (!isExpectedToken(TOK_IDENTIFIER)) {
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	func_name = Tokens->getCurString();
	Tokens->getNextToken();

	//'('
	if (!isExpectedToken("(")) {
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();

	//parameter_list
	std::vector<Seq> param_list;
	Types param_type;

	if (!isExpectedToken(")")) {
		param_type = visitTypes();
		if (param_type == Types(Type_null)) {
			fprintf(stderr, "%s:%d:%d: error: type is nothing\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}

		if (!isExpectedToken(TOK_IDENTIFIER)) {
			fprintf(stderr, "%s:%d:%d: error: expected identifer but %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
		param_list.push_back(Seq(param_type, Tokens->getCurString()));
		Tokens->getNextToken();

		while (true) {
			if (isExpectedToken(")"))
				break;

			if (!isExpectedToken(",")) {
				fprintf(stderr, "%s:%d:%d: error: expected identifer but %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}
			Tokens->getNextToken();

			param_type = visitTypes();
			if (param_type == Types(Type_null)) {
				fprintf(stderr, "%s:%d:%d: error: argument type is nothing\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}

			if (!isExpectedToken(TOK_IDENTIFIER)) {
				fprintf(stderr, "%s:%d:%d: error: expected identifer but %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}

			//引数の変数名に被りがないか確認
			for (size_t i = 0;i < param_list.size();i++) {
				if (param_list[i].Name == Tokens->getCurString()) {
					fprintf(stderr, "%s:%d:%d: error: argument name is same\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
					Tokens->applyTokenIndex(bkup);
					return NULL;
				}
			}
			param_list.push_back(Seq(param_type, Tokens->getCurString()));
			Tokens->getNextToken();
		}
	}
	Tokens->getNextToken();

	PrototypeAST *proto = new PrototypeAST(func_type, func_name, param_list);
	for (size_t i = 0;i < PrototypeTable.size();i++) {
		if (PrototypeTable[i] == proto) {
			fprintf(stderr, "Prototype %s is redefined\n", proto->getName().c_str());
			SAFE_DELETE(proto);
			return NULL;
		}
	}
	FunctionAST *func = new FunctionAST(proto);
	for (size_t i = 0;i < FunctionTable.size();i++) {
		if (FunctionTable[i] == func) {
			fprintf(stderr, "Function %s is redefined\n", proto->getName().c_str());
			SAFE_DELETE(proto);
			return NULL;
		}
	}
	return proto;
}


/**
  * FunctionStatement用構文解析メソッド
  * @param 勘数名，引数を格納したPrototypeクラスのインスタンス
  * @return 解析成功：FunctionSmtAST　解析失敗：NULL
  */
FunctionStmtAST *Parser::visitFunctionStatement(PrototypeAST *proto) {
	int bkup = Tokens->getCurIndex();

	//create FunctionStatement
	FunctionStmtAST *func_stmt = new FunctionStmtAST();

	//add parameter to FunctionStatement
	VariableDeclAST *vdecl;
	SetInsertPoint(func_stmt->getStatements());
	for (size_t i = 0;i < proto->getParamSize(); i++) {
		vdecl = new VariableDeclAST(proto->getParamType(i), proto->getParamName(i));
		addStatement(vdecl);
	}
	if (Debbug) fprintf(stderr, "%s:%d:%d: %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());

	setFuncType(proto->getType());

	visitStatements(func_stmt->getStatements(), InsertPoint->getVars());
	StatementsAST *stmts = func_stmt->getStatements();
	if (warning) return func_stmt;
	BaseAST *last_stmt = stmts->getStatement(stmts->getSize()-1);

	//check if last statement is jump_statement
	if (!last_stmt) {
		warning = true;
		Tokens->applyTokenIndex(bkup);
		return func_stmt;
	}
	if (!llvm::isa<JumpStmtAST>(last_stmt)) {
		fprintf(stderr, "%s:%d:%d: warning: end of statement is not return statement\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
		warning = true;
		stmts->addStatement(new JumpStmtAST(new ValueAST(0, getFuncType())));
	}

	return func_stmt;
}


/**
  * Statements用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
StatementsAST *Parser::visitStatements(StatementsAST* insert, std::vector<VariableDeclAST*> vars) {
	int bkup = Tokens->getCurIndex();
	StatementsAST *stmts = new StatementsAST();
	BaseAST *stmt;
	if (Debbug) fprintf(stderr, "%s:%d:%d: %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
	insert->setOldVars(vars);
	SetInsertPoint(insert);

	if (!isExpectedToken("{")) {
		stmt = visitStatement();
		if (stmt) {
			SetInsertPoint(insert);
			addStatement(stmt);
			stmts->addStatement(stmt);
			return stmts;
		}
		if (!isExpectedToken("{")) {
			Tokens->applyTokenIndex(bkup);
			// SAFE_DELETE(insert);
			return stmts;
		}
	}
	Tokens->getNextToken();

	while (true) {
		if (isExpectedToken("}")) {
			Tokens->getNextToken();
			break;
		}
		stmt = visitStatement();
		if (stmt) {
			SetInsertPoint(insert);
			addStatement(stmt);
			stmts->addStatement(stmt);
			continue;
		}else{
			if (Debbug) fprintf(stderr, "%s:%d:%d: unknown %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
			Tokens->applyTokenIndex(bkup);
			stmts->clear();
			// SAFE_DELETE(insert);
			warning = true;
			return stmts;
		}
	}
	return stmts;
}


/**
  * Types用構文解析メソッド
  * @return 解析成功：Types　解析失敗：Type_null
  */
Types Parser::visitTypes() {
	int bkup = Tokens->getCurIndex();

	int bits;
	prim_type Type;
	bool non_null = false;

	// primary type
	if (!isExpectedToken(TOK_TYPE)) {
		Tokens->applyTokenIndex(bkup);
		return Types(Type_null);
	}
	Type = str2Type(Tokens->getCurString());
	Tokens->getNextToken();

	// bit数
	if (isExpectedToken(TOK_DIGIT)) {
		bits = Tokens->getCurNumVal();
		Tokens->getNextToken();
	}else{
		bits = 32;
		if (Type == Type_bool)
			bits = 1;
		if (Type == Type_void)
			bits = 0;
	}

	// non-null type
	if (isExpectedToken("?")) {
		non_null = true;
		Tokens->getNextToken();
	}

	if (Debbug) fprintf(stderr, "%s:%d:%d: %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
	return Types(Type, bits, non_null);
}


/**
  * Statement用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitStatement() {
	int bkup = Tokens->getCurIndex();
	BaseAST *stmt;

	// if文
	if (isExpectedToken(TOK_IF)) {
		stmt = visitIfStatement();
		return stmt;
	}
	// while文
	if (isExpectedToken(TOK_WHILE)) {
		stmt = visitWhileStatement();
		return stmt;
	}
	// match文
	if (isExpectedToken(TOK_MATCH)) {
		stmt = visitMatchExpression();
		return stmt;
	}
	// return文
	if (isExpectedToken(TOK_RETURN)) {
		stmt = visitJumpStatement();
		if (!stmt || stmt->getType() != getFuncType()) {
			fprintf(stderr, "%s:%d:%d: error: return type is expected %s but %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, getFuncType().printType().c_str(), stmt->getType().printType().c_str());
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
		return stmt;
	}
	// 変数宣言
	stmt = visitVariableDeclaration();
	if (stmt) {
		VariableDeclAST* var_decl = (VariableDeclAST*)stmt;
		var_decl->setDeclType(VariableDeclAST::local);
		if (Debbug) fprintf(stderr, "%s:%d:%d: %s %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, var_decl->getType().printType().c_str(), var_decl->getName().c_str());
		VariableTable.push_back(var_decl);

		if (isExpectedToken("=")) {
			Tokens->ungetToken(1);
			return var_decl;
		}
		return stmt;
	}
	// 変数代入か式
	stmt = visitAssignmentExpression(Types(Type_all));
	if (stmt) return stmt;

	return NULL;
}


/**
  * VariableDeclaration用構文解析メソッド
  * @return 解析成功：VariableDeclAST　解析失敗：NULL
  */
VariableDeclAST *Parser::visitVariableDeclaration() {
	int bkup = Tokens->getCurIndex();
	std::string name;

	Types type = visitTypes();
	if (type.getPrimType() == Type_null) {
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}

	//IDENTIFIER
	if (!isExpectedToken(TOK_IDENTIFIER)) {
		Tokens->applyTokenIndex(bkup);
		fprintf(stderr, "%s:%d:%d: error: cannot find identifier of variable\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
		return NULL;
	}

	name = Tokens->getCurString();
	Tokens->getNextToken();

	//配列かどうか
	if (isExpectedToken("[")) {
		Tokens->getNextToken();
		if (!isExpectedToken(TOK_DIGIT)) {
			fprintf(stderr, "%s:%d:%d: error: cannot find index\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}

		type.setArray(Tokens->getCurNumVal());

		Tokens->getNextToken();
		if (!isExpectedToken("]")) {
			fprintf(stderr, "%s:%d:%d: error: cannot find right palen\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
		Tokens->getNextToken();
	}

	for (size_t i = 0;;i++) {
		if (!InsertPoint->getOldVar(i)) break;
		if (InsertPoint->getOldVar(i)->getName() == name) {
			fprintf(stderr, "%s:%d:%d: error: redefinition of '%s' with a different type: '%s' vs '%s'\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, VariableTable[i]->getType().printType().c_str(), VariableTable[i]->getType().printType().c_str(), type.printType().c_str());
			return NULL;
		}
	}

	for (size_t i = 0;;i++) {
		if (!InsertPoint->getNewVar(i)) break;
		if (InsertPoint->getNewVar(i)->getName() == name) {
			fprintf(stderr, "%s:%d:%d: error: redefinition of '%s' with a different type: '%s' vs '%s'\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, VariableTable[i]->getType().printType().c_str(), VariableTable[i]->getType().printType().c_str(), type.printType().c_str());
			return NULL;
		}
	}

	//式が終わったなら次の文、AssignもあるならFuncStmtで一回ループ回った後にAssignmentExpression関数を呼び出す
	//';'
	if (isExpectedToken(";") || isExpectedToken("\n")) {
		Tokens->getNextToken();
		return new VariableDeclAST(type, name);
	//'='
	}else if (isExpectedToken("=") || isExpectedToken(",")) {
		return new VariableDeclAST(type, name);
	}else{
		fprintf(stderr, "%s:%d:%d: unknown syntax\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
}


/**
  * JumpStatement用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitJumpStatement() {
	int bkup = Tokens->getCurIndex();
	BaseAST *expr;

	if (!isExpectedToken(TOK_RETURN))
		return NULL;
	Tokens->getNextToken();

	expr = visitAssignmentExpression(getFuncType());
	if (!expr) {
		if (Debbug) fprintf(stderr, "%s:%d:%d: return %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	// 暗黙の型変換
	expr = visitImplicitCastNumber(expr, getFuncType());
	if (!expr) {
		fprintf(stderr, "%s:%d:%d: error: no match for type\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	return new JumpStmtAST(expr);
}


/**
  * IfStatement用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitIfStatement() {
	int bkup = Tokens->getCurIndex();

	BaseAST *CondStmt;

	if (!isExpectedToken(TOK_IF)) {
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();

	CondStmt = visitCondition(NULL, Types(Type_all));
	auto vars = InsertPoint->getVars();

	// is キャストが働いた時
	if (llvm::isa<CastExprAST>(CondStmt)) {
		if (Debbug) fprintf(stderr, "%s:%d:%d: is cast\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
		auto iscast = llvm::dyn_cast<CastExprAST>(CondStmt);
		if (iscast->getNestin() && llvm::isa<VariableAST>(iscast->getSource())) {
			auto source = llvm::dyn_cast<VariableAST>(iscast->getSource());
			for (size_t i = 0;i < vars.size();i++) {
				if (vars[i]->getName() == source->getName()) {
					if (Debbug) fprintf(stderr, "%s:%d:%d: is cast accepted\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
					vars[i]->setType(iscast->getDestType());
				}
			}
		}
	}

	if (!CondStmt || CondStmt->getType() != Types(Type_bool, 1, true)) {
		fprintf(stderr, "%s:%d:%d: error: if condition is not bool\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	IfStmtAST *if_expr = new IfStmtAST(CondStmt);

	visitStatements(if_expr->getThens(), vars);

	if (!isExpectedToken("else")) {
		return if_expr;
	}
	Tokens->getNextToken();

	visitStatements(if_expr->getElses(), InsertPoint->getVars());

	return if_expr;
}


/**
  * WhileStatement用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitWhileStatement() {
	int bkup = Tokens->getCurIndex();

	BaseAST *CondStmt;
	if (Debbug) fprintf(stderr, "%s:%d:%d: %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
	if (!isExpectedToken(TOK_WHILE)) {
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();

	CondStmt = visitCondition(NULL, Types(Type_all));
	if (Debbug) fprintf(stderr, "%s:%d:%d: %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
	if (!CondStmt || CondStmt->getType() != Types(Type_bool)) {
		fprintf(stderr, "%s:%d:%d: error: while condition is not bool\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	WhileStmtAST *while_expr = new WhileStmtAST(CondStmt);

	visitStatements(while_expr->getLoops(), InsertPoint->getVars());
	if (Debbug) fprintf(stderr, "%s:%d:%d: %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
	return while_expr;
}


/**
  * MatchExpression用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitMatchExpression() {
	int bkup = Tokens->getCurIndex();

	if (Debbug) fprintf(stderr, "%s:%d:%d: %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
	if (!isExpectedToken(TOK_MATCH)) {
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();

	BaseAST *Eval = visitAssignmentExpression(Types(Type_all));
	if (!Eval) {
		fprintf(stderr, "%s:%d:%d: error: eval is nothing\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}

	if (!isExpectedToken("{")) {
		fprintf(stderr, "%s:%d:%d: expected '{' but %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();

	IfStmtAST *if_expr = visitPatternExpression(Eval);
	if (!if_expr) {
		fprintf(stderr, "%s:%d:%d: expected pattern but %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}

	Tokens->getNextToken();

	if (Debbug) fprintf(stderr, "%s:%d:%d: %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
	return if_expr;
}



IfStmtAST *Parser::visitPatternExpression(BaseAST *Eval) {
	int bkup = Tokens->getCurIndex();

	BaseAST *eval = visitExpression(NULL, Types(Type_all));
	if (!eval) {
		fprintf(stderr, "%s:%d:%d: error: expected eval but %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}

	eval = visitImplicitCastNumber(eval, Eval->getType());
	if (!Eval || !eval) {
		fprintf(stderr, "%s:%d:%d: error: no match for match statement type\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}

	IfStmtAST *if_expr;
	if_expr = new IfStmtAST(new BinaryExprAST("==", Eval, eval, Types(Type_bool)));

	if (!isExpectedToken("=>")) {
		fprintf(stderr, "%s:%d:%d: error: expected '=>' but %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();
	if (!isExpectedToken(",")) {
		StatementsAST *stmts = new StatementsAST();
		visitStatements(stmts, InsertPoint->getVars());
		if (!stmts) {
			fprintf(stderr, "%s:%d:%d: error: statement is nothing\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
		for (int i = 0;;i++)
			if (!stmts->getStatement(i)) break;
			else if_expr->addThen(stmts->getStatement(i));
	
		if (!isExpectedToken(",")) {
			Tokens->getNextToken();
			return if_expr;
		}
	}
	Tokens->getNextToken();
	
	IfStmtAST *pattern = visitPatternExpression(Eval);
	if (!pattern) {
		Tokens->getNextToken();
		return if_expr;
	}
	if_expr->addElse(pattern);
	return if_expr;
}


/**
  * AssignmentExpression用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitAssignmentExpression(Types type) {
	int bkup = Tokens->getCurIndex();

	//	| IDENTIFIER '=' additive_expression
	BaseAST *lhs;

	if (isExpectedToken(TOK_IDENTIFIER)) {
		//変数が宣言されているか確認
		bool all_confirm = false;
		for (size_t i = 0;;i++) {
			if (!InsertPoint->getNewVar(i)) break;
			if (InsertPoint->getNewVar(i)->getName() == Tokens->getCurString()) {
				Tokens->getNextToken();
				all_confirm = true;

				int Index = -1;
				if (isExpectedToken("[")) {
					Tokens->getNextToken();
					if (!isExpectedToken(TOK_DIGIT)) {
						fprintf(stderr, "%s:%d:%d: error: cannot find index\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
						Tokens->applyTokenIndex(bkup);
						return NULL;
					}
					Index = Tokens->getCurNumVal();
					Tokens->getNextToken();
					if (!isExpectedToken("]")) {
						fprintf(stderr, "%s:%d:%d: error: expected ']' but %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
						Tokens->applyTokenIndex(bkup);
						return NULL;
					}
					Tokens->getNextToken();
				}
				lhs = new VariableAST(InsertPoint->getNewVar(i), Index);
				BaseAST *rhs;
				if (isExpectedToken("=")) {
					Tokens->getNextToken();
					rhs = visitExpression(NULL, InsertPoint->getNewVar(i)->getType());
					rhs = visitImplicitCast(rhs, lhs->getType());
					if (rhs) {
						rhs = new BinaryExprAST("=", lhs, rhs, InsertPoint->getNewVar(i)->getType());
						if (isExpectedToken(";") || isExpectedToken("\n")) {
							Tokens->getNextToken();
							return rhs;
						}else{
							fprintf(stderr, "%s:%d:%d: error: expected rhs of = but %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
							Tokens->applyTokenIndex(bkup);
							return NULL;
						}
					}else{
						fprintf(stderr, "%s:%d:%d: error: expected rhs of = but %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
						Tokens->applyTokenIndex(bkup);
						return NULL;
					}

				}else if (isExpectedToken("+=") ||
						isExpectedToken("-=") ||
						isExpectedToken("*=") ||
						isExpectedToken("/=") ||
						isExpectedToken("%=") ||
						isExpectedToken("|=") ||
						isExpectedToken("&=")) {
					std::string op = {Tokens->getCurString()[0]};
					Tokens->getNextToken();
					rhs = visitExpression(NULL, InsertPoint->getNewVar(i)->getType());
					rhs = visitImplicitCastNumber(rhs, lhs->getType());
					if (rhs) {
						rhs = new BinaryExprAST(op, lhs, rhs, lhs->getType());
						rhs = new BinaryExprAST("=", lhs, rhs, InsertPoint->getNewVar(i)->getType());
						if (isExpectedToken(";") || isExpectedToken("\n")) {
							Tokens->getNextToken();
							return rhs;
						}else{
							fprintf(stderr, "%s:%d:%d: error: expected rhs of %s= but %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, op.c_str(), Tokens->getCurString().c_str());
							Tokens->applyTokenIndex(bkup);
							return NULL;
						}
					}else{
						fprintf(stderr, "%s:%d:%d: error: expected rhs of %s= but %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, op.c_str(), Tokens->getCurString().c_str());
						Tokens->applyTokenIndex(bkup);
						return NULL;
					}
				}else{
					Tokens->applyTokenIndex(bkup);
				}
			}
		}
		for (size_t i = 0;;i++) {
			if (!InsertPoint->getOldVar(i)) break;
			if (InsertPoint->getOldVar(i)->getName() == Tokens->getCurString()) {
				Tokens->getNextToken();
				all_confirm = true;

				int Index = -1;
				if (isExpectedToken("[")) {
					Tokens->getNextToken();
					if (!isExpectedToken(TOK_DIGIT)) {
						fprintf(stderr, "%s:%d:%d: error: cannot find index\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
						Tokens->applyTokenIndex(bkup);
						return NULL;
					}
					Index = Tokens->getCurNumVal();
					Tokens->getNextToken();
					if (!isExpectedToken("]")) {
						fprintf(stderr, "%s:%d:%d: error: expected ']' but %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
						Tokens->applyTokenIndex(bkup);
						return NULL;
					}
					Tokens->getNextToken();
				}
				lhs = new VariableAST(InsertPoint->getOldVar(i), Index);
				BaseAST *rhs;
				if (isExpectedToken("=")) {
					Tokens->getNextToken();

					rhs = visitExpression(NULL, InsertPoint->getOldVar(i)->getType());
					if (rhs) {
						rhs = visitImplicitCastNumber(rhs, lhs->getType());
						if (!rhs) {
							fprintf(stderr, "%s:%d:%d: error: no match for type\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
							Tokens->applyTokenIndex(bkup);
							return NULL;
						}
						if (!isExpectedToken(","))
							Tokens->getNextToken();
						return new BinaryExprAST("=", lhs, rhs, InsertPoint->getOldVar(i)->getType());
					}else{
						Tokens->applyTokenIndex(bkup);
					}
				}else{
					Tokens->applyTokenIndex(bkup);
				}
			}
		}
		if (!all_confirm)
			Tokens->applyTokenIndex(bkup);
	}else{
		Tokens->applyTokenIndex(bkup);
	}

	//additive_expression
	lhs = visitCondition(NULL, Types(Type_all));
	if (!lhs)
		return NULL;
	
	if (isExpectedToken(";") || isExpectedToken("\n")) {
		Tokens->getNextToken();
		return lhs;
	}

	return lhs;
}


/**
  * Condition用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitCondition(BaseAST *lhs, Types type) {
	int bkup = Tokens->getCurIndex();
	if (!lhs)
		lhs = visitExpression(NULL, type);
	if (!lhs)
		return NULL;
	
	BaseAST *rhs;

	if (isExpectedToken("||") ||
		isExpectedToken("&&")) {
		std::string op = Tokens->getCurString();
		Tokens->getNextToken();
		rhs = visitExpression(NULL, Types(Type_bool));
		if (rhs) {
			// 暗黙の型変換
			lhs = visitImplicitCastNumber(lhs, Types(Type_bool));
			rhs = visitImplicitCastNumber(rhs, Types(Type_bool));
			if (!lhs || !rhs) {
				fprintf(stderr, "%s:%d:%d: error: no match for type\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}
			type = Types(Type_bool);
			return visitExpression(new BinaryExprAST(op, lhs, rhs, type), type);
		}else{
			fprintf(stderr, "%s:%d:%d: error: expected rhs of %s but %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, op.c_str(), Tokens->getCurString().c_str());
			SAFE_DELETE(lhs);
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
	}

	return lhs;
}


/**
  * ConditionExpressionとAdditiveExpression用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitExpression(BaseAST *lhs, Types type) {
	int bkup = Tokens->getCurIndex();
	if (!lhs)
		lhs = visitAdditiveExpression(NULL, type);
	if (!lhs)
		return NULL;

	if (lhs && lhs->getType() == Types(Type_number)) {
		if (isExpectedToken("<") ||
			isExpectedToken(">") ||
			isExpectedToken("<=") ||
			isExpectedToken(">=") ||
			isExpectedToken("==") ||
			isExpectedToken("!=")) {
			std::string op = Tokens->getCurString();
			BaseAST *rhs;

			Tokens->getNextToken();
			rhs = visitAdditiveExpression(NULL, Types(Type_number));
			if (rhs) {
				lhs = visitImplicitCastNumber(lhs, rhs->getType());
				rhs = visitImplicitCastNumber(rhs, lhs->getType());
				if (!lhs || !rhs) {
					fprintf(stderr, "%s:%d:%d: error: no match for type\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
					Tokens->applyTokenIndex(bkup);
					return NULL;
				}
				return new BinaryExprAST(op, lhs, rhs, Types(Type_bool));
			}else{
				fprintf(stderr, "%s:%d:%d: error: expected rhs of %s but %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, op.c_str(), Tokens->getCurString().c_str());
				SAFE_DELETE(lhs);
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}
		}
	}
	return lhs;
}


/**
 * AdditiveExpression用構文解析メソッド
 * @param lhs(左辺),初回呼び出し時はNULL
 * @return 解析成功：AST　解析失敗：NULL
 */
BaseAST *Parser::visitAdditiveExpression(BaseAST *lhs, Types type = Types(Type_all)) {
	//bkup index
	int bkup = Tokens->getCurIndex();

	if (!lhs)
		lhs = visitMultiplicativeExpression(NULL, type);
	if (!lhs)
		return NULL;
	BaseAST *rhs;

	if (isExpectedToken("+") ||
		isExpectedToken("-") ||
		isExpectedToken("|") ||
		isExpectedToken("^")) {
		std::string op = Tokens->getCurString();
		Tokens->getNextToken();
		rhs = visitMultiplicativeExpression(NULL, type);
		if (rhs) {
			// 暗黙の型変換
			lhs = visitImplicitCastNumber(lhs, rhs->getType());
			if (!lhs) {
				fprintf(stderr, "%s:%d:%d: error: no match for type\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}
			rhs = visitImplicitCastNumber(rhs, lhs->getType());
			if (!rhs) {
				fprintf(stderr, "%s:%d:%d: error: no match for type\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}
			type = lhs->getType();
			return visitAdditiveExpression(new BinaryExprAST(op, lhs, rhs, type), type);
		}else{
			SAFE_DELETE(lhs);
			Tokens->applyTokenIndex(bkup);
			fprintf(stderr, "%s:%d:%d: error: expected rhs of %s but %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, op.c_str(), Tokens->getCurString().c_str());
			return NULL;
		}
	}
	return lhs;
}


/**
 * MultiplicativeExpression用解析メソッド
 * @param  lhs(左辺),初回呼び出し時はNULL
 * @return 解析成功：AST　解析失敗：NULL
 */
BaseAST *Parser::visitMultiplicativeExpression(BaseAST *lhs, Types type = Types(Type_all)) {
	//bkup index
	int bkup = Tokens->getCurIndex();

	if (!lhs)
		lhs = visitCastExpression();
	if (!lhs)
		return NULL;
	BaseAST *rhs;

	if (isExpectedToken("*") ||
		isExpectedToken("/") ||
		isExpectedToken("%") ||
		isExpectedToken("<<") ||
		isExpectedToken(">>") ||
		isExpectedToken("&")) {
		std::string op = Tokens->getCurString();
		Tokens->getNextToken();
		rhs = visitCastExpression();
		if (rhs) {
			// 暗黙の型変換
			lhs = visitImplicitCastNumber(lhs, rhs->getType());
			if (!lhs) {
				fprintf(stderr, "%s:%d:%d: error: no match for type\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}
			rhs = visitImplicitCastNumber(rhs, lhs->getType());
			if (!rhs) {
				fprintf(stderr, "%s:%d:%d: error: no match for type\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}
			// 型変換が行われなかった時
			type = lhs->getType();
			return visitMultiplicativeExpression(new BinaryExprAST(op, lhs, rhs, type), type);
		}else{
			fprintf(stderr, "%s:%d:%d: error: expected rhs of %s but %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, op.c_str(), Tokens->getCurString().c_str());
			SAFE_DELETE(lhs);
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
	}
	return lhs;
}


/**
  * CastExpression用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitCastExpression() {
	//bkup index
	int bkup = Tokens->getCurIndex();

	BaseAST *lhs;

	if (isExpectedToken("!")) {
		Tokens->getNextToken();
		lhs = visitPostfixExpression();
		if (lhs->getType() == Types(Type_bool)) {
			return new SingleExprAST("!", lhs, Types(Type_bool));
		}else{
			fprintf(stderr, "%s:%d:%d: error: ! bool value is nothing\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
	}

	lhs = visitPostfixExpression();
	if (!lhs)
		return NULL;

	if (isExpectedToken("as")) {
		Tokens->getNextToken();
		Types DestType = visitTypes();
		if (Debbug) fprintf(stderr, "%s:%d:%d: as %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, DestType.printType().c_str());
		if (DestType.getPrimType() == Type_null) {
			fprintf(stderr, "%s:%d:%d: error: unknown Type\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
		if (llvm::isa<ValueAST>(lhs)) {
			ValueAST *value = llvm::dyn_cast<ValueAST>(lhs);
			value->setType(DestType);
			return value;
		}
		return new CastExprAST(lhs, DestType, false);

	}else if (isExpectedToken("is")) {
		Tokens->getNextToken();
		Types DestType = visitTypes();
		if (Debbug) fprintf(stderr, "%s:%d:%d: is %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, DestType.printType().c_str());
		if (DestType.getPrimType() == Type_null) {
			fprintf(stderr, "%s:%d:%d: error: unknown Type\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
		return new CastExprAST(lhs, DestType, true);

	}else if (isExpectedToken("?")) {
		if (Debbug) fprintf(stderr, "%s:%d:%d: non-null\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
		lhs->getType().setNonNull(true);
		Tokens->getNextToken();
		if (!lhs->getType().getNonNull()) {
			IfStmtAST *nonif_stmt = new IfStmtAST(new BinaryExprAST("!=", lhs, new ValueAST(0, lhs->getType()), Types(Type_bool)));
			nonif_stmt->addElse(new JumpStmtAST(new ValueAST(0, getFuncType())));
			addStatement(nonif_stmt);
		}
		return new CastExprAST(lhs,
			Types(lhs->getType().getPrimType(), lhs->getType().getBits(), true),
			false);

	}else if (isExpectedToken("++") || isExpectedToken("--")) {
		Tokens->getNextToken();
		if (lhs && lhs->getType() == Types(Type_number)) {
			addStatement(new BinaryExprAST("=", lhs, new BinaryExprAST("+", lhs, new ValueAST(1, lhs->getType()), lhs->getType()), lhs->getType()));
			return lhs;
		}else{
			fprintf(stderr, "%s:%d:%d: error: ++ value is nothing\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
	}
		
	return lhs;
}


/**
  * ImplicitCastNumber用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitImplicitCastNumber(BaseAST *src, Types impl_type) {
	if (!src) return NULL;
	if (src->getType().getPrimType() == Type_number || src->getType().getPrimType() == Type_all) {
		if (impl_type.getPrimType() == Type_number) {
			src->setType(Types(Type_int));
		}else{
			src->setType(impl_type);
		}
		return src;
	}else{
		if (impl_type.getPrimType() == Type_number || impl_type.getPrimType() == Type_all)
			return src;
	}

	if (src->getType().getPrimType() == impl_type.getPrimType()) {
		if (src->getType().getBits() != impl_type.getBits())
			return new CastExprAST(src, Types(
				impl_type.getPrimType(),
				impl_type.getBits(),
				std::min(src->getType().getNonNull(), impl_type.getNonNull())
			));
		else return src;
	}else{
		return NULL;
	}
}


/**
  * ImplicitCastNonNull用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitImplicitCast(BaseAST *src, Types impl_type) {
	if (src->getType().getPrimType() == Type_number) {
		src->setType(impl_type);
		return src;
	}

	if (impl_type.getPrimType() == Type_number)
		return src;
	
	if (src->getType() == impl_type && src->getType().getBits() == impl_type.getBits() && src->getType().getNonNull() >= impl_type.getNonNull()) {
		return src;
	}
	return new CastExprAST(src, impl_type);
}


/**
  * PostfixExpression用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitPostfixExpression() {
	int bkup = Tokens->getCurIndex();

	//primary_expression
	BaseAST *prim_expr = visitPrimaryExpression();
	if (prim_expr)
		return prim_expr;

	// 関数呼び出し用
	//FUNCTION_IDENTIFIER
	if (!isExpectedToken(TOK_IDENTIFIER)) {
		fprintf(stderr, "%s:%d:%d: error: unknown Token: %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}

	//関数名取得
	std::string Callee = Tokens->getCurString();
	if (Debbug) fprintf(stderr, "%s:%d:%d: call function %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
	Tokens->getNextToken();

	//LEFT PALEN
	if (!isExpectedToken("(")) {
		fprintf(stderr, "%s:%d:%d: error: expected '(' but %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();
	
	//argument list
	std::vector<BaseAST*> args;

	// 引数の解析
	if (!isExpectedToken(")")) {
		BaseAST *assign_expr = visitAssignmentExpression(Types(Type_all));
		if (assign_expr) {
			args.push_back(assign_expr);
			while (isExpectedToken(",")) {
				Tokens->getNextToken();
				assign_expr = visitAssignmentExpression(Types(Type_all));
				if (assign_expr) args.push_back(assign_expr);
				else break;
			}
		}
	}

	//関数の名前の確認
	PrototypeAST *proto;
	for (int i = 0;;i++) {
		proto = getPrototype(i);
		if (!proto) break;
		if (proto->getName() == Callee) break;
	}
	if (!proto) {
		FunctionAST *func;
		for (int j = 0;;j++) {
			func = getFunction(j);
			if (!func) break;
			if (func->getName() == Callee) {
				proto = func->getPrototype();
				break;
			}
		}
	}
	if (!proto) {
		fprintf(stderr, "%s:%d:%d: error: undefined function %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Callee.c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Types func_type = proto->getType();

	if (proto->getParamSize() != args.size()) {
		fprintf(stderr, "%s:%d:%d: error: no match for function param size found %zu but %lu in '%s'\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, proto->getParamSize(), args.size(), Callee.c_str());
		for (size_t j = 0;j < args.size();j++)
			SAFE_DELETE(args[j]);
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	// 引数の型の確認
	for (size_t i = 0;i < proto->getParamSize();i++) {
		// 暗黙の型変換
		args[i] = visitImplicitCast(args[i], proto->getParamType(i));
		if (proto->getParamType(i) != args[i]->getType() ||
			proto->getParamType(i).getBits() != args[i]->getType().getBits() ||
			proto->getParamType(i).getNonNull() > args[i]->getType().getNonNull()) {
			fprintf(stderr, "%s:%d:%d: error: no match for function param found %s but %s in '%s'\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, proto->getParamType(i).printType().c_str(), args[i]->getType().printType().c_str(), Callee.c_str());
			for (size_t j = 0;j < args.size();j++)
				SAFE_DELETE(args[j]);
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
	}

	//RIGHT PALEN
	if (isExpectedToken(")")) {
		Tokens->getNextToken();
		return new CallExprAST(func_type, Callee, args, proto);
	}else{
		fprintf(stderr, "%s:%d:%d: error: expected ')' but %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		for (size_t i = 0;i < args.size();i++)
			SAFE_DELETE(args[i]);
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
}


/**
  * PrimaryExpression用構文解析メソッド
  * @return 解析成功時：AST　失敗時：NULL
  */
BaseAST *Parser::visitPrimaryExpression() {
	int bkup = Tokens->getCurIndex();

	// 数値、真偽値、条件式、変数呼び出しなど
	//integer
	if (isExpectedToken(TOK_DIGIT)) {
		int val = Tokens->getCurNumVal();
		Tokens->getNextToken();
		return new ValueAST(val, Types(Type_number));

	//float
	}else if (isExpectedToken(TOK_FLOAT)) {
		float val = Tokens->getCurNumVal();
		Tokens->getNextToken();
		return new ValueAST(val, Types(Type_float, 32, true));

	//負の数
	}else if (isExpectedToken("-")) {
		Tokens->getNextToken();
		if (isExpectedToken(TOK_DIGIT)) {
			int val = Tokens->getCurNumVal();
			Tokens->getNextToken();
			return new ValueAST(-val, Types(Type_number, 32, true));
		}else if (isExpectedToken(TOK_FLOAT)) {
			float val = Tokens->getCurNumVal();
			Tokens->getNextToken();
			return new ValueAST(-val, Types(Type_float, 32, true));
		}else{
			fprintf(stderr, "%s:%d:%d: error: rhs of - is nothing\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}

	// true / false
	}else if (isExpectedToken(TOK_TRUTH)) {
		Tokens->getNextToken();
		return new ValueAST(Tokens->getCurBoolVal(), Types(Type_bool, 1, true));

	// 文字列
	}else if (isExpectedToken("\"")) {
		Tokens->getNextToken();
		if (!isExpectedToken(TOK_STRING)) {
			fprintf(stderr, "%s:%d:%d: error: string is nothing\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
		std::string str = Tokens->getCurString();
		Tokens->getNextToken();
		if (!isExpectedToken("\"")) {
			fprintf(stderr, "%s:%d:%d: error: \" is nothing\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
		Tokens->getNextToken();
		return new ValueAST(str, Types(Type_char, 1, true));

	}else if (isExpectedToken("null")) {
		if (Debbug) fprintf(stderr, "%s:%d:%d: none\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
		Tokens->getNextToken();
		return new ValueAST(0, Types(Type_all));

	}else if (isExpectedToken("_")) {
		Tokens->getNextToken();
		return new PlaceholderAST();

	//VARIABLE_IDENTIFIER
	}else if (isExpectedToken(TOK_IDENTIFIER) || isExpectedToken("&")) {
		bool reference = false;
		if (isExpectedToken("&")) {
			reference = true;
			Tokens->getNextToken();
		}
		std::string var = Tokens->getCurString();
		Tokens->getNextToken();
		int index = -1;
		if (isExpectedToken("[")) {
			Tokens->getNextToken();
			if (!isExpectedToken(TOK_DIGIT)) {
				fprintf(stderr, "%s:%d:%d: error: cannot find index\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__);
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}
			index = Tokens->getCurNumVal();
			Tokens->getNextToken();
			if (!isExpectedToken("]")) {
				fprintf(stderr, "%s:%d:%d: error: expected ']' but %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}
			Tokens->getNextToken();
		}
		for (size_t i = 0;;i++) {
			if (!InsertPoint->getOldVar(i)) break;
			if (InsertPoint->getOldVar(i)->getName() == var) {
				if (Debbug) fprintf(stderr, "%s:%d:%d: call old variable %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, var.c_str());
				if (!InsertPoint->getOldVar(i) || !InsertPoint->getOldVar(i)->getOwner()) {
					fprintf(stderr, "%s:%d:%d: error: don't have ownership %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
					Tokens->applyTokenIndex(bkup);
					return NULL;
				}
				if (!reference) InsertPoint->getOldVar(i)->setOwner(false);
				if (index != ~0) {
					return new VariableAST(InsertPoint->getOldVar(i), index);
				}
				return new VariableAST(InsertPoint->getOldVar(i));
			}
		}
		for (size_t i = 0;;i++) {
			if (!InsertPoint->getNewVar(i)) break;
			if (InsertPoint->getNewVar(i)->getName() == var) {
				if (Debbug) fprintf(stderr, "%s:%d:%d: call new variable %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, var.c_str());
				if (!InsertPoint->getNewVar(i)->getOwner()) {
					fprintf(stderr, "%s:%d:%d: error: don't have ownership\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
					Tokens->applyTokenIndex(bkup);
					return NULL;
				}
				if (!reference) InsertPoint->getNewVar(i)->setOwner(false);
				if (index != ~0) {
					return new VariableAST(InsertPoint->getNewVar(i), index);
				}
				return new VariableAST(InsertPoint->getNewVar(i));
			}
		}
		Tokens->applyTokenIndex(bkup);
		
	// '(' expression ')'
	}else if (isExpectedToken("(")) {
		Tokens->getNextToken();
		//expression
		BaseAST *assign_expr = visitAssignmentExpression(Type_all);
		if (!assign_expr) {
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}

		//RIGHT PALEN
		if (!isExpectedToken(")")) {
			if (Debbug) fprintf(stderr, "%s:%d:%d: expected ')' but %s\n", Tokens->getFile().c_str(), Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
			SAFE_DELETE(assign_expr);
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
		Tokens->getNextToken();
		
		return assign_expr;
	}
	Tokens->applyTokenIndex(bkup);
	return NULL;
}

