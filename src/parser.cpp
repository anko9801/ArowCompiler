#include "parser.hpp"

bool Debbug = true;

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
	fprintf(stderr, "%d:%d: %s %s\n", Tokens->getLine(), __LINE__, __func__, Tokens->getCurString().c_str());
	if (!isExpectedToken("import"))
		return false;
	Tokens->getNextToken();

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

	if (Debbug) fprintf(stderr, "%s\n", __func__);
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
	int bkup=Tokens->getCurIndex();

	if (Debbug) fprintf(stderr, "%s\n", __func__);
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

	VariableTable.clear();
	FunctionTable.push_back(func);
	FunctionStmtAST *func_stmt = visitFunctionStatement(proto);
	if (func_stmt) {
		if (Debbug) fprintf(stderr, "%d:%d: %s %s\n", Tokens->getLine(), __LINE__, __func__, Tokens->getCurString().c_str());
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
	if (Debbug) fprintf(stderr, "%s\n", __func__);
	Types func_type;
	std::string func_name;
	Types var_type;

	//bkup index
	int bkup=Tokens->getCurIndex();

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

	param_type = visitTypes();
	if (param_type != Types(Type_null)) {
		if (!isExpectedToken(TOK_IDENTIFIER)) {
			fprintf(stderr, "%d:%d: error: expected identifer but %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
		param_list.push_back(Seq(param_type, Tokens->getCurString()));
		Tokens->getNextToken();

		while (true) {
			if (isExpectedToken(")"))
				break;

			if (!isExpectedToken(",")) {
				fprintf(stderr, "%d:%d: error: expected identifer but %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}
			Tokens->getNextToken();

			param_type = visitTypes();
			if (param_type == Types(Type_null)) {
				fprintf(stderr, "%d:%d: error: argument type is nothing\n", Tokens->getLine(), __LINE__);
				break;
			}

			if (!isExpectedToken(TOK_IDENTIFIER)) {
				fprintf(stderr, "%d:%d: error: expected identifer but %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}

			//引数の変数名に被りがないか確認
			for (size_t i = 0;i < param_list.size();i++) {
				if (param_list[i].Name == Tokens->getCurString()) {
					Tokens->applyTokenIndex(bkup);
					return NULL;
				}
			}
			param_list.push_back(Seq(param_type, Tokens->getCurString()));
			Tokens->getNextToken();
		}
	}

	//')'
	if (!isExpectedToken(")")) {
		Tokens->applyTokenIndex(bkup);
		return NULL;
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
	int bkup=Tokens->getCurIndex();

	//create FunctionStatement
	FunctionStmtAST *func_stmt = new FunctionStmtAST();

	//add parameter to FunctionStatement
	VariableDeclAST *vdecl;
	SetInsertPoint(func_stmt);
	for(size_t i = 0;i < proto->getParamSize(); i++) {
		vdecl = new VariableDeclAST(proto->getParamType(i), proto->getParamName(i));
		vdecl->setDeclType(VariableDeclAST::param);
		addStatement(vdecl);
		VariableTable.push_back(vdecl);
	}

	setFuncType(proto->getType());

	StatementsAST *stmts = visitStatements(func_stmt, 0);
	if (warning) return func_stmt;
	BaseAST *last_stmt = stmts->getStatement(stmts->getSize()-1);

	//check if last statement is jump_statement
	if (!last_stmt) {
		warning = true;
		Tokens->applyTokenIndex(bkup);
		return func_stmt;
	}
	if (!llvm::isa<JumpStmtAST>(last_stmt)) {
		fprintf(stderr, "%d:%d: warning: end of statement is not return statement\n", Tokens->getLine(), __LINE__);
		warning = true;
	}

	return func_stmt;
}


/**
  * Statements用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
StatementsAST *Parser::visitStatements(BaseAST* InsertPoint, int branch = 0) {
	int bkup = Tokens->getCurIndex();
	StatementsAST *stmts = new StatementsAST();
	BaseAST *stmt;

	if (!isExpectedToken("{")) {
		stmt = visitStatement();
		if (stmt) {
			if (InsertPoint) {
				SetInsertPoint(InsertPoint);
				addStatement(stmt, branch);
			}
			stmts->addStatement(stmt);
			return stmts;
		}
		if (!isExpectedToken("{")) {
			Tokens->applyTokenIndex(bkup);
			SAFE_DELETE(InsertPoint);
			return stmts;
		}
	}
	Tokens->getNextToken();

	while (true) {
		if (Debbug) fprintf(stderr, "%d:%d: %s %s\n", Tokens->getLine(), __LINE__, __func__, Tokens->getCurString().c_str());
		if (isExpectedToken("}")) {
			Tokens->getNextToken();
			break;
		}
		stmt = visitStatement();
		if (stmt) {
			if (InsertPoint) {
				SetInsertPoint(InsertPoint);
				addStatement(stmt, branch);
			}
			stmts->addStatement(stmt);
			continue;
		}else{
			if (Debbug) fprintf(stderr, "%d:%d: unknown %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
			Tokens->applyTokenIndex(bkup);
			stmts->clear();
			if (InsertPoint)
				SAFE_DELETE(InsertPoint);
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

	int bits;prim_type Type;bool non_null = false;int ArraySize = 0;

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
	}

	// non-null type
	if (isExpectedToken("?")) {
		non_null = true;
		Tokens->getNextToken();
	}

	// array
	if (isExpectedToken("[")) {
		Tokens->getNextToken();
		if (!isExpectedToken(TOK_DIGIT)) {
			Tokens->applyTokenIndex(bkup);
			return Types(Type_null);
		}
		ArraySize = Tokens->getCurNumVal();
		Tokens->getNextToken();
		return Types(Type, ArraySize, bits, non_null);
	}else{
		if (Debbug) fprintf(stderr, "%d:%d: %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		return Types(Type, bits, non_null);
	}
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
			fprintf(stderr, "%d:%d: error: return type is expected %s but %s\n", Tokens->getLine(), __LINE__, getFuncType().printType().c_str(), stmt->getType().printType().c_str());
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
		if (Debbug) fprintf(stderr, "%d:%d: %s %s\n", Tokens->getLine(), __LINE__, var_decl->getType().printType().c_str(), var_decl->getName().c_str());
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

	fprintf(stderr, "%d:%d: not find statement %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
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
		fprintf(stderr, "%d:%d: error: cannot find identifier of variable\n", Tokens->getLine(), __LINE__);
		return NULL;
	}

	name = Tokens->getCurString();
	Tokens->getNextToken();

	//配列かどうか
	if (isExpectedToken("[")) {
		Tokens->getNextToken();
		if (!isExpectedToken(TOK_DIGIT)) {
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}

		type.setArray(Tokens->getCurNumVal());

		Tokens->getNextToken();
		if (!isExpectedToken("]")) {
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
		Tokens->getNextToken();
	}

	for (size_t i = 0;i < VariableTable.size();i++) {
		if (VariableTable[i]->getName() == name) {
			fprintf(stderr, "%d:%d: error: redefinition of '%s' with a different type: '%s' vs '%s'\n", Tokens->getLine(), __LINE__, VariableTable[i]->getType().printType().c_str(), VariableTable[i]->getType().printType().c_str(), type.printType().c_str());
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
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
}


/**
  * JumpStatement用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitJumpStatement() {
	int bkup=Tokens->getCurIndex();
	BaseAST *expr;

	if (!isExpectedToken(TOK_RETURN))
		return NULL;
	Tokens->getNextToken();

	expr = visitAssignmentExpression(getFuncType());
	if (!expr) {
		if (Debbug) fprintf(stderr, "%d:%d: return %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	// 暗黙の型変換
	if (expr->getType() != getFuncType()) {
		if (expr->getType().getPrimType() == Type_number && getFuncType().getPrimType() == Type_number) {
			expr = new CastExprAST(expr, getFuncType());
			expr->setType(getFuncType());
		}
	}
	if (expr->getType().getBits() != getFuncType().getBits()) {
		expr = new CastExprAST(expr, getFuncType());
		expr->setType(getFuncType());
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
	
	if (!isExpectedToken("(")) {
		fprintf(stderr, "%d:%d: error: expected '(' but %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();

	CondStmt = visitExpression(NULL, Types(Type_bool));
	if (!CondStmt || CondStmt->getType() != Types(Type_bool, 1, true)) {
		fprintf(stderr, "%d:%d: error: if condition is not bool\n", Tokens->getLine(), __LINE__);
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	BaseAST *if_expr = new IfStmtAST(CondStmt);

	if (!isExpectedToken(")")) {
		fprintf(stderr, "%d:%d: error: expected ')' but %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();

	visitStatements(if_expr, 0);

	if (!isExpectedToken("else")) {
		return if_expr;
	}
	Tokens->getNextToken();

	visitStatements(if_expr, 1);
	return if_expr;
}


/**
  * WhileStatement用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitWhileStatement() {
	int bkup = Tokens->getCurIndex();

	BaseAST *CondStmt;
	if (Debbug) fprintf(stderr, "%d:%d: %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
	if (!isExpectedToken(TOK_WHILE)) {
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();

	if (!isExpectedToken("(")) {
		fprintf(stderr, "%d:%d: expected '(' but %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();

	CondStmt = visitExpression(NULL, Types(Type_bool));
	if (Debbug) fprintf(stderr, "%d:%d: %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
	if (!CondStmt || CondStmt->getType() != Types(Type_bool)) {
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	BaseAST *while_expr = new WhileStmtAST(CondStmt);

	if (!isExpectedToken(")")) {
		fprintf(stderr, "%d:%d: expected ')' but %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();

	visitStatements(while_expr);
	if (Debbug) fprintf(stderr, "%d:%d: %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
	return while_expr;
}


/**
  * MatchExpression用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitMatchExpression() {
	int bkup = Tokens->getCurIndex();

	if (Debbug) fprintf(stderr, "%d:%d: %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
	if (!isExpectedToken(TOK_MATCH)) {
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();

	if (!isExpectedToken("(")) {
		fprintf(stderr, "%d:%d: expected '(' but %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();

	BaseAST *Eval = visitAssignmentExpression(Types(Type_all));
	if (!Eval) {
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}

	if (!isExpectedToken(")")) {
		fprintf(stderr, "%d:%d: expected ')' but %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();

	if (!isExpectedToken("{")) {
		fprintf(stderr, "%d:%d: expected '{' but %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();

	IfStmtAST *if_expr = visitPatternExpression(Eval);
	if (!if_expr) {
		fprintf(stderr, "%d:%d: expected pattern but %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}

	if (!isExpectedToken("}")) {
		fprintf(stderr, "%d:%d: expected '}' but %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();

	if (Debbug) fprintf(stderr, "%d:%d: %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
	return if_expr;
}



IfStmtAST *Parser::visitPatternExpression(BaseAST *Eval) {
	int bkup = Tokens->getCurIndex();
	bool end = false;

	BaseAST *eval = visitExpression(NULL, Types(Type_all));
	if (!eval) {
		fprintf(stderr, "%d:%d: error: expected eval but %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	if (llvm::isa<PlaceholderAST>(eval)) end = true;

	IfStmtAST *if_expr;
	if_expr = new IfStmtAST(new BinaryExprAST("==", Eval, eval, Types(Type_bool)));

	if (!isExpectedToken("=>")) {
		fprintf(stderr, "%d:%d: error: expected '=>' but %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();

	StatementsAST *stmts = visitStatements(NULL);
	if (!stmts || !stmts->getSize()) {
		fprintf(stderr, "%d:%d: error: statement is nothing\n", Tokens->getLine(), __LINE__);
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	for (int i = 0;;i++)
		if (!stmts->getStatement(i)) break;
		else if_expr->addThen(stmts->getStatement(i));
	
	if (!isExpectedToken(",")) {
		if (!isExpectedToken("}")) {
			fprintf(stderr, "%d:%d: error: expected ',' but %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
		return if_expr;
	}
	Tokens->getNextToken();
	
	IfStmtAST *pattern = visitPatternExpression(Eval);
	if (!pattern) {
		Tokens->getNextToken();
		if (!isExpectedToken("}")) {
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
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
		for (size_t i = 0;i < VariableTable.size();i++) {
			if (VariableTable[i]->getName() == Tokens->getCurString()) {
				Tokens->getNextToken();
				all_confirm = true;

				int Index = 0;
				lhs = new VariableAST(VariableTable[i], Index);
				BaseAST *rhs;
				if (isExpectedToken("=")) {
					Tokens->getNextToken();

					rhs = visitExpression(NULL, VariableTable[i]->getType());
					if (rhs) {
						if (!isExpectedToken(","))
							Tokens->getNextToken();
						return new BinaryExprAST("=", lhs, rhs, VariableTable[i]->getType());
					}
				}else{
					SAFE_DELETE(lhs);
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
	lhs = visitExpression(NULL, Types(Type_all));
	if (!lhs)
		return NULL;
	
	if (isExpectedToken(";") || isExpectedToken("\n")) {
		Tokens->getNextToken();
		return lhs;
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
			if (rhs && rhs->getType() == Types(Type_number)) {
				return new BinaryExprAST(op, lhs, rhs, Types(Type_bool));
			}else{
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
	int bkup=Tokens->getCurIndex();

	if (!lhs)
		lhs = visitMultiplicativeExpression(NULL, type);
	if (!lhs)
		return NULL;
	BaseAST *rhs;

	//+
	if (isExpectedToken("+")) {
		Tokens->getNextToken();
		rhs = visitMultiplicativeExpression(NULL, type);
		if (rhs) {
			// 暗黙の型変換
			lhs = visitImplicitCastNumber(lhs, rhs->getType());
			rhs = visitImplicitCastNumber(rhs, lhs->getType());
			// 型変換が行われなかった時
			type = lhs->getType();
			return visitAdditiveExpression(new BinaryExprAST("+", lhs, rhs, type), type);
		}else{
			SAFE_DELETE(lhs);
			Tokens->applyTokenIndex(bkup);
			fprintf(stderr, "%d:%d: error: rhs of plus is nothing\n", Tokens->getLine(), __LINE__);
			return NULL;
		}
	//-
	}else if (isExpectedToken("-")) {
		Tokens->getNextToken();
		rhs = visitMultiplicativeExpression(NULL, type);
		if (rhs) {
			// 暗黙の型変換
			lhs = visitImplicitCastNumber(lhs, rhs->getType());
			rhs = visitImplicitCastNumber(rhs, lhs->getType());
			// 型変換が行われなかった時
			type = lhs->getType();
			return visitAdditiveExpression(new BinaryExprAST("-", lhs, rhs, type), type);
			// if (other->getType() != Types(Type_number)) {fprintf(stderr, "error: Type is not difference\n");return NULL;}
		}else{
			SAFE_DELETE(lhs);
			Tokens->applyTokenIndex(bkup);
			fprintf(stderr, "%d:%d: error: rhs of minus is nothing\n", Tokens->getLine(), __LINE__);
			return NULL;
		}
	}
	if (type == Types(Type_number) && lhs->getType() != type)
		lhs = new CastExprAST(lhs, type);
	return lhs;
}


/**
 * MultiplicativeExpression用解析メソッド
 * @param  lhs(左辺),初回呼び出し時はNULL
 * @return 解析成功：AST　解析失敗：NULL
 */
BaseAST *Parser::visitMultiplicativeExpression(BaseAST *lhs, Types type = Types(Type_all)) {
	//bkup index
	int bkup=Tokens->getCurIndex();

	if (!lhs)
		lhs = visitCastExpression();
	if (!lhs)
		return NULL;
	BaseAST *rhs;

	// *
	if (isExpectedToken("*")) {
		Tokens->getNextToken();
		rhs = visitCastExpression();
		if (rhs) {
			// 暗黙の型変換
			lhs = visitImplicitCastNumber(lhs, rhs->getType());
			rhs = visitImplicitCastNumber(rhs, lhs->getType());
			// 型変換が行われなかった時
			type = lhs->getType();
			return visitMultiplicativeExpression(new BinaryExprAST("*", lhs, rhs, type));
		}else{
			SAFE_DELETE(lhs);
			Tokens->applyTokenIndex(bkup);
			fprintf(stderr, "%d:%d: error: rhs of multiple is nothing\n", Tokens->getLine(), __LINE__);
			return NULL;
		}
			
	// /
	}else if (isExpectedToken("/")) {
		Tokens->getNextToken();
		rhs=visitCastExpression();
		if (rhs) {
			// 暗黙の型変換
			lhs = visitImplicitCastNumber(lhs, rhs->getType());
			rhs = visitImplicitCastNumber(rhs, lhs->getType());
			// 型変換が行われなかった時
			type = lhs->getType();
			return visitMultiplicativeExpression(new BinaryExprAST("/", lhs, rhs, type));
		}else{
			SAFE_DELETE(lhs);
			Tokens->applyTokenIndex(bkup);
			fprintf(stderr, "%d:%d: error: rhs of divide is nothing\n", Tokens->getLine(), __LINE__);
			return NULL;
		}

	// %
	}else if (isExpectedToken("%")) {
		Tokens->getNextToken();
		rhs = visitCastExpression();
		if (rhs) {
			// 暗黙の型変換
			lhs = visitImplicitCastNumber(lhs, rhs->getType());
			rhs = visitImplicitCastNumber(rhs, lhs->getType());
			// 型変換が行われなかった時
			type = lhs->getType();
			return visitMultiplicativeExpression(new BinaryExprAST("%", lhs, rhs, type), type);
		}else{
			SAFE_DELETE(lhs);
			Tokens->applyTokenIndex(bkup);
			fprintf(stderr, "%d:%d: error: rhs of divide is nothing\n", Tokens->getLine(), __LINE__);
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
	int bkup=Tokens->getCurIndex();
	BaseAST *lhs = visitPostfixExpression();
	if (!lhs)
		return NULL;

	if (isExpectedToken("as")) {
		Tokens->getNextToken();
		Types DestType = visitTypes();
		if (Debbug) fprintf(stderr, "%d:%d: as %s\n", Tokens->getLine(), __LINE__, DestType.printType().c_str());
		if (DestType.getPrimType() == Type_null) {
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
		// if (lhs->getType().getNonNull() < DestType.getNonNull()) {
		// 	IfStmtAST *nonif_stmt = new IfStmtAST(new BinaryExprAST("!=", lhs, new ValueAST(0, lhs->getType()), Types(Type_bool)));
		// 	nonif_stmt->addElse(new JumpStmtAST(new ValueAST(0, getFuncType())));
		// 	addStatement(nonif_stmt);
		// }
		return new CastExprAST(lhs, DestType, false);

	}else if (isExpectedToken("is")) {
		Tokens->getNextToken();
		Types DestType = visitTypes();
		if (Debbug) fprintf(stderr, "%d:%d: is %s\n", Tokens->getLine(), __LINE__, DestType.printType().c_str());
		if (DestType.getPrimType() == Type_null) {
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
		return new CastExprAST(lhs, DestType, true);

	}else if (isExpectedToken("?")) {
		if (Debbug) fprintf(stderr, "%d:%d: non-null\n", Tokens->getLine(), __LINE__);
		lhs->getType().setNonNull(true);
		Tokens->getNextToken();
		return new CastExprAST(lhs,
			Types(lhs->getType().getPrimType(), lhs->getType().getBits(), true),
			false);
	}
	return lhs;
}


/**
  * ImplicitCastNumber用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitImplicitCastNumber(BaseAST *src, Types impl_type) {
	if (src->getType() == impl_type && src->getType().getBits() == impl_type.getBits())
		return src;

	int src_priority;
	int impl_priority;

	if (impl_type.getPrimType() == Type_float) impl_priority = 2;
	if (impl_type.getPrimType() == Type_int) impl_priority = 1;
	if (impl_type.getPrimType() == Type_uint) impl_priority = 0;

	if (src->getType().getPrimType() == Type_float) src_priority = 2;
	if (src->getType().getPrimType() == Type_int) src_priority = 1;
	if (src->getType().getPrimType() == Type_uint) src_priority = 0;

	if (src_priority < impl_priority || src->getType().getBits() < impl_type.getBits()) {
		return new CastExprAST(src,
			Types(
				impl_type.getPrimType(),
				std::max(src->getType().getBits(), impl_type.getBits()),
				std::min(src->getType().getNonNull(), impl_type.getNonNull())
			)
		);
	}
	return src;
}


/**
  * ImplicitCastNonNull用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitImplicitCastNonNull(BaseAST *src, Types impl_type) {
	if (src->getType() == impl_type) {
		if (src->getType().getNonNull() > impl_type.getNonNull()) {
			return new CastExprAST(src, impl_type);
		}
	}
	return src;
}


/**
  * ImplicitCastBits用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitImplicitCastBits(BaseAST *src, Types impl_type) {
	if (src->getType() == impl_type) {
		if (src->getType().getBits() < impl_type.getBits()) {
			return new CastExprAST(src, impl_type);
		}
	}
	return src;
}


/**
  * PostfixExpression用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitPostfixExpression() {
	int bkup=Tokens->getCurIndex();

	//primary_expression
	BaseAST *prim_expr = visitPrimaryExpression();
	if (prim_expr)
		return prim_expr;

	// 関数呼び出し用
	//FUNCTION_IDENTIFIER
	if (!isExpectedToken(TOK_IDENTIFIER))
		return NULL;

	//関数名取得
	std::string Callee = Tokens->getCurString();
	if (Debbug) fprintf(stderr, "%d:%d: call function %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
	Tokens->getNextToken();

	//LEFT PALEN
	if (!isExpectedToken("(")) {
		Tokens->applyTokenIndex(bkup);
		fprintf(stderr, "%d:%d: error: LEFT PALEN is nothing\n", Tokens->getLine(), __LINE__);
		return NULL;
	}

	Tokens->getNextToken();
	//argument list
	std::vector<BaseAST*> args;

	BaseAST *assign_expr = visitAssignmentExpression(Type_all);
	if (assign_expr) {
		args.push_back(assign_expr);
		while (isExpectedToken(",")) {
			Tokens->getNextToken();
			//IDENTIFIER
			assign_expr = visitAssignmentExpression(Type_all);
			if (assign_expr) args.push_back(assign_expr);
			else break;
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
		fprintf(stderr, "%d:%d: error: undefined function %s\n", Tokens->getLine(), __LINE__, Callee.c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Types func_type = proto->getType();

	// 引数の型の確認
	for (size_t i = 0;i < args.size();i++) {
		// 暗黙の型変換
		args[i] = visitImplicitCastNumber(args[i], proto->getParamType(i));
		if (args[i]->getType() != proto->getParamType(i) || args[i]->getType().getNonNull() <= proto->getParamType(i).getNonNull()) {
			for (size_t j = 0;j < args.size();j++)
				SAFE_DELETE(args[j]);
			fprintf(stderr, "%d:%d: error: no match for function param '%s'\n", Tokens->getLine(), __LINE__, Callee.c_str());
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
	}

	//RIGHT PALEN
	if (isExpectedToken(")")) {
		Tokens->getNextToken();
		return new CallExprAST(func_type, Callee, args, proto);
	}else{
		for (size_t i = 0;i < args.size();i++)
			SAFE_DELETE(args[i]);
		Tokens->applyTokenIndex(bkup);
		fprintf(stderr, "%d:%d: error: expected ')' but %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		return NULL;
	}
}


/**
  * PrimaryExpression用構文解析メソッド
  * @return 解析成功時：AST　失敗時：NULL
  */
BaseAST *Parser::visitPrimaryExpression() {
	int bkup=Tokens->getCurIndex();

	// 数値、真偽値、条件式、変数呼び出しなど
	//integer
	if (isExpectedToken(TOK_DIGIT)) {
		int val = Tokens->getCurNumVal();
		if (Debbug) fprintf(stderr, "%d:%d: integer\n", Tokens->getLine(), __LINE__);
		Tokens->getNextToken();
		return new ValueAST(val, Types(Type_int, 32, true));

	//float
	}else if (isExpectedToken(TOK_FLOAT)) {
		float val = Tokens->getCurNumVal();
		if (Debbug) fprintf(stderr, "%d:%d: float\n", Tokens->getLine(), __LINE__);
		Tokens->getNextToken();
		return new ValueAST(val, Types(Type_float, 32, true));

	//負の数
	}else if (isExpectedToken("-")) {
		if (Debbug) fprintf(stderr, "%d:%d: integer\n", Tokens->getLine(), __LINE__);
		Tokens->getNextToken();
		if (isExpectedToken(TOK_DIGIT)) {
			int val = Tokens->getCurNumVal();
			Tokens->getNextToken();
			return new ValueAST(-val, Types(Type_int, 32, true));
		}else if (isExpectedToken(TOK_FLOAT)) {
			float val = Tokens->getCurNumVal();
			Tokens->getNextToken();
			return new ValueAST(-val, Types(Type_float, 32, true));
		}else{
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}

	// true / false
	}else if (isExpectedToken(TOK_TRUTH)) {
		if (Debbug) fprintf(stderr, "%d:%d: true\n", Tokens->getLine(), __LINE__);
		Tokens->getNextToken();
		return new ValueAST(Tokens->getCurBoolVal(), Types(Type_bool, 1, true));

	}else if (isExpectedToken("null")) {
		if (Debbug) fprintf(stderr, "%d:%d: none\n", Tokens->getLine(), __LINE__);
		Tokens->getNextToken();
		return new ValueAST(0, Types(Type_all));

	}else if (isExpectedToken("_")) {
		Tokens->getNextToken();
		return new PlaceholderAST();

	//VARIABLE_IDENTIFIER
	}else if (isExpectedToken(TOK_IDENTIFIER)) {
		for (size_t i = 0;i < VariableTable.size();i++)
			if (VariableTable[i]->getName() == Tokens->getCurString()) {
				if (Debbug) fprintf(stderr, "%d:%d: call variable %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
				Tokens->getNextToken();
				return new VariableAST(VariableTable[i]);
			}

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
			if (Debbug) fprintf(stderr, "%d:%d: expected ')' but %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
			SAFE_DELETE(assign_expr);
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
		Tokens->getNextToken();
		
		return assign_expr;
	}
	return NULL;
}

