#include "parser.hpp"


/**
  * コンストラクタ
  */
Parser::Parser(std::string filename){
	Tokens = LexicalAnalysis(filename);
}


/**
  * 構文解析実効
  * @return 解析成功：true　解析失敗：false
  */
bool Parser::doParse(){
	if(!Tokens){
		fprintf(stderr, "error at lexer\n");
		return false;
	}else
		return visitTranslationUnit();
}


/**
  * AST取得
  * @return TranslationUnitへの参照
  */
TranslationUnitAST &Parser::getAST(){
	if(TU)
		return *TU;
	else
		return *(new TranslationUnitAST());
}


/**
  * TranslationUnit用構文解析メソッド
  * @return 解析成功：true　解析失敗：false
  */
bool Parser::visitTranslationUnit(){
	//最初にprintnumの宣言追加
	TU = new TranslationUnitAST();
	std::vector<Seq> param_list;
	param_list.push_back(Seq("i32", "i"));
	TU->addPrototype(new PrototypeAST("bool", "printnum", param_list));
	PrototypeTable["printnum"] = 1;

	//ExternalDecl
	while(true){
		if(!visitExternalDeclaration(TU)){
			SAFE_DELETE(TU);
			return false;
		}
		if(Tokens->getCurType() == TOK_EOF)
			break;
	}
	return true;
}


/**
  * ExternalDeclaration用構文解析クラス
  * 解析したPrototyupeとFunctionASTをTranslationUitに追加
  * @param TranslationUnitAST
  * @return true 
  */
bool Parser::visitExternalDeclaration(
		TranslationUnitAST *tunit
		){
	//FunctionDeclaration
	PrototypeAST *proto = visitFunctionDeclaration();
	if(proto){
		tunit->addPrototype(proto);
		return true;
	}

	//FunctionDefinition
	FunctionAST *func_def = visitFunctionDefinition();
	if(func_def){
		tunit->addFunction(func_def);
		return true;
	}

	return false;
}


/**
  * FunctionDeclaration用構文解析メソッド
  * @return 解析成功：PrototypeAST　解析失敗：NULL
  */
PrototypeAST *Parser::visitFunctionDeclaration(){
	int bkup = Tokens->getCurIndex();

	PrototypeAST *proto = visitPrototype();
	if(!proto){
		return NULL;
	}

	//prototype;
	if(Tokens->getCurString() ==";"){
		if( PrototypeTable.find(proto->getName()) != PrototypeTable.end() ||
			(FunctionTable.find(proto->getName()) != FunctionTable.end() &&
			FunctionTable[proto->getName()] != proto->getParamNum() ) ){
			fprintf(stderr, "Function：%s is redefined" ,proto->getName().c_str()); 
			SAFE_DELETE(proto);
			return NULL;
		}
		PrototypeTable[proto->getName()]=proto->getParamNum();
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
FunctionAST *Parser::visitFunctionDefinition(){
	int bkup=Tokens->getCurIndex();

	PrototypeAST *proto=visitPrototype();
	if(!proto){
		return NULL;
	}else if( (PrototypeTable.find(proto->getName()) != PrototypeTable.end() &&
				PrototypeTable[proto->getName()] != proto->getParamNum() ) ||
				FunctionTable.find(proto->getName()) != FunctionTable.end()){
			fprintf(stderr, "Function：%s is redefined" ,proto->getName().c_str()); 
			SAFE_DELETE(proto);
			return NULL;
	}

	VariableTable.clear();
	FunctionStmtAST *func_stmt = visitFunctionStatement(proto);
	if(func_stmt){
		FunctionTable[proto->getName()]=proto->getParamNum();
		return new FunctionAST(proto,func_stmt);
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
PrototypeAST *Parser::visitPrototype(){
	std::string func_name;
	std::string var_type;

	//bkup index
	int bkup=Tokens->getCurIndex();

	//type_specifier
	if(Tokens->getCurType() == TOK_TYPE){
		var_type = Tokens->getCurString();
		Tokens->getNextToken();
	}else{
		return NULL;
	}


	//IDENTIFIER
	if(Tokens->getCurType() ==TOK_IDENTIFIER){
		func_name=Tokens->getCurString();
		Tokens->getNextToken();
	}else{
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}

	//'('
	if(Tokens->getCurString() =="("){
		Tokens->getNextToken();
	}else{
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}


	//parameter_list
	std::vector<Seq> param_list;
	std::string param_type;
	bool is_first_param = true;
	while(true){
		//','
		if(!is_first_param && Tokens->getCurType() ==TOK_SYMBOL && Tokens->getCurString() ==","){
			Tokens->getNextToken();
		}

		if(Tokens->getCurType() == TOK_TYPE){
			param_type = Tokens->getCurString();
			Tokens->getNextToken();
		}else{
			break;
		}

		if(Tokens->getCurType() ==TOK_IDENTIFIER){
			//引数の変数名に被りがないか確認
			for (int i = 0;i < param_list.size();i++) {
				if (param_list[i].Name == Tokens->getCurString())
					break;
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}
			param_list.push_back(Seq(param_type, Tokens->getCurString()));
			Tokens->getNextToken();
		}else{
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
		is_first_param = false;
	}

	//')'
	if(Tokens->getCurString() ==")"){
		Tokens->getNextToken();
		return new PrototypeAST("i32", func_name, param_list);
	}else{
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
}


/**
  * FunctionStatement用構文解析メソッド
  * @param 勘数名，引数を格納したPrototypeクラスのインスタンス
  * @return 解析成功：FunctionSmtAST　解析失敗：NULL
  */
FunctionStmtAST *Parser::visitFunctionStatement(PrototypeAST *proto){
	int bkup = Tokens->getCurIndex();

	//{
	if(Tokens->getCurString() =="{"){
		Tokens->getNextToken();
	}else{
		return NULL;
	}

	//create FunctionStatement
	FunctionStmtAST *func_stmt = new FunctionStmtAST();

	//add parameter to FunctionStatement
	for(int i=0; i<proto->getParamNum(); i++){
		VariableDeclAST *vdecl = new VariableDeclAST(proto->getParamType(i), proto->getParamName(i));
		vdecl->setDeclType(VariableDeclAST::param);
		func_stmt->addVariableDeclaration(vdecl);
		VariableTable.push_back(Seq(vdecl->getType(), vdecl->getName()));
	}

	VariableDeclAST *var_decl;
	BaseAST *stmt;
	BaseAST *last_stmt;

	while(true) {
		fprintf(stderr, "%d: %s %s\n", __LINE__, __func__, Tokens->getCurString().c_str());
		// 色々な文
		if(stmt = visitStatement()){
			last_stmt = stmt;
			func_stmt->addStatement(stmt);

		}else if(stmt = visitJumpStatement(proto->getType().c_str())){
			last_stmt = stmt;
			func_stmt->addStatement(stmt);

		// 変数宣言
		}else if (var_decl = visitVariableDeclaration()){
			var_decl->setDeclType(VariableDeclAST::local);

			for (int i = 0;i < VariableTable.size();i++) {
				if (VariableTable[i].Name == Tokens->getCurString()) {
					SAFE_DELETE(var_decl);
					SAFE_DELETE(func_stmt);
					fprintf(stderr, "%d: error: redeclaration variable\n", __LINE__);
					return NULL;
				}
			}

			func_stmt->addVariableDeclaration(var_decl);
			//fprintf(stderr, "%d: %s %s\n", __LINE__, var_decl->getType().c_str(), var_decl->getName().c_str());
			VariableTable.push_back(Seq(var_decl->getType(), var_decl->getName()));

			if (Tokens->getCurString() == "="){
				BaseAST *lhs, *rhs;
				lhs = new VariableAST(var_decl->getType(), var_decl->getName());
				Tokens->getNextToken();
				if (rhs = visitAdditiveExpression(NULL, var_decl->getType().c_str())) {
					func_stmt->addStatement(new BinaryExprAST("=", lhs, rhs));
					Tokens->getNextToken();
				}else{
					SAFE_DELETE(lhs);
					SAFE_DELETE(var_decl);
					SAFE_DELETE(func_stmt);
					return NULL;
				}
			}

		}else if(Tokens->getCurString() == "}") {
			break;

		}else{
			SAFE_DELETE(func_stmt);
			Tokens->applyTokenIndex(bkup);
			fprintf(stderr, "%d: error: shiranai statement , %s\n", __LINE__, Tokens->getCurString().c_str());
			return NULL;
		}
	}

	//check if last statement is jump_statement
	/*if(!last_stmt || !llvm::isa<JumpStmtAST>(last_stmt)){
		SAFE_DELETE(func_stmt);
		Tokens->applyTokenIndex(bkup);
		fprintf(stderr, "%d: error: cannot find return statement\n", __LINE__);
		return NULL;
	}*/

	// }
	if (Tokens->getCurString() == "}"){
		Tokens->getNextToken();
		return func_stmt;
	}else{
		SAFE_DELETE(func_stmt);
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
}


/**
  * Statement用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitStatement(){
	BaseAST *stmt = NULL;
	if(stmt = visitExpressionStatement()){
		return stmt;
	}else if(stmt = visitIfExpression()){
		return stmt;
	}else{
		return NULL;
	} 
}


/**
  * VariableDeclaration用構文解析メソッド
  * @return 解析成功：VariableDeclAST　解析失敗：NULL
  */
VariableDeclAST *Parser::visitVariableDeclaration(){
	int bkup = Tokens->getCurIndex();
	std::string name;
	std::string type;

	// 型
	if (Tokens->getCurType() != TOK_TYPE)
		return NULL;

	type = Tokens->getCurString();
	Tokens->getNextToken();

	//IDENTIFIER
	if (Tokens->getCurType() != TOK_IDENTIFIER){
		Tokens->applyTokenIndex(bkup);
		fprintf(stderr, "%d: error: cannot find identifier of variable\n", __LINE__);
		return NULL;
	}

	name = Tokens->getCurString();
	Tokens->getNextToken();

	//';'
	if(Tokens->getCurString() == ";"){
		Tokens->getNextToken();
		return new VariableDeclAST(type, name);

	//'='
	}else if(Tokens->getCurString() == "="){
		return new VariableDeclAST(type, name);
	}else{
		Tokens->applyTokenIndex(bkup);
		fprintf(stderr, "%d: error: cannot find semicoron\n", __LINE__);
		return NULL;
	}
}



/**
  * ExpressionStatement用構文解析メソッド
  * @return  解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitExpressionStatement(){
	BaseAST *expr;

	//NULL Expression
	if(Tokens->getCurString() == ";") {
		Tokens->getNextToken();
		return new NullExprAST();
	}else if (expr = visitAssignmentExpression()) {
		if (Tokens->getCurString() == ";") {
			Tokens->getNextToken();
			return expr;
		}
	}else if (expr = visitAdditiveExpression(NULL, "i32")) {
		if (Tokens->getCurString() == ";") {
			Tokens->getNextToken();
			return expr;
		}
	}
	return NULL;
}


/**
  * JumpStatement用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitJumpStatement(const char* Type) {
	int bkup = Tokens->getCurIndex();
	BaseAST *expr;

	if(Tokens->getCurType() != TOK_RETURN) {
		return NULL;
	}

	Tokens->getNextToken();
	if(expr = visitAssignmentExpression()){
		
	}else if (expr = visitAdditiveExpression(NULL, Type)) {
		
	}else{
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}

	if (Tokens->getCurString() == ";") {
		Tokens->getNextToken();
		return new JumpStmtAST(expr);
	}else{
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
}


/**
  * IfExpression用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitIfExpression() {
	int bkup = Tokens->getCurIndex();

	std::vector<BaseAST*> ThenStmt, ElseStmt;
	BaseAST *stmt, *CondStmt;

	if (Tokens->getCurType() == TOK_IF) {
		Tokens->getNextToken();

		if (!(stmt = visitAdditiveExpression(NULL, "bool"))){
			CondStmt = stmt;
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}

		if (Tokens->getCurString() != "{") {
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
		Tokens->getNextToken();

		while(true) {
			if (stmt = visitStatement()) {
				ThenStmt.push_back(stmt);
				continue;
			}

			if (Tokens->getCurString() == "}") {
				Tokens->getNextToken();
				break;
			}else{
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}
		}

		if (Tokens->getCurString() != "else") {
			return new IfExprAST(CondStmt, ThenStmt, ElseStmt);
		}
		Tokens->getNextToken();

		if (Tokens->getCurString() != "{") {
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
		Tokens->getNextToken();

		while(true) {
			if (stmt = visitStatement()) {
				ElseStmt.push_back(stmt);
				continue;
			}

			if (Tokens->getCurString() == "}") {
				Tokens->getNextToken();
				break;
			}else{
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}
		}
		return new IfExprAST(CondStmt, ThenStmt, ElseStmt);
	}else{
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
}


/**
  * AssignmentExpression用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitAssignmentExpression(){
	int bkup = Tokens->getCurIndex();

	//	| IDENTIFIER '=' additive_expression
	BaseAST *lhs;
	const char* Type;

	if (Tokens->getCurType() == TOK_IDENTIFIER){
		//変数が宣言されているか確認
		bool all_confirm = false;
		for (int i = 0;i < VariableTable.size();i++) {
			if (VariableTable[i].Name == Tokens->getCurString()) {
				all_confirm = true;
				Type = VariableTable[i].Type.c_str();
				lhs = new VariableAST(VariableTable[i].Type, VariableTable[i].Name);
				Tokens->getNextToken();
				BaseAST *rhs;
				if (Tokens->getCurType() == TOK_SYMBOL &&
						Tokens->getCurString() == "=") {
					Tokens->getNextToken();
					if (rhs = visitAdditiveExpression(NULL, Type)) {
						return new BinaryExprAST("=", lhs, rhs);
					}else{
						SAFE_DELETE(lhs);
						Tokens->applyTokenIndex(bkup);
						return NULL;
					}
				}else{
					SAFE_DELETE(lhs);
					Tokens->applyTokenIndex(bkup);
					return NULL;
				}
			}
		}
		//if (!all_confirm)
			// 関数の可能性
			// fprintf(stderr, "%d: error: cannot find variable\n", __LINE__);
	}else{
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
}



/**
  * AdditiveExpression用構文解析メソッド
  * @param lhs(左辺),初回呼び出し時はNULL
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitAdditiveExpression(BaseAST *lhs, const char* Type){
	//bkup index
	int bkup = Tokens->getCurIndex();

	if(!lhs)
		lhs = visitMultiplicativeExpression(NULL, Type);
	BaseAST *rhs;

	if(!lhs){
		// AEじゃない
		// fprintf(stderr, "%d: error: lhs is nothing\n", __LINE__);
		return NULL;
	}

	if (!strcmp(Type, "i32")) {

		//+
		if(Tokens->getCurType() == TOK_SYMBOL &&
				Tokens->getCurString() == "+"){
			Tokens->getNextToken();
			rhs = visitMultiplicativeExpression(NULL, Type);
			if(rhs){
				return visitAdditiveExpression(
						new BinaryExprAST("+", lhs, rhs),
						Type);
			}else{
				SAFE_DELETE(lhs);
				Tokens->applyTokenIndex(bkup);
				fprintf(stderr, "%d: error: rhs of plus is nothing\n", __LINE__);
				return NULL;
			}

			//-
		}else if(Tokens->getCurType() == TOK_SYMBOL &&
				Tokens->getCurString() == "-"){
			Tokens->getNextToken();
			rhs = visitMultiplicativeExpression(NULL, Type);
			if(rhs){
				return visitAdditiveExpression(
						new BinaryExprAST("-", lhs, rhs),
						Type);
			}else{
				SAFE_DELETE(lhs);
				Tokens->applyTokenIndex(bkup);
				fprintf(stderr, "%d: error: rhs of minus is nothing\n", __LINE__);
				return NULL;
			}
		}
		return lhs;

	}else if (!strcmp(Type, "bool")) {
		//||
		if(Tokens->getCurType() == TOK_SYMBOL &&
				Tokens->getCurString() == "||"){
			Tokens->getNextToken();
			rhs = visitMultiplicativeExpression(NULL, Type);
			if(rhs){
				return visitAdditiveExpression(
						new BinaryExprAST("||", lhs, rhs),
						Type);
			}else{
				SAFE_DELETE(lhs);
				Tokens->applyTokenIndex(bkup);
				fprintf(stderr, "%d: error: rhs of plus is nothing\n", __LINE__);
				return NULL;
			}
		}

	}else{
		SAFE_DELETE(lhs);
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
}


/**
  * MultiplicativeExpression用解析メソッド
  * @param  lhs(左辺),初回呼び出し時はNULL
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitMultiplicativeExpression(BaseAST *lhs, const char* Type){
	//bkup index
	int bkup = Tokens->getCurIndex();

	if(!lhs)
		lhs = visitPostfixExpression(Type);

	BaseAST *rhs;

	if(!lhs){
		// fprintf(stderr, "%d: error: lhs of Multiple is nothing\n", __LINE__);
		return NULL;
	}

	if (!strcmp(Type, "i32")) {
		// *
		if(Tokens->getCurType() == TOK_SYMBOL &&
				Tokens->getCurString() == "*"){
			Tokens->getNextToken();
			rhs = visitPostfixExpression(Type);
			if(rhs){
				return visitMultiplicativeExpression(
						new BinaryExprAST("*", lhs, rhs),
						Type);
			}else{
				SAFE_DELETE(lhs);
				Tokens->applyTokenIndex(bkup);
				fprintf(stderr, "%d: error: rhs of multiple is nothing\n", __LINE__);
				return NULL;
			}

			// /
		}else if(Tokens->getCurType() == TOK_SYMBOL &&
				Tokens->getCurString() == "/"){
			Tokens->getNextToken();
			rhs = visitPostfixExpression(Type);
			if(rhs){
				return visitMultiplicativeExpression(
						new BinaryExprAST("/", lhs, rhs),
						Type);
			}else{
				SAFE_DELETE(lhs);
				Tokens->applyTokenIndex(bkup);
				fprintf(stderr, "%d: error: rhs of divide is nothing\n", __LINE__);
				return NULL;
			}
		}

	}else if (!strcmp(Type, "bool")) {
		// &&
		if(Tokens->getCurType() ==TOK_SYMBOL &&
				Tokens->getCurString() =="&&"){
			Tokens->getNextToken();
			rhs = visitPostfixExpression(Type);
			if(rhs){
				return visitMultiplicativeExpression(
						new BinaryExprAST("&&", lhs, rhs),
						Type);
			}else{
				SAFE_DELETE(lhs);
				Tokens->applyTokenIndex(bkup);
				fprintf(stderr, "%d: error: rhs of multiple is nothing\n", __LINE__);
				return NULL;
			}
		}
	}
	return lhs;
}


/**
  * PostfixExpression用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitPostfixExpression(const char* Type){
	//get index
	int bkup = Tokens->getCurIndex();

	//primary_expression
	BaseAST *prim_expr = visitPrimaryExpression(Type);
	if (prim_expr)
		return prim_expr;

	//FUNCTION_IDENTIFIER
	if (Tokens->getCurType() == TOK_IDENTIFIER){
		//is FUNCTION_IDENTIFIER
		int param_num;
		if(PrototypeTable.find(Tokens->getCurString()) !=
			PrototypeTable.end() ){
			param_num = PrototypeTable[Tokens->getCurString()];
		}else if(FunctionTable.find(Tokens->getCurString()) !=
			FunctionTable.end()){
			param_num = FunctionTable[Tokens->getCurString()];
		}else{
			fprintf(stderr, "%d: error: function identifier is nothing\n", __LINE__);
			return NULL;
		}

		//関数名取得
		std::string Callee = Tokens->getCurString();
		Tokens->getNextToken();

		//LEFT PALEN
		if(Tokens->getCurType() != TOK_SYMBOL ||
				Tokens->getCurString() != "("){
			Tokens->applyTokenIndex(bkup);
			fprintf(stderr, "%d: error: LEFT PALEN is nothing\n", __LINE__);
			return NULL;
		}

		Tokens->getNextToken();
		//argument list
		std::vector<BaseAST*> args;
		BaseAST *assign_expr = visitAssignmentExpression();
		if(assign_expr){
			args.push_back(assign_expr);
			while(Tokens->getCurType() == TOK_SYMBOL &&
					Tokens->getCurString() == ","){
				Tokens->getNextToken();

				//IDENTIFIER
				assign_expr = visitAssignmentExpression();
				if(assign_expr){
					args.push_back(assign_expr);
				}else{
					break;
				}
			}//end while
		}

		//引数の数を確認
		if(args.size() != param_num){
			for(int i = 0;i < args.size();i++)
				SAFE_DELETE(args[i]);
			Tokens->applyTokenIndex(bkup);
			fprintf(stderr, "%d: error: no match for function\n", __LINE__);
			return NULL;
		}

		//RIGHT PALEN
		if(Tokens->getCurType() ==TOK_SYMBOL &&
					Tokens->getCurString() ==")"){
			Tokens->getNextToken();
			return new CallExprAST("i32", Callee, args);
		}else{
			for(int i=0;i<args.size();i++)
				SAFE_DELETE(args[i]);
			Tokens->applyTokenIndex(bkup);
			fprintf(stderr, "%d: error: RIGHT PALEN is nothing\n", __LINE__);
			return NULL;
		}

	}else{
		// fprintf(stdout, "%d: error: cannot find function identifer\n", __LINE__);
		return NULL;
	}
}


/**
  * PrimaryExpression用構文解析メソッド
  * @return 解析成功時：AST　失敗時：NULL
  */
BaseAST *Parser::visitPrimaryExpression(const char* Type){
	//recored index
	int bkup = Tokens->getCurIndex();

	if (!Type){
		return NULL;
	}

	if (!strcmp(Type, "i32")) {
		//VARIABLE_IDENTIFIER
		if(Tokens->getCurType() == TOK_IDENTIFIER) {
			for (int i = 0;i < VariableTable.size();i++) {
				if (VariableTable[i].Name == Tokens->getCurString()) {
					Tokens->getNextToken();
					return new VariableAST(VariableTable[i].Type, VariableTable[i].Name);
				}
			}

		//integer
		}else if(Tokens->getCurType() == TOK_DIGIT){
			int val=Tokens->getCurNumVal();
			Tokens->getNextToken();
			return new NumberAST(val);

		//integer(-)
		}else if(Tokens->getCurType() == TOK_SYMBOL &&
				Tokens->getCurString() == "-"){
			Tokens->getNextToken();
			if(Tokens->getCurType() == TOK_DIGIT){
				int val = Tokens->getCurNumVal();
				Tokens->getNextToken();
				return new NumberAST(-val);
			}else{
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}
        // '(' expression ')'
		}else if(Tokens->getCurType() == TOK_SYMBOL &&
				Tokens->getCurString() == "("){
			Tokens->getNextToken();

			//expression
			BaseAST *assign_expr;
			if(!(assign_expr = visitAssignmentExpression())){
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}

			//RIGHT PALEN
			if(Tokens->getCurString() == ")"){
				Tokens->getNextToken();
				return assign_expr;
			}else{
				SAFE_DELETE(assign_expr);
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}
		}else{
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}

	}else if (!strcmp(Type, "bool")) {
		bool isnot = false;
		BaseAST *expr;
		BaseAST *lhs;

		//VARIABLE_IDENTIFIER
		if(Tokens->getCurType() == TOK_IDENTIFIER) {
			for (int i = 0;i < VariableTable.size();i++) {
				if (VariableTable[i].Name == Tokens->getCurString()) {
					Tokens->getNextToken();
					return new VariableAST(VariableTable[i].Type, VariableTable[i].Name);
				}
			}

		}else if (Tokens->getCurType() == TOK_TRUE) {
			fprintf(stderr, "%d: %s\n", __LINE__, Tokens->getCurString().c_str());
			Tokens->getNextToken();
			return new BooleanAST(true);
		}else if (Tokens->getCurType() == TOK_FALSE) {
			Tokens->getNextToken();
			return new BooleanAST(false);

		// '(' expression ')'
		}else if(Tokens->getCurType() == TOK_SYMBOL &&
				Tokens->getCurString() == "("){
			Tokens->getNextToken();

			//expression
			BaseAST *assign_expr;
			if(!(assign_expr = visitAssignmentExpression())){
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}

			//RIGHT PALEN
			if(Tokens->getCurString() == ")"){
				Tokens->getNextToken();
				return assign_expr;
			}else{
				SAFE_DELETE(assign_expr);
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}
		}

		if (Tokens->getCurString() == "!") {
			isnot = !isnot;
			Tokens->getNextToken();
		}

		if (lhs = visitAdditiveExpression(NULL, "i32")) {
			std::string op;
			BaseAST *rhs;

			if (Tokens->getCurString() == "<" ||
					Tokens->getCurString() == ">" ||
					Tokens->getCurString() == "<=" ||
					Tokens->getCurString() == ">=" ||
					Tokens->getCurString() == "==" ||
					Tokens->getCurString() == "!=") {
				op = Tokens->getCurString();
				Tokens->getNextToken();
				if (rhs = visitAdditiveExpression(NULL, "i32")) {
					return new BinaryExprAST(op, lhs, rhs);
				}else{
					SAFE_DELETE(lhs);
					SAFE_DELETE(rhs);
					Tokens->applyTokenIndex(bkup);
				}
			}else{
				SAFE_DELETE(lhs);
				Tokens->applyTokenIndex(bkup);
			}
		}else{
			Tokens->applyTokenIndex(bkup);
		}
		return NULL;
	}else{
		return NULL;
	}
}







