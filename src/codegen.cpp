#include "codegen.hpp"

/**
  * コンストラクタ
  */
CodeGen::CodeGen(){
	Builder = new IRBuilder<>(getGlobalContext());
	Mod = NULL;
}

/**
  * デストラクタ
  */
CodeGen::~CodeGen(){
	SAFE_DELETE(Builder);
	SAFE_DELETE(Mod);
}


/**
  * コード生成実行
  * @param  TranslationUnitAST　Module名(入力ファイル名)
  * @return 成功時：true　失敗時:false
  */
bool CodeGen::doCodeGen(TranslationUnitAST &tunit, std::string name, 
		std::string link_file, bool with_jit=false){

	if(!generateTranslationUnit(tunit, name)){
		return false;
	}

	//LinkFileの指定があったらModuleをリンク
	if(!link_file.empty() && !linkModule(Mod, link_file) )
		return false;

	//JITのフラグが立っていたらJIT
	if(with_jit){
		ExecutionEngine *EE = EngineBuilder(Mod).create();
		EngineBuilder(Mod).create();
			Function *F;
		if(!(F=Mod->getFunction("main")))
			return false;

		int (*fp)() = (int (*)())EE->getPointerToFunction(F);
		fprintf(stderr,"%d\n",fp());
	}

	return true;
}


/**
  * Module取得
  */
Module &CodeGen::getModule(){
	if(Mod)
		return *Mod;
	else
		return *(new Module("null", getGlobalContext()));
}


/**
  * Module生成メソッド
  * @param  TranslationUnitAST Module名(入力ファイル名)
  * @return 成功時：true　失敗時：false　
  */
bool CodeGen::generateTranslationUnit(TranslationUnitAST &tunit, std::string name){
	Mod = new Module(name, getGlobalContext());
	//funtion declaration
	for(int i=0; ; i++){
		PrototypeAST *proto=tunit.getPrototype(i);
		if(!proto)
			break;
		else if(!generatePrototype(proto, Mod)){
			SAFE_DELETE(Mod);
			return false;
		}
	}

	//function definition
	for(int i=0; ; i++){
		FunctionAST *func = tunit.getFunction(i);
		if(!func)
			break;
		else if(!(generateFunctionDefinition(func, Mod))){
			SAFE_DELETE(Mod);
			return false;
		}
	}

	return true;
}


/**
  * 関数定義生成メソッド
  * @param  FunctionAST Module
  * @return 生成したFunctionのポインタ
  */
Function *CodeGen::generateFunctionDefinition(FunctionAST *func_ast,
		Module *mod){
	Function *func = generatePrototype(func_ast->getPrototype(), mod);
	if(!func){
		return NULL;
	}
	CurFunc = func;
	BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", func);
	Builder->SetInsertPoint(bblock);
	generateFunctionStatement(func_ast->getBody());

	return func;
}


/**
  * 関数宣言生成メソッド
  * @param  PrototypeAST, Module
  * @return 生成したFunctionのポインタ
  */
Function *CodeGen::generatePrototype(PrototypeAST *proto, Module *mod){
	//already declared?
	Function *func=mod->getFunction(proto->getName());
	if(func){
		if(func->arg_size()==proto->getParamNum() && 
				func->empty()){
			return func;
		}else{
			fprintf(stderr, "error::function %s is redefined",proto->getName().c_str());
			return NULL;
		}
	}

	//create arg_types
	std::vector<Type*> int_types(proto->getParamNum(),
								Type::getInt32Ty(getGlobalContext()));

	//create func type
	FunctionType *func_type = FunctionType::get(
							Type::getInt32Ty(getGlobalContext()),
							int_types,false
							);
	//create function
	func = Function::Create(func_type, 
							Function::ExternalLinkage,
							proto->getName(),
							mod);

	//set names
	Function::arg_iterator arg_iter=func->arg_begin();
	for(int i=0; i<proto->getParamNum(); i++){
		arg_iter->setName(proto->getParamName(i).append("_arg"));
		++arg_iter;
	}

	return func;
}


/**
  * 関数生成メソッド
  * 変数宣言、ステートメントの順に生成　
  * @param  FunctionStmtAST
  * @return 最後に生成したValueのポインタ
  */
Value *CodeGen::generateFunctionStatement(FunctionStmtAST *func_stmt){
	//insert variable decls
	VariableDeclAST *vdecl;
	Value *v=NULL;
	for(int i=0; ; i++){
		//最後まで見たら終了
		if(!func_stmt->getVariableDecl(i))
			break;

		//create alloca
		vdecl = dyn_cast<VariableDeclAST>(func_stmt->getVariableDecl(i));
		v = generateVariableDeclaration(vdecl);
	}

	//insert expr statement
	BaseAST *stmt;
	for(int i=0; ; i++){
		stmt=func_stmt->getStatement(i);
		if(!stmt)
			break;
		else if(isa<IfExprAST>(stmt))
			v = generateIfExpr(dyn_cast<IfExprAST>(stmt));
		else if(!isa<NullExprAST>(stmt))
			v = generateStatement(stmt);
	}

	return v;
}


/**
  * 変数宣言(alloca命令)生成メソッド
  * @param VariableDeclAST
  * @return 生成したValueのポインタ
  */
Value *CodeGen::generateVariableDeclaration(VariableDeclAST *vdecl){
	//create alloca
	AllocaInst *alloca;
	if (vdecl->getType() == "i32" || vdecl->getType() == "int") {
		alloca = Builder->CreateAlloca(
			Type::getInt32Ty(getGlobalContext()),
			0,
			vdecl->getName());
	}else if (vdecl->getType() == "bool") {
		alloca = Builder->CreateAlloca(
			Type::getInt1Ty(getGlobalContext()),
			0,
			vdecl->getName());
	}

	//if args alloca
	if(vdecl->getDeclType() == VariableDeclAST::param){
		//store args
		ValueSymbolTable &vs_table = CurFunc->getValueSymbolTable();
		Builder->CreateStore(vs_table.lookup(vdecl->getName().append("_arg")), alloca);
	}
	//ValueMap[vdecl->getName()]=alloca;
	return alloca;
}



// IfExpr
Value *CodeGen::generateIfExpr(IfExprAST *if_expr) {
	Value *CondV;
	CondV = Builder->CreateFCmpONE(
			ConstantFP::get(getGlobalContext(), APFloat(3.0)),
			ConstantFP::get(getGlobalContext(), APFloat(0.0)), "ifcond");

	Function *function = Builder->GetInsertBlock()->getParent();

	BasicBlock *ThenBB = BasicBlock::Create(getGlobalContext(), "then", function);
	BasicBlock *ElseBB = BasicBlock::Create(getGlobalContext(), "else");
	BasicBlock *MergeBB = BasicBlock::Create(getGlobalContext(), "ifcont");

	Builder->CreateCondBr(CondV, ThenBB, ElseBB);

	Builder->SetInsertPoint(ThenBB);

	// if文がネストしたとき
	//auto lastThenBB = Builder->GetInsertBlock();

	ThenBB = Builder->GetInsertBlock();
	for (int i = 0;i < if_expr->getThen().size();i++)
		generateStatement(if_expr->getThen()[i]);
	Builder->CreateBr(MergeBB);

	function->getBasicBlockList().push_back(ElseBB);
	Builder->SetInsertPoint(ElseBB);

	ElseBB = Builder->GetInsertBlock();
	for (int i = 0;i < if_expr->getElse().size();i++)
		generateStatement(if_expr->getElse()[i]);
	Builder->CreateBr(MergeBB);

	function->getBasicBlockList().push_back(MergeBB);
	Builder->SetInsertPoint(MergeBB);

	//PHINode *PN = Builder->CreatePHI(Type::getDoubleTy(getGlobalContext()), 2, "iftmp");

	//PN->addIncoming(ThenV, ThenBB);
	//PN->addIncoming(ElseV, ElseBB);
	return CondV;
}


/**
  * ステートメント生成メソッド
  * 実際にはASTの種類を確認して各種生成メソッドを呼び出し
  * @param  JumpStmtAST
  * @return 生成したValueのポインタ
  */
Value *CodeGen::generateStatement(BaseAST *stmt){
	if(isa<BinaryExprAST>(stmt)){
		return generateBinaryExpression(dyn_cast<BinaryExprAST>(stmt));
	}else if(isa<CallExprAST>(stmt)){
		return generateCallExpression(dyn_cast<CallExprAST>(stmt));
	}else if(isa<JumpStmtAST>(stmt)){
		return generateJumpStatement(dyn_cast<JumpStmtAST>(stmt));
	}else{
		return NULL;
	}
}


/**
  * 二項演算生成メソッド
  * @param  JumpStmtAST
  * @return 生成したValueのポインタ
  */
Value *CodeGen::generateBinaryExpression(BinaryExprAST *bin_expr){
	BaseAST *lhs=bin_expr->getLHS();
	BaseAST *rhs=bin_expr->getRHS();

	Value *lhs_v;
	Value *rhs_v;

	//assignment
	if(bin_expr->getOp()=="="){
		//lhs is variable
		VariableAST *lhs_var=dyn_cast<VariableAST>(lhs);
		ValueSymbolTable &vs_table = CurFunc->getValueSymbolTable();
		lhs_v = vs_table.lookup(lhs_var->getName());

	//other operand
	}else{
		//lhs=?
		//Binary?
		if(isa<BinaryExprAST>(lhs)){
			lhs_v=generateBinaryExpression(dyn_cast<BinaryExprAST>(lhs));

		//Variable?
        }else if(isa<VariableAST>(lhs)){
			lhs_v=generateVariable(dyn_cast<VariableAST>(lhs));

		//Number?
        }else if(isa<NumberAST>(lhs)){
			NumberAST *num = dyn_cast<NumberAST>(lhs);
			lhs_v = generateNumber(num->getValue());

		//Boolean?
		}else if(isa<BooleanAST>(rhs)){
			BooleanAST *boolean = dyn_cast<BooleanAST>(rhs);
			rhs_v = generateBoolean(boolean->getValue());
		}
	}


	//create rhs value
	if(isa<BinaryExprAST>(rhs)){
		rhs_v=generateBinaryExpression(dyn_cast<BinaryExprAST>(rhs));

	//CallExpr?
    }else if(isa<CallExprAST>(rhs)){
		rhs_v=generateCallExpression(dyn_cast<CallExprAST>(rhs));

	//Variable?
    }else if(isa<VariableAST>(rhs)){
		rhs_v=generateVariable(dyn_cast<VariableAST>(rhs));

	//Number?
    }else if(isa<NumberAST>(rhs)){
		NumberAST *num = dyn_cast<NumberAST>(rhs);
		rhs_v=generateNumber(num->getValue());

	//Boolean?
	}else if(isa<BooleanAST>(rhs)){
		BooleanAST *boolean = dyn_cast<BooleanAST>(rhs);
		rhs_v = generateBoolean(boolean->getValue());
	}



	if(bin_expr->getOp()=="="){
		//store
		return Builder->CreateStore(rhs_v, lhs_v);
	}else if(bin_expr->getOp()=="+"){
		//add
		return Builder->CreateAdd(lhs_v, rhs_v, "add_tmp");
	}else if(bin_expr->getOp()=="-"){
		//sub
		return Builder->CreateSub(lhs_v, rhs_v, "sub_tmp");
	}else if(bin_expr->getOp()=="*"){
		//mul
		return Builder->CreateMul(lhs_v, rhs_v, "mul_tmp");
	}else if(bin_expr->getOp()=="/"){
		//div
		return Builder->CreateSDiv(lhs_v, rhs_v, "div_tmp");
	}
}


/**
  * 関数呼び出し(Call命令)生成メソッド
  * @param CallExprAST
  * @return 生成したValueのポインタ　
  */
Value *CodeGen::generateCallExpression(CallExprAST *call_expr){
	std::vector<Value*> arg_vec;
	BaseAST *arg;
	Value *arg_v;
	ValueSymbolTable &vs_table = CurFunc->getValueSymbolTable();
	for(int i=0; ; i++){
		if(!(arg=call_expr->getArgs(i)))
			break;

		//isCall
		if(isa<CallExprAST>(arg))
			arg_v = generateCallExpression(dyn_cast<CallExprAST>(arg));

		//isBinaryExpr
		else if(isa<BinaryExprAST>(arg)){
			BinaryExprAST *bin_expr = dyn_cast<BinaryExprAST>(arg);

			//二項演算命令を生成
			arg_v = generateBinaryExpression(dyn_cast<BinaryExprAST>(arg));

			//代入の時はLoad命令を追加
			if(bin_expr->getOp() == "="){
				VariableAST *var = dyn_cast<VariableAST>(bin_expr->getLHS());
				arg_v = Builder->CreateLoad(vs_table.lookup(var->getName()), "arg_val");
			}
		}

		//isVar
		else if(isa<VariableAST>(arg))
			arg_v = generateVariable(dyn_cast<VariableAST>(arg));
		
		//isNumber
		else if(isa<NumberAST>(arg)){
			NumberAST *num=dyn_cast<NumberAST>(arg);
			arg_v = generateNumber(num->getValue());
		}

		//isBoolean
		else if(isa<BooleanAST>(arg)) {
			BooleanAST *boolean = dyn_cast<BooleanAST>(arg);
			arg_v = generateBoolean(boolean->getValue());
		}

		arg_vec.push_back(arg_v);
	}
	return Builder->CreateCall(Mod->getFunction(call_expr->getCallee()),
										arg_vec,"call_tmp");
}


/**
  * ジャンプ(今回はreturn命令のみ)生成メソッド
  * @param  JumpStmtAST
  * @return 生成したValueのポインタ
  */
Value *CodeGen::generateJumpStatement(JumpStmtAST *jump_stmt){
	BaseAST *expr = jump_stmt->getExpr();
	Value *ret_v;
	if(isa<BinaryExprAST>(expr)){
		ret_v = generateBinaryExpression(dyn_cast<BinaryExprAST>(expr));

	}else if(isa<VariableAST>(expr)){
		VariableAST *var=dyn_cast<VariableAST>(expr);
		ret_v = generateVariable(var);

	}else if(isa<NumberAST>(expr)){
		NumberAST *num=dyn_cast<NumberAST>(expr);
		ret_v = generateNumber(num->getValue());

	}else if(isa<BooleanAST>(expr)){
		BooleanAST *boolean = dyn_cast<BooleanAST>(expr);
		ret_v = generateBoolean(boolean->getValue());
	}

	Builder->CreateRet(ret_v);
}


/**
  * 変数参照(load命令)生成メソッド
  * @param VariableAST
  * @return  生成したValueのポインタ
  */
Value *CodeGen::generateVariable(VariableAST *var){
	ValueSymbolTable &vs_table = CurFunc->getValueSymbolTable();
	return Builder->CreateLoad(vs_table.lookup(var->getName()), "var_tmp");
}


// Number
Value *CodeGen::generateNumber(int value){
	return ConstantInt::get(
			Type::getInt32Ty(getGlobalContext()),
			value);
}


// Boolean
Value *CodeGen::generateBoolean(bool value) {
	return ConstantInt::get(
			Type::getInt1Ty(getGlobalContext()),
			value);
}



bool CodeGen::linkModule(Module *dest, std::string file_name){
	SMDiagnostic err;
	Module *link_mod = ParseIRFile(file_name, err, getGlobalContext());
	if(!link_mod)
		return false;

	std::string err_msg;
	if(Linker::LinkModules(dest, link_mod, Linker::DestroySource, &err_msg))
		return false;

	SAFE_DELETE(link_mod);

	return true;
}

