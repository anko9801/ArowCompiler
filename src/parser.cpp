#include "parser.hpp"

bool Debbug = true;

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
	}else{
		return visitTranslationUnit();
	}
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
	TU = new TranslationUnitAST();

	// LLVMライブラリの関数addLib(TranslationUnitAST, ReturnType, FuncName, {Arguments, ...})
	addLib(TU, Types(Type_int, 1, true), "printnum", {Types(Type_int, 32, true)});
	addLib(TU, Types(Type_int, 1, true), "sleep", {Types(Type_int, 32, true)});
	addLib(TU, Types(Type_int, 1, true), "usclock", {});
	addLib(TU, Types(Type_int, 1, true), "BlinkLED", {Types(Type_int, 32, true)});
	addLib(TU, Types(Type_int, 1, true), "GPIOsetup", {});
	addLib(TU, Types(Type_int, 1, true), "GPIOclear", {});

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
		TranslationUnitAST *tunit) {
	//FunctionDefinition
	FunctionAST *func_def = visitFunctionDefinition();
	// warningが出ていたら戻す
	if(warning) return false;
	if(func_def){
		tunit->addFunction(func_def);
		return true;
	}
	//FunctionDeclaration
	PrototypeAST *proto=visitFunctionDeclaration();
	if(proto){
		tunit->addPrototype(proto);
		return true;
	}

	return false;
}


Types Parser::visitTypes() {
	int bkup = Tokens->getCurIndex();

	int bits;prim_type Type;bool non_null = false;int ArraySize = 0;

	// primary type
	if(Tokens->getCurType() != TOK_TYPE) {
		Tokens->applyTokenIndex(bkup);
		return Types(Type_null);
	}
	Type = str2Type(Tokens->getCurString());
	Tokens->getNextToken();

	// bit数
	if(Tokens->getCurType() == TOK_DIGIT) {
		bits = Tokens->getCurNumVal();
		Tokens->getNextToken();
	}else{
		bits = 32;
		if (Type == Type_bool)
			bits = 1;
	}

	// non-null type
	if(Tokens->getCurString() == "?") {
		non_null = true;
		Tokens->getNextToken();
	}

	// array
	if(Tokens->getCurString() == "[") {
		Tokens->getNextToken();
		if(Tokens->getCurType() != TOK_DIGIT) {
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
  * FunctionDeclaration用構文解析メソッド
  * @return 解析成功：PrototypeAST　解析失敗：NULL
  */
PrototypeAST *Parser::visitFunctionDeclaration(){
	int bkup = Tokens->getCurIndex();

	if (Debbug) fprintf(stderr, "%s\n", __func__);
	PrototypeAST *proto = visitPrototype();
	if(!proto){
		return NULL;
	}

	//prototype;
	if(Tokens->getCurString() == ";"){
		for (int i = 0;i < PrototypeTable.size();i++) {
			if (!PrototypeTable[i]) break;
			if (PrototypeTable[i] == proto) {
				fprintf(stderr, "Function：%s is redefined\n", proto->getName().c_str());
				SAFE_DELETE(proto);
				return NULL;
			}
		}
		FunctionAST *func = new FunctionAST(proto);
		for (int i = 0;i < FunctionTable.size();i++) {
			if (!FunctionTable[i]) break;
			if (FunctionTable[i] == func) {
				fprintf(stderr, "Function：%s is redefined\n", proto->getName().c_str());
				SAFE_DELETE(proto);
				return NULL;
			}
		}
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

	if(Debbug) fprintf(stderr, "%s\n", __func__);
	PrototypeAST *proto = visitPrototype();
	if(!proto) {
		return NULL;
	}
	FunctionAST *func = new FunctionAST(proto);
	for (int i = 0;i < FunctionTable.size();i++) {
		if (!FunctionTable[i]) break;
		if (FunctionTable[i] == func) {
			fprintf(stderr, "Function：%s is redefined\n", proto->getName().c_str());
			SAFE_DELETE(proto);
			return NULL;
		}
	}

	VariableTable.clear();
	FunctionStmtAST *func_stmt = visitFunctionStatement(proto);
	if(func_stmt){
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
PrototypeAST *Parser::visitPrototype(){
	if (Debbug) fprintf(stderr, "%s\n", __func__);
	Types func_type;
	std::string func_name;
	Types var_type;

	//bkup index
	int bkup=Tokens->getCurIndex();

	//type_specifier
	func_type = visitTypes();
	if(func_type.getPrimType() == Type_null) {
		if (Debbug) fprintf(stderr, "%d:%d: error: type is null\n", Tokens->getLine(), __LINE__);
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}

	//IDENTIFIER
	if(Tokens->getCurType() != TOK_IDENTIFIER){
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	func_name=Tokens->getCurString();
	Tokens->getNextToken();

	//'('
	if(Tokens->getCurString() != "("){
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();

	//parameter_list
	std::vector<Seq> param_list;
	Types param_type;
	bool is_first_param = true;
	while(true){
		if(Tokens->getCurString() == ")"){
			break;
		}
		//','
		if(!is_first_param && Tokens->getCurString() == ","){
			Tokens->getNextToken();
		}

		param_type = visitTypes();
		if(param_type == Types(Type_null)){
			break;
		}

		if(Tokens->getCurType() == TOK_IDENTIFIER){
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
			fprintf(stderr, "%d:%d: error: expected identifer but %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
		is_first_param = false;
	}

	//')'
	if(Tokens->getCurString() != ")"){
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();

	PrototypeTable.push_back(new PrototypeAST(func_type, func_name, param_list));
	return new PrototypeAST(func_type, func_name, param_list);
}


/**
  * FunctionStatement用構文解析メソッド
  * @param 勘数名，引数を格納したPrototypeクラスのインスタンス
  * @return 解析成功：FunctionSmtAST　解析失敗：NULL
  */
FunctionStmtAST *Parser::visitFunctionStatement(PrototypeAST *proto){
	int bkup=Tokens->getCurIndex();

	//create FunctionStatement
	FunctionStmtAST *func_stmt = new FunctionStmtAST();

	//add parameter to FunctionStatement
	VariableDeclAST *vdecl;
	SetInsertPoint(func_stmt);
	for(int i=0; i < proto->getParamNum(); i++){
		vdecl = new VariableDeclAST(proto->getParamType(i), proto->getParamName(i));
		vdecl->setDeclType(VariableDeclAST::param);
		addStatement(vdecl);
		VariableTable.push_back(vdecl);
	}

	setFuncType(proto->getType());

	std::vector<BaseAST*> stmts = visitStatements(func_stmt, 0);
	if (warning) return func_stmt;
	BaseAST *last_stmt = stmts[stmts.size()-1];

	//check if last statement is jump_statement
	if(!last_stmt) {
		warning = true;
		return func_stmt;
	}
	if(!llvm::isa<JumpStmtAST>(last_stmt)){
		fprintf(stderr, "%d:%d: warning: end of statement is not return statement\n", Tokens->getLine(), __LINE__);
		warning = true;
	}

	return func_stmt;
}


std::vector<BaseAST*> Parser::visitStatements(BaseAST* InsertPoint, int branch = 0) {
	int bkup = Tokens->getCurIndex();
	std::vector<BaseAST*> stmts;
	BaseAST *stmt;

	if (stmt = visitStatement()) {
		SetInsertPoint(InsertPoint);
		addStatement(stmt, branch);
		stmts.push_back(stmt);
		return stmts;
	}

	if (Tokens->getCurString() != "{") {
		Tokens->applyTokenIndex(bkup);
		SAFE_DELETE(InsertPoint);
		return stmts;
	}
	Tokens->getNextToken();

	while(true) {
		if (Tokens->getCurString() == "}") {
			Tokens->getNextToken();
			break;
		}else if (stmt = visitStatement()) {
			if (Debbug) fprintf(stderr, "%d:%d: %s %s\n", Tokens->getLine(), __LINE__, __func__, Tokens->getCurString().c_str());

			if(Tokens->getCurString() == ";"){
				Tokens->getNextToken();
			}
			SetInsertPoint(InsertPoint);
			addStatement(stmt, branch);
			stmts.push_back(stmt);
			continue;
		}else{
			fprintf(stderr, "%d:%d: unknown %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
			Tokens->applyTokenIndex(bkup);
			stmts.clear();
			SAFE_DELETE(InsertPoint);
			warning = true;
			return stmts;
		}
	}
	if (Debbug) fprintf(stderr, "%d:%d: %s %s\n", Tokens->getLine(), __LINE__, __func__, Tokens->getCurString().c_str());
	return stmts;
}

/**
  * Statement用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitStatement(){
	int bkup = Tokens->getCurIndex();
	BaseAST *stmt;

	// if文
	if(stmt = visitIfExpression()){
		return stmt;
	// while文
	}else if(stmt = visitWhileExpression()) {
		return stmt;
	// return文
	}else if(Tokens->getCurType() == TOK_RETURN){
		stmt = visitJumpStatement();
		if (stmt && stmt->getType() == getFuncType()) return stmt;
		else{
			fprintf(stderr, "%d:%d: error: return type is expected %s but %s\n", Tokens->getLine(), __LINE__, printType(getFuncType()).c_str(), printType(stmt->getType()).c_str());
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
	// 変数宣言
	}else if (stmt = visitVariableDeclaration()){
		VariableDeclAST* var_decl = (VariableDeclAST*)stmt;
		var_decl->setDeclType(VariableDeclAST::local);
		if (Debbug) fprintf(stderr, "%d:%d: %s %s\n", Tokens->getLine(), __LINE__, printType(var_decl->getType()).c_str(), var_decl->getName().c_str());
		VariableTable.push_back(var_decl);

		if (Tokens->getCurString() == "="){
			Tokens->ungetToken(1);
			return var_decl;
		}
		return stmt;
	// 変数代入か式
	}else if(stmt = visitAssignmentExpression()){
		return stmt;
	}else if(Tokens->getCurString() == "{"){
		return NULL;
	}
	fprintf(stderr, "%d:%d: not find statement %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
	return NULL;
}



/**
  * VariableDeclaration用構文解析メソッド
  * @return 解析成功：VariableDeclAST　解析失敗：NULL
  */
VariableDeclAST *Parser::visitVariableDeclaration(){
	int bkup = Tokens->getCurIndex();
	std::string name;

	Types type = visitTypes();
	if (type == Type_null) {
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}

	//IDENTIFIER
	if(Tokens->getCurType() != TOK_IDENTIFIER){
		Tokens->applyTokenIndex(bkup);
		fprintf(stderr, "%d:%d: error: cannot find identifier of variable\n", Tokens->getLine(), __LINE__);
		return NULL;
	}

	name = Tokens->getCurString();
	Tokens->getNextToken();

	//配列の変数かどうか
	if(Tokens->getCurString() == "["){
		Tokens->getNextToken();
		if (Tokens->getCurType() != TOK_DIGIT){
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}

		type.setArray(Tokens->getCurNumVal());

		Tokens->getNextToken();
		if (Tokens->getCurString() != "]"){
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
		Tokens->getNextToken();
	}

	for (int i = 0;i < VariableTable.size();i++) {
		if (VariableTable[i]->getName() == name) {
			fprintf(stderr, "%d:%d: error: redefinition of '%s' with a different type: '%s' vs '%s'\n", Tokens->getLine(), __LINE__, printType(VariableTable[i]->getType()).c_str(), printType(VariableTable[i]->getType()).c_str(), printType(type).c_str());
			return NULL;
		}
	}

	//式が終わったなら次の文、AssignもあるならFuncStmtで一回ループ回った後にAssignmentExpression関数を呼び出す
	//';'
	if(Tokens->getCurString() == ";"){
		Tokens->getNextToken();
		return new VariableDeclAST(type, name);
	//'='
	}else if(Tokens->getCurString() == "="){
		return new VariableDeclAST(type, name);
	}else{
		Tokens->applyTokenIndex(bkup);
		fprintf(stderr, "%d:%d: error: cannot find semicoron\n", Tokens->getLine(), __LINE__);
		return NULL;
	}
}



/**
  * JumpStatement用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitJumpStatement(){
	int bkup=Tokens->getCurIndex();
	BaseAST *expr;

	if(Tokens->getCurType() != TOK_RETURN){
		return NULL;
	}
	Tokens->getNextToken();

	expr = visitAssignmentExpression();
	if(!expr){
		if (Debbug) fprintf(stderr, "%d:%d: return %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}

	if(Tokens->getCurString() != ";"){
		if (Debbug) fprintf(stderr, "%d:%d: error expected ';' but %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}

	Tokens->getNextToken();
	return new JumpStmtAST(expr);
}

/**
  * IfExpression用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitIfExpression(){
	int bkup = Tokens->getCurIndex();

	BaseAST *CondStmt;

	if (Tokens->getCurType() != TOK_IF) {
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();
	
	if (Tokens->getCurString() != "(") {
		fprintf(stderr, "%d:%d: expected '(' but %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();

	CondStmt = visitExpression(NULL);
	if (!CondStmt || CondStmt->getType() != Types(Type_bool, 1, true)){
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	BaseAST *if_expr = new IfExprAST(CondStmt);

	if (Tokens->getCurString() != ")") {
		fprintf(stderr, "%d:%d: expected ')' but %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();

	visitStatements(if_expr, 0);

	if (Tokens->getCurString() != "else") {
		return if_expr;
	}
	Tokens->getNextToken();

	visitStatements(if_expr, 1);
	return if_expr;
}



/**
  * WhileExpression用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitWhileExpression(){
	int bkup = Tokens->getCurIndex();

	BaseAST *CondStmt;
	if (Debbug) fprintf(stderr, "%d:%d: %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
	if (Tokens->getCurType() != TOK_WHILE) {
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();

	if (Tokens->getCurString() != "(") {
		fprintf(stderr, "%d:%d: expected '(' but %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();

	CondStmt = visitExpression(NULL);
	if (Debbug) fprintf(stderr, "%d:%d: %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
	if (!CondStmt || CondStmt->getType() != Types(Type_bool)){
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	BaseAST *while_expr = new WhileExprAST(CondStmt);

	if (Tokens->getCurString() != ")") {
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
  * AssignmentExpression用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitAssignmentExpression() {
	int bkup = Tokens->getCurIndex();

	//	| IDENTIFIER '=' additive_expression
	BaseAST *lhs;

	if(Tokens->getCurType() == TOK_IDENTIFIER){
		//変数が宣言されているか確認
		bool all_confirm = false;
		for (int i = 0;i < VariableTable.size();i++) {
			if (VariableTable[i]->getName() == Tokens->getCurString()) {
				Tokens->getNextToken();

				int Index = 0;
				lhs = new VariableAST(VariableTable[i], Index);
				BaseAST *rhs;
				if(Tokens->getCurString() == "="){
					Tokens->getNextToken();

					rhs = visitAdditiveExpression(NULL);
					if (rhs && rhs->getType() == VariableTable[i]->getType()) {
						Tokens->getNextToken();
						return new BinaryExprAST("=", lhs, rhs, VariableTable[i]->getType());
					}
				}else{
					SAFE_DELETE(lhs);
					Tokens->applyTokenIndex(bkup);
				}
			}
		}
	}else{
		Tokens->applyTokenIndex(bkup);
	}

	//additive_expression
	lhs = visitExpression(NULL);
	if(lhs)
		return lhs;

	return NULL;
}



BaseAST *Parser::visitExpression(BaseAST *lhs) {
	int bkup = Tokens->getCurIndex();
	if (!lhs)
		lhs = visitAdditiveExpression(NULL);
	if (!lhs)
		return NULL;

	if (lhs && lhs->getType() == Types(Type_int)) {
		if (Tokens->getCurString() == "<" ||
			Tokens->getCurString() == ">" ||
			Tokens->getCurString() == "<=" ||
			Tokens->getCurString() == ">=" ||
			Tokens->getCurString() == "==" ||
			Tokens->getCurString() == "!=") {
			std::string op = Tokens->getCurString();
			BaseAST *rhs;

			Tokens->getNextToken();
			rhs = visitAdditiveExpression(NULL);
			if (rhs && rhs->getType() == Types(Type_int)) {
				return new BinaryExprAST(op, lhs, rhs, Types(Type_bool));
			}else{
				SAFE_DELETE(lhs);
				SAFE_DELETE(rhs);
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
BaseAST *Parser::visitAdditiveExpression(BaseAST *lhs){
	//bkup index
	int bkup=Tokens->getCurIndex();

	if(!lhs)
		lhs = visitMultiplicativeExpression(NULL);
	if(!lhs)
		return NULL;
	BaseAST *rhs;

	//+
	if(Tokens->getCurString() == "+"){
		Tokens->getNextToken();
		rhs = visitMultiplicativeExpression(NULL);
		if(rhs && rhs->getType() == Types(Type_int)){
			BaseAST *other = visitAdditiveExpression(
					new BinaryExprAST("+", lhs, rhs, Types(Type_int, 32, true)));
			if(other->getType() != Types(Type_int)){fprintf(stderr, "error: Type is not difference");return NULL;}
			return other;
		}else{
			SAFE_DELETE(lhs);
			Tokens->applyTokenIndex(bkup);
			fprintf(stderr, "%d:%d: error: rhs of plus is nothing\n", Tokens->getLine(), __LINE__);
			return NULL;
		}
	//-
	}else if(Tokens->getCurString() == "-"){
		Tokens->getNextToken();
		rhs = visitMultiplicativeExpression(NULL);
		if(rhs && rhs->getType() == Types(Type_int)){
			BaseAST *other = visitAdditiveExpression(
					new BinaryExprAST("-", lhs, rhs, Types(Type_int, 32, true)));
			if(other->getType() != Types(Type_int)){fprintf(stderr, "error: Type is not difference");return NULL;}
			return other;
		}else{
			SAFE_DELETE(lhs);
			Tokens->applyTokenIndex(bkup);
			fprintf(stderr, "%d:%d: error: rhs of minus is nothing\n", Tokens->getLine(), __LINE__);
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
BaseAST *Parser::visitMultiplicativeExpression(BaseAST *lhs){
	//bkup index
	int bkup=Tokens->getCurIndex();

	if(!lhs)
		lhs = visitPostfixExpression();
	if(!lhs)
		return NULL;
	BaseAST *rhs;

	// *
	if(Tokens->getCurString() == "*"){
		Tokens->getNextToken();
		rhs = visitPostfixExpression();
		if(rhs && rhs->getType() == Types(Type_int)){
			return visitMultiplicativeExpression(
					new BinaryExprAST("*", lhs, rhs, Types(Type_int, 32, true)));
		}else{
			SAFE_DELETE(lhs);
			Tokens->applyTokenIndex(bkup);
			fprintf(stderr, "%d:%d: error: rhs of multiple is nothing\n", Tokens->getLine(), __LINE__);
			return NULL;
		}
			
	// /
	}else if(Tokens->getCurString() == "/"){
		Tokens->getNextToken();
		rhs=visitPostfixExpression();
		if(rhs && rhs->getType() == Types(Type_int)){
			return visitMultiplicativeExpression(
					new BinaryExprAST("/", lhs, rhs, Types(Type_int, 32, true)));
		}else{
			SAFE_DELETE(lhs);
			Tokens->applyTokenIndex(bkup);
			fprintf(stderr, "%d:%d: error: rhs of divide is nothing\n", Tokens->getLine(), __LINE__);
			return NULL;
		}
	}
	return lhs;
}

BaseAST *Parser::visitCastExpression(BaseAST *lhs){
	//bkup index
	int bkup=Tokens->getCurIndex();

	if(!lhs)
		lhs = visitPostfixExpression();
	if(!lhs)
		return NULL;

	BaseAST *rhs;
	if(Tokens->getCurString() == "as") {
		if (Debbug) fprintf(stderr, "%d:%d: cast %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		Tokens->getNextToken();
		Types DestType = visitTypes();
		if (DestType.getPrimType() == Type_null){
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
		return new CastAST(lhs, DestType);

	}else if(Tokens->getCurString() == "is") {
		if (Debbug) fprintf(stderr, "%d:%d: cast %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
		Tokens->getNextToken();
		Types DestType = visitTypes();
		if (DestType.getPrimType() == Type_null){
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
		/*if (lhs->getType() != DestType) {
			lhs->setType(DestType);
		}*/
		return new CastAST(lhs, DestType);

	}else if(Tokens->getCurString() == "?") {
		if (Debbug) fprintf(stderr, "%d:%d: non-null\n", Tokens->getLine(), __LINE__);
		Types DestType = lhs->getType();
		if (DestType.getNonNull() == false) {
			DestType = Types(Type_int, 32, true);
		}
		Tokens->getNextToken();
		return new CastAST(lhs, DestType);
	}
	return lhs;
}

/**
  * PostfixExpression用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitPostfixExpression(){
	int bkup=Tokens->getCurIndex();

	//primary_expression
	BaseAST *prim_expr = visitPrimaryExpression();
	if(prim_expr)
		return prim_expr;

	// 関数呼び出し用
	//FUNCTION_IDENTIFIER
	if(Tokens->getCurType() != TOK_IDENTIFIER)
		return NULL;

	//関数名取得
	std::string Callee = Tokens->getCurString();
	if (Debbug) fprintf(stderr, "%d:%d: call function %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());
	Tokens->getNextToken();

	//LEFT PALEN
	if(Tokens->getCurString() != "("){
		Tokens->applyTokenIndex(bkup);
		fprintf(stderr, "%d:%d: error: LEFT PALEN is nothing\n", Tokens->getLine(), __LINE__);
		return NULL;
	}

	Tokens->getNextToken();
	//argument list
	std::vector<BaseAST*> args;

	BaseAST *assign_expr = visitAssignmentExpression();
	if(assign_expr){
		args.push_back(assign_expr);
		while(Tokens->getCurString() == ","){
			Tokens->getNextToken();

			//IDENTIFIER
			assign_expr=visitAssignmentExpression();
			if(assign_expr){
				args.push_back(assign_expr);
			}else{
				break;
			}
		}
	}

	//関数の名前と引数の型の確認
	Types func_type = confirm(Callee, args);
	if(func_type == Type_null) {
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}

	if (Debbug) fprintf(stderr, "%d:%d: %s\n", Tokens->getLine(), __LINE__, Tokens->getCurString().c_str());

	//RIGHT PALEN
	if(Tokens->getCurString() == ")"){
		Tokens->getNextToken();
		return new CallExprAST(func_type, Callee, args);
	}else{
		for(int i=0;i<args.size();i++)
			SAFE_DELETE(args[i]);
		Tokens->applyTokenIndex(bkup);
		fprintf(stderr, "%d:%d: error: RIGHT PALEN is nothing\n", Tokens->getLine(), __LINE__);
		return NULL;
	}
}


/**
  * PrimaryExpression用構文解析メソッド
  * @return 解析成功時：AST　失敗時：NULL
  */
BaseAST *Parser::visitPrimaryExpression(){
	int bkup=Tokens->getCurIndex();

	// 数値、真偽値、条件式、変数呼び出しなど
	//integer
	if(Tokens->getCurType() == TOK_DIGIT){
		int val = Tokens->getCurNumVal();
		Tokens->getNextToken();
		return new NumberAST(val);
	//integer(-)
	}else if(Tokens->getCurString() == "-"){
		Tokens->getNextToken();
		if(Tokens->getCurType() == TOK_DIGIT){
			int val = Tokens->getCurNumVal();
			Tokens->getNextToken();
			return new NumberAST(-val);
		}else{
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
	// true / false
	}else if(Tokens->getCurType() == TOK_TRUE) {
		Tokens->getNextToken();
		return new BooleanAST(true);
	}else if(Tokens->getCurType() == TOK_FALSE) {
		Tokens->getNextToken();
		return new BooleanAST(false);
	}else if(Tokens->getCurString() == "None") {
		Tokens->getNextToken();
		return new NoneAST();
	//VARIABLE_IDENTIFIER
	}else if(Tokens->getCurType() == TOK_IDENTIFIER) {
		for (int i = 0;i < VariableTable.size();i++) {
			if (VariableTable[i]->getName() == Tokens->getCurString()) {
				Tokens->getNextToken();
				return new VariableAST(VariableTable[i]);
			}
		}
	// '(' expression ')'
	}else if(Tokens->getCurString() == "("){
		Tokens->getNextToken();

		//expression
		BaseAST *assign_expr = visitAssignmentExpression();
		if(!assign_expr){
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}

		//RIGHT PALEN
		if(Tokens->getCurString() != ")"){
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

