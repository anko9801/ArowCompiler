#include "parser.hpp"

bool Debbug = true;
std::string Ty2str(Types type) {
	if(type == Types::i1){
		return "bool";
	}else if(type == Types::i2) {
		return "i2";
	}else if(type == Types::i4) {
		return "i4";
	}else if(type == Types::i8) {
		return "i8";
	}else if(type == Types::i16) {
		return "i16";
	}else if(type == Types::i32) {
		return "i32";
	}else if(type == Types::$i32) {
		return "$i32";
	}else if(type == Types::$i1) {
		return "$i1";
	}else if(type == Types::null){
		return "null";
	}
}

Types str2Ty(std::string str) {
	if(str == "bool"){
		return Types::i1;
	}else if(str == "i2") {
		return Types::i2;
	}else if(str == "i4") {
		return Types::i4;
	}else if(str == "i8") {
		return Types::i8;
	}else if(str == "i16") {
		return Types::i16;
	}else if(str == "i32") {
		return Types::i32;
	}else if(str == "int") {
		return Types::i32;
	}else if(str == "$i32") {
		return Types::$i32;
	}else if(str == "$i1") {
		return Types::$i1;
	}else if(str == "null"){
		return Types::null;
	}
}

/**
  * コンストラクタ
  */
Parser::Parser(std::string filename){
	Tokens=LexicalAnalysis(filename);
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
	// ライブラリの関数
	TU = new TranslationUnitAST();
	std::vector<Seq> param_list;

	param_list.push_back(Seq(Types::$i32, "i"));
	TU->addPrototype(new PrototypeAST(Types::$i1, "printnum", param_list));
	PrototypeTable.push_back(Func(Types::$i1, "printnum", param_list));
	param_list.clear();

	param_list.push_back(Seq(Types::$i32, "i"));
	TU->addPrototype(new PrototypeAST(Types::$i1, "sleep", param_list));
	PrototypeTable.push_back(Func(Types::$i1, "sleep", param_list));
	param_list.clear();

	TU->addPrototype(new PrototypeAST(Types::$i1, "usclock", param_list));
	PrototypeTable.push_back(Func(Types::$i1, "usclock", param_list));
	param_list.clear();

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
	PrototypeAST *proto=visitFunctionDeclaration();
	if(proto){
		tunit->addPrototype(proto);
		return true;
	}

	//FunctionDefinition
	FunctionAST *func_def=visitFunctionDefinition();
	fprintf(stderr, "%d: %s %s\n", __LINE__, __func__, Tokens->getCurString().c_str());
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
		int index;
		if(index = find(PrototypeTable, Seq(proto->getType(), proto->getName())) != PrototypeTable.size() || find(FunctionTable, Func(proto->getType(), proto->getName(), proto->getParam())) != FunctionTable.size()) {
			fprintf(stderr, "Function：%s is redefined\n", proto->getName().c_str());
			SAFE_DELETE(proto);
			return NULL;
		}
		//PrototypeTable[index].setParamNum(proto->getParamNum());
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
	}else if(find(FunctionTable, Func(proto->getType(), proto->getName(), proto->getParam())) != FunctionTable.size()) {
			fprintf(stderr, "Function：%s is redefined\n" ,proto->getName().c_str()); 
			SAFE_DELETE(proto);
			return NULL;
	}

	int index = find(FunctionTable, Func(proto->getType(), proto->getName(), proto->getParam()));
	VariableTable.clear();
	FunctionStmtAST *func_stmt = visitFunctionStatement(proto);
	if(func_stmt){
		fprintf(stderr, "%d: %s %s\n", __LINE__, __func__, Tokens->getCurString().c_str());
		//FunctionTable[index].setParamNum(proto->getParamNum());
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
	Types var_type;

	//bkup index
	int bkup=Tokens->getCurIndex();

	bool type_class = false;
	if (Tokens->getCurString() == "$") {
		type_class = true;
		Tokens->getNextToken();
	}

	//type_specifier
	if(Tokens->getCurType() == TOK_TYPE){
		if (type_class) {
			var_type = str2Ty("$" + Tokens->getCurString());
		}else{
			var_type = str2Ty(Tokens->getCurString());
		}
		Tokens->getNextToken();
	}else{
		return NULL;
	}

	//IDENTIFIER
	if(Tokens->getCurType() == TOK_IDENTIFIER){
		func_name=Tokens->getCurString();
		Tokens->getNextToken();
	}else{
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}

	//'('
	if(Tokens->getCurString() == "("){
		Tokens->getNextToken();
	}else{
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}

	//parameter_list
	std::vector<Seq> param_list;
	Types param_type;
	bool is_first_param = true;
	while(true){
		//','
		if(!is_first_param && Tokens->getCurType() ==TOK_SYMBOL && Tokens->getCurString() ==","){
			Tokens->getNextToken();
		}

		if(Tokens->getCurType() == TOK_TYPE){
			param_type = str2Ty(Tokens->getCurString());
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
		return new PrototypeAST(var_type, func_name, param_list);
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
	int bkup=Tokens->getCurIndex();

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
		func_stmt->addStatement(vdecl);
		VariableTable.push_back(vdecl);
	}

	VariableDeclAST *var_decl;
	BaseAST *stmt;
	BaseAST *last_stmt;

	while(true) {
		if (Debbug) fprintf(stderr, "%d: %s %s\n", __LINE__, __func__, Tokens->getCurString().c_str());
		// 色々な文
		if(stmt = visitStatement(proto->getType())){
			last_stmt = stmt;
			func_stmt->addStatement(stmt);

		}else if(Tokens->getCurString() == "}") {
			break;

		}else{
			SAFE_DELETE(func_stmt);
			Tokens->applyTokenIndex(bkup);
			fprintf(stderr, "%d: error: unknown syntax %s\n", __LINE__, Tokens->getCurString().c_str());
			return NULL;
		}
	}

	//check if last statement is jump_statement
	if(!last_stmt || !llvm::isa<JumpStmtAST>(last_stmt)){
		SAFE_DELETE(func_stmt);
		Tokens->applyTokenIndex(bkup);
		fprintf(stderr, "%d: error: cannot find return statement\n", __LINE__);
		return NULL;
	}

	// }
	if(Tokens->getCurString() == "}"){
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
BaseAST *Parser::visitStatement(Types func_type = Types::all){
	BaseAST *stmt = NULL;

	//NULL Expression
	if(Tokens->getCurString() == ";"){
		Tokens->getNextToken();
		return new NullExprAST();
	// 変数代入か式
	}else if(stmt = visitAssignmentExpression(Types::all)){
		if(Tokens->getCurString() == ";"){
			Tokens->getNextToken();
			return stmt;
		}
	// 変数宣言
	}else if (stmt = visitVariableDeclaration()){
		VariableDeclAST* var_decl = (VariableDeclAST*)stmt;
		var_decl->setDeclType(VariableDeclAST::local);
		if (Debbug) fprintf(stderr, "%d: %s %s\n", __LINE__, Ty2str(var_decl->getType()[0]).c_str(), var_decl->getName().c_str());
		VariableTable.push_back(var_decl);

		if (Tokens->getCurString() == "="){
			Tokens->ungetToken(1);
			return var_decl;
		}
		return stmt;
	// if文
	}else if(stmt = visitIfExpression()){
		return stmt;
	// while文
	}else if(stmt = visitWhileExpression()) {
		return stmt;
	// return文
	}else if(stmt = visitJumpStatement(func_type)){
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
	Types type;
	int size;
	bool type_class = false;

	// 型
	if(Tokens->getCurString() == "$") {
		type_class = true;
		Tokens->getNextToken();
	}

	if (Tokens->getCurType() == TOK_TYPE){
		if (type_class) {
			type = str2Ty("$" + Tokens->getCurString());
		}else{
			type = str2Ty(Tokens->getCurString());
		}
		Tokens->getNextToken();
	}else{
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}

	//IDENTIFIER
	if(Tokens->getCurType() == TOK_IDENTIFIER){
		name = Tokens->getCurString();
		Tokens->getNextToken();
	}else{
		Tokens->applyTokenIndex(bkup);
		fprintf(stderr, "%d: error: cannot find identifier of variable\n", __LINE__);
		return NULL;
	}

	//配列の変数かどうか
	//[integer]
	if(Tokens->getCurString() == "["){
		Tokens->getNextToken();
		if (Tokens->getCurType() != TOK_DIGIT){
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}

		size = atoi(Tokens->getCurString().c_str());
		Tokens->getNextToken();
		if (Tokens->getCurString() != "]"){
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
		Tokens->getNextToken();
	}

	for (int i = 0;i < VariableTable.size();i++) {
		if (VariableTable[i]->getName() == name) {
			fprintf(stderr, "%d: error: redefinition of '%s' with a different type: '%s' vs '%s'\n", __LINE__, Ty2str(VariableTable[i]->getType()[0]).c_str(), Ty2str(VariableTable[i]->getType()[0]).c_str(), Ty2str(type).c_str());
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
		fprintf(stderr, "%d: error: cannot find semicoron\n", __LINE__);
		return NULL;
	}
}



/**
  * JumpStatement用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitJumpStatement(Types func_type){
	if (Debbug) fprintf(stderr, "%d: %s %s\n", __LINE__, __func__, Tokens->getCurString().c_str());
	int bkup=Tokens->getCurIndex();
	BaseAST *expr;

	if(Tokens->getCurType() != TOK_RETURN){
		return NULL;
	}
	Tokens->getNextToken();

	if(!(expr = visitAssignmentExpression(func_type))){
		if (Debbug) fprintf(stderr, "%d: return %s\n", __LINE__, Tokens->getCurString().c_str());
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}

	//if (expr->getValueID() == BinaryExprID){
	//auto binary_expr = (BinaryExprAST*)expr;
	//if (return_type != func_type) {
	//	fprintf(stderr, "%d: a different type: '%s' vs '%s'\n", __LINE__, binary_expr->getType(), Type);
	//}

	if (Debbug) fprintf(stderr, "%d: return %s\n", __LINE__, Tokens->getCurString().c_str());
	if(Tokens->getCurString() == ";"){
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
BaseAST *Parser::visitIfExpression(){
	int bkup = Tokens->getCurIndex();

	std::vector<BaseAST*> ThenStmt, ElseStmt;
	BaseAST *stmt, *CondStmt;

	if (Tokens->getCurType() != TOK_IF) {
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();
	if (Debbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);

	if (!(stmt = visitAdditiveExpression(NULL, Types::$i1))){
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	CondStmt = stmt;

	if (Tokens->getCurString() != "{") {
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();

	while(true) {
		if (stmt = visitStatement()) {
			if (Debbug) fprintf(stderr, "%d: %s %s\n", __LINE__, __func__, Tokens->getCurString().c_str());
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
	if (Debbug) fprintf(stderr, "%d: %s %s\n", __LINE__, __func__, Tokens->getCurString().c_str());

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
			if (Debbug) fprintf(stderr, "%d: %s\n", __LINE__, Tokens->getCurString().c_str());
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
	if (Debbug) fprintf(stderr, "%d: %s\n", __LINE__, Tokens->getCurString().c_str());

	return new IfExprAST(CondStmt, ThenStmt, ElseStmt);

}



/**
  * WhileExpression用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitWhileExpression(){
	int bkup = Tokens->getCurIndex();

	std::vector<BaseAST*> LoopStmt;
	BaseAST *stmt, *CondStmt;

	if (Tokens->getCurType() != TOK_WHILE) {
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();

	if (!(stmt = visitAdditiveExpression(NULL, Types::$i1))){
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	CondStmt = stmt;

	if (Tokens->getCurString() != "{") {
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();

	while(true) {
		if (stmt = visitStatement()) {
			if (Debbug) fprintf(stderr, "%d: while %s\n", __LINE__, Tokens->getCurString().c_str());
			LoopStmt.push_back(stmt);
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
	if (Debbug) fprintf(stderr, "%d: %s\n", __LINE__, Tokens->getCurString().c_str());
	return new WhileExprAST(CondStmt, LoopStmt);
}


/**
  * AssignmentExpression用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitAssignmentExpression(Types Type = Types::all){
	int bkup = Tokens->getCurIndex();

	//	| IDENTIFIER '=' additive_expression
	BaseAST *lhs;

	if(Tokens->getCurType() == TOK_IDENTIFIER){
		//変数が宣言されているか確認
		bool all_confirm = false;
		for (int i = 0;i < VariableTable.size();i++) {
			if (VariableTable[i]->getName() == Tokens->getCurString()) {
				if (Debbug) fprintf(stderr, "%d: assign %s\n", __LINE__, VariableTable[i]->getName().c_str());
				Tokens->getNextToken();

				int Index = 0;
				// array type
				if (Tokens->getCurString() == "["){
					Tokens->getNextToken();
					Index = atoi(Tokens->getCurString().c_str());
					Tokens->getNextToken();
					Tokens->getNextToken();
				}

				Type = VariableTable[i]->getType()[0];
				lhs = new VariableAST(VariableTable[i], Index);
				BaseAST *rhs;
				if(Tokens->getCurType() == TOK_SYMBOL &&
						Tokens->getCurString() == "="){
					Tokens->getNextToken();

					// 配列か変数か
					if (VariableTable[i]->getSize()){
						if (rhs = visitArrayExpression(Type, VariableTable[i]->getSize())) {
							Tokens->getNextToken();
							return new BinaryExprAST("=", lhs, rhs, Type);
						}else{
							SAFE_DELETE(lhs);
							Tokens->applyTokenIndex(bkup);
						}
					}else{
						for (int j = 0;j < VariableTable[i]->getType().size();j++) {
							if (rhs = visitAdditiveExpression(NULL, VariableTable[i]->getType()[j])) {
								Tokens->getNextToken();
								return new BinaryExprAST("=", lhs, rhs, Type);
							}
						}
						SAFE_DELETE(lhs);
						Tokens->applyTokenIndex(bkup);
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
	BaseAST *add_expr=visitAdditiveExpression(NULL, Type);
	if(add_expr)
		return add_expr;

	return NULL;
}



/**
  * AdditiveExpression用構文解析メソッド
  * @param lhs(左辺),初回呼び出し時はNULL
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitAdditiveExpression(BaseAST *lhs, Types Type){
	//bkup index
	int bkup=Tokens->getCurIndex();

	if(!lhs)
		lhs = visitMultiplicativeExpression(NULL, Type);
	BaseAST *rhs;

	if(!lhs)
		return NULL;

	//+
	if(Tokens->getCurType() ==TOK_SYMBOL &&
			Tokens->getCurString() =="+"){
		Tokens->getNextToken();
		rhs=visitMultiplicativeExpression(NULL, Type);
		if(rhs){
			return visitAdditiveExpression(
					new BinaryExprAST("+", lhs, rhs, Types::$i32),
					Type);
		}else{
			SAFE_DELETE(lhs);
			Tokens->applyTokenIndex(bkup);
			fprintf(stderr, "%d: error: rhs of plus is nothing\n", __LINE__);
			return NULL;
		}
	//-
	}else if(Tokens->getCurType() ==TOK_SYMBOL &&
			Tokens->getCurString() =="-"){
		Tokens->getNextToken();
		rhs=visitMultiplicativeExpression(NULL, Type);
		if(rhs){
			return visitAdditiveExpression(
					new BinaryExprAST("-", lhs, rhs, Types::$i32),
					Type);
		}else{
			SAFE_DELETE(lhs);
			Tokens->applyTokenIndex(bkup);
			fprintf(stderr, "%d: error: rhs of minus is nothing\n", __LINE__);
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
BaseAST *Parser::visitMultiplicativeExpression(BaseAST *lhs, Types Type){
	//bkup index
	int bkup=Tokens->getCurIndex();

	if(!lhs)
		lhs=visitPostfixExpression(Type);

	BaseAST *rhs;

	if(!lhs)
		return NULL;

	// *
	if(Tokens->getCurType() ==TOK_SYMBOL &&
			Tokens->getCurString() == "*"){
		Tokens->getNextToken();
		rhs=visitPostfixExpression(Type);
		if(rhs){
			return visitMultiplicativeExpression(
					new BinaryExprAST("*", lhs, rhs, Types::$i32),
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
		rhs=visitPostfixExpression(Type);
		if(rhs){
			return visitMultiplicativeExpression(
					new BinaryExprAST("/", lhs, rhs, Types::$i32),
					Type);
		}else{
			SAFE_DELETE(lhs);
			Tokens->applyTokenIndex(bkup);
			fprintf(stderr, "%d: error: rhs of divide is nothing\n", __LINE__);
			return NULL;
		}
	}
	return lhs;
}


/**
  * PostfixExpression用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitPostfixExpression(Types Type){
	int bkup=Tokens->getCurIndex();

	//primary_expression
	BaseAST *prim_expr=visitPrimaryExpression(Type);
	if(prim_expr)
		return prim_expr;

	// 関数呼び出し用
	//FUNCTION_IDENTIFIER
	if(Tokens->getCurType() != TOK_IDENTIFIER)
		return NULL;

	//関数名取得
	std::string Callee = Tokens->getCurString();
	if (Debbug) fprintf(stderr, "%d: call function %s\n", __LINE__, Tokens->getCurString().c_str());
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

	BaseAST *assign_expr=visitAssignmentExpression();
	if(assign_expr){
		args.push_back(assign_expr);
		while(Tokens->getCurType() == TOK_SYMBOL &&
				Tokens->getCurString() == ","){
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
	int index;
	if(index = find(PrototypeTable, Seq(Type, Tokens->getCurString())) != PrototypeTable.size()){
		Func func = PrototypeTable[index];
		for(int i=0;i<args.size();i++) {
			if (args[i]->getType() != func.param[i].Type) {
				SAFE_DELETE(args[i]);
				fprintf(stderr, "%d: error: no match for function param '%s'\n", __LINE__, Callee.c_str());
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}
		}
	}else if(index = find(FunctionTable, Seq(Type, Tokens->getCurString())) != FunctionTable.size()){
		Func func = FunctionTable[index];
		for(int i=0;i<args.size();i++) {
			if (args[i]->getType() != func.param[i].Type) {
				SAFE_DELETE(args[i]);
				fprintf(stderr, "%d: error: no match for function param '%s'\n", __LINE__, Callee.c_str());
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}
		}
	}else{
		fprintf(stderr, "%d: error: undefined function\n", __LINE__);
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}

	//RIGHT PALEN
	if(Tokens->getCurType() == TOK_SYMBOL &&
			Tokens->getCurString() == ")"){
		Tokens->getNextToken();
		return new CallExprAST(Types::$i1, Callee, args);
	}else{
		for(int i=0;i<args.size();i++)
			SAFE_DELETE(args[i]);
		Tokens->applyTokenIndex(bkup);
		fprintf(stderr, "%d: error: RIGHT PALEN is nothing\n", __LINE__);
		return NULL;
	}
}


/**
  * PrimaryExpression用構文解析メソッド
  * @return 解析成功時：AST　失敗時：NULL
  */
BaseAST *Parser::visitPrimaryExpression(Types Type){
	int bkup=Tokens->getCurIndex();

	// 数値、真偽値、条件式、変数呼び出しなど
	if (Type == Types::i32 || Type == Types::$i32 || Type == Types::all) {
		//integer
		if(Tokens->getCurType() == TOK_DIGIT){
			int val = Tokens->getCurNumVal();
			Tokens->getNextToken();
			return new NumberAST(val);

		//integer(-)
		}else if(Tokens->getCurType() == TOK_SYMBOL &&
				Tokens->getCurString() == "-"){
			Tokens->getNextToken();
			if(Tokens->getCurType() == TOK_DIGIT){
				int val=Tokens->getCurNumVal();
				Tokens->getNextToken();
				return new NumberAST(-val);
			}else{
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}
		}else if(Tokens->getCurString() == "None"){
			if (Type == Types::$i32) {
				fprintf(stderr, "error: $i32 don't substitute None\n");
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}
			Tokens->getNextToken();
			return new NoneAST(Type);
		}

	}

	if (Type == Types::i1 || Type == Types::$i1 || Type == Types::all) {
		// true / false
		if(Tokens->getCurType() == TOK_TRUE) {
			Tokens->getNextToken();
			return new BooleanAST(true);
		}else if(Tokens->getCurType() == TOK_FALSE) {
			Tokens->getNextToken();
			return new BooleanAST(false);
		//integer
		}else if(Tokens->getCurType() == TOK_DIGIT){
			Tokens->getNextToken();
			return new BooleanAST(true);
		//integer(-)
		}else if(Tokens->getCurType() == TOK_SYMBOL &&
				Tokens->getCurString() == "-"){
			Tokens->getNextToken();
			if(Tokens->getCurType() == TOK_DIGIT){
				Tokens->getNextToken();
				return new BooleanAST(true);
			}else{
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}
		}else if(Tokens->getCurString() == "None"){
			if (Type == Types::$i1) {
				fprintf(stderr, "error: $i1 don't substitute None\n");
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}
			Tokens->getNextToken();
			return new NoneAST(Type);
		}

		BaseAST *expr;
		BaseAST *lhs;
		if (lhs = visitAdditiveExpression(NULL, Types::$i32)) {
			if (Tokens->getCurString() == "<" ||
				Tokens->getCurString() == ">" ||
				Tokens->getCurString() == "<=" ||
				Tokens->getCurString() == ">=" ||
				Tokens->getCurString() == "==" ||
				Tokens->getCurString() == "!=") {
				std::string op = Tokens->getCurString();
				BaseAST *rhs;

				Tokens->getNextToken();
				if (rhs = visitAdditiveExpression(NULL, Types::$i32)) {
					return new BinaryExprAST(op, lhs, rhs, Types::$i1);
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
	}

	//VARIABLE_IDENTIFIER
	if(Tokens->getCurType() == TOK_IDENTIFIER) {
		for (int i = 0;i < VariableTable.size();i++) {
			if (VariableTable[i]->getName() == Tokens->getCurString()) {
				Tokens->getNextToken();
				if(Tokens->getCurString() == "?") {
					if (Type == Types::i32) {
						Type = Types::$i32;
						VariableTable[i]->setType(Types::$i32);
					}
					Tokens->getNextToken();
				}
				return new VariableAST(VariableTable[i]);
			}
		}
	// '(' expression ')'
	}else if(Tokens->getCurType() == TOK_SYMBOL &&
			Tokens->getCurString() == "("){
		Tokens->getNextToken();

		//expression
		BaseAST *assign_expr;
		if(!(assign_expr=visitAssignmentExpression())){
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}

		//RIGHT PALEN
		if(Tokens->getCurString() != ")"){
			SAFE_DELETE(assign_expr);
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}

		Tokens->getNextToken();
		return assign_expr;
	}
	return NULL;
}


/**
  * ArrayExpression用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitArrayExpression(Types Type, int Size){
	int bkup = Tokens->getCurIndex();

	if (Tokens->getCurString() != "{") {
		Tokens->applyTokenIndex(bkup);
		fprintf(stderr, "error at lhs is a array, rhs is a variable\n");
		return NULL;
	}
	Tokens->getNextToken();

	BaseAST *stmt;
	std::vector<BaseAST*> ElementStmt;
	while(true) {
		if (stmt = visitAdditiveExpression(NULL, Type)) {
			ElementStmt.push_back(stmt);
			Tokens->getNextToken();
		}
		if (Tokens->getCurString() == ",") {
			Tokens->getNextToken();
		}else if (Tokens->getCurString() == "}") {
			Tokens->getNextToken();
			return new ArrayAST(Type, Size, ElementStmt);
		}else{
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
	}
}




