#include "codegen.hpp"

bool llvmDebbug = true;

/**
  * コンストラクタ
  */
CodeGen::CodeGen(){
	Builder = new IRBuilder<>(GlobalContext);
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
bool CodeGen::doCodeGen(TranslationUnitAST &tunit, std::string name, std::string link_file, bool with_jit=false){
	if(!generateTranslationUnit(tunit, name)){
		return false;
	}

	//LinkFileの指定があったらModuleをリンク
	if(!link_file.empty() && !linkModule(Mod, link_file) )
		return false;

	//JITのフラグが立っていたらJIT
	if(with_jit){
		ExecutionEngine *EE = EngineBuilder().create();
		EngineBuilder().create();
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
		return *(new Module("null", GlobalContext));
}


Type *CodeGen::generateType(Types type) {
	if (type.getPrimType() == Type_void) {
		return Type::getVoidTy(GlobalContext);
	}else if (type.getPrimType() == Type_int) {
		if (type.getBits() == 1) {
			return Type::getInt1Ty(GlobalContext);
		}else if (type.getBits() == 8) {
			return Type::getInt8Ty(GlobalContext);
		}else if (type.getBits() == 16) {
			return Type::getInt16Ty(GlobalContext);
		}else if (type.getBits() == 32) {
			return Type::getInt32Ty(GlobalContext);
		}else if (type.getBits() == 64) {
			return Type::getInt64Ty(GlobalContext);
		}else{
			return Type::getIntNTy(GlobalContext, type.getBits());
		}
	}else if (type.getPrimType() == Type_float) {
		if (type.getBits() == 16) {
			return Type::getHalfTy(GlobalContext);
		}else if (type.getBits() == 32) {
			return Type::getFloatTy(GlobalContext);
		}else if (type.getBits() == 64) {
			return Type::getDoubleTy(GlobalContext);
		}else if (type.getBits() == 128) {
			return Type::getFP128Ty(GlobalContext);
		}else if (type.getBits() == 80) {
			return Type::getX86_FP80Ty(GlobalContext);
		}else{
			fprintf(stderr, "Type is not found\n");
			return Type::getFloatTy(GlobalContext);
		}
	}else if (type.getPrimType() == Type_bool) {
		return Type::getInt1Ty(GlobalContext);
	}else{
		fprintf(stderr, "Type is not found\n");
		return Type::getInt32Ty(GlobalContext);
	}
}


/**
  * Module生成メソッド
  * @param  TranslationUnitAST Module名(入力ファイル名)
  * @return 成功時：true　失敗時：false　
  */
bool CodeGen::generateTranslationUnit(TranslationUnitAST &tunit, std::string name){
	Mod = new Module(name, GlobalContext);
	//funtion declaration
	for(int i=0; ; i++){
		PrototypeAST *proto = tunit.getPrototype(i);
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
	if(!func)
		return NULL;
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);
	CurFunc = func;
	CurBB = BasicBlock::Create(GlobalContext, "entry", func);
	Builder->SetInsertPoint(CurBB);
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
	Function *func = mod->getFunction(proto->getName());
	if(func){
		if(func->arg_size() == proto->getParamNum() && func->empty()){
			return func;
		}else{
			fprintf(stderr, "error: function %s is redefined",proto->getName().c_str());
			return NULL;
		}
	}

	//create arg_types
	std::vector<Type*> int_types(proto->getParamNum(),
								Type::getInt32Ty(GlobalContext));

	//create func type
	FunctionType *func_type = FunctionType::get(
							Type::getInt32Ty(GlobalContext),
							int_types,
							false);
	//create function
	func = Function::Create(func_type, 
							Function::ExternalLinkage,
							proto->getName(),
							mod);

	//set names
	Function::arg_iterator arg_iter = func->arg_begin();
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
	Value *v = NULL;

	//insert expr statement
	BaseAST *stmt;
	for(int i=0; ; i++){
		if (llvmDebbug) fprintf(stderr, "%d\n", i);
		stmt=func_stmt->getStatement(i);
		if(!stmt)
			break;
		v = generateStatement(stmt);
	}
	if (llvmDebbug) fprintf(stderr, "finish\n");

	return v;
}


/**
  * 変数宣言(alloca命令)生成メソッド
  * @param VariableDeclAST
  * @return 生成したValueのポインタ
  */
Value *CodeGen::generateVariableDeclaration(VariableDeclAST *vdecl){
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);
	//create alloca
	Value *alloca;
	Type* type = generateType(vdecl->getType());
	alloca = Builder->CreateAlloca(type, nullptr, vdecl->getName());

	//if args alloca
	if(vdecl->getDeclType() == VariableDeclAST::param){
		//store args
		Builder->CreateStore(CurFunc->getValueSymbolTable()->lookup(vdecl->getName().append("_arg")), alloca);
	}
	return alloca;
}


/**
  * If文生成メソッド
  * @param IfExprAST
  * @return 生成したValueのポインタ
  */
Value *CodeGen::generateIfExpr(IfExprAST *if_expr) {
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);

	Value *CondV = generateCondition(if_expr->getCond());

	Function *function = Builder->GetInsertBlock()->getParent();
	BasicBlock *ThenBB = BasicBlock::Create(GlobalContext, "then", function);
	BasicBlock *ElseBB = BasicBlock::Create(GlobalContext, "else");
	BasicBlock *MergeBB = BasicBlock::Create(GlobalContext, "ifcont");

	Builder->CreateCondBr(CondV, ThenBB, ElseBB);

	Builder->SetInsertPoint(ThenBB);
	// if文がネストしたとき
	//auto lastThenBB = Builder->GetInsertBlock();

	ThenBB = Builder->GetInsertBlock();
	for (int i = 0;;i++)
		if (if_expr->getThen(i)) break;
		else generateStatement(if_expr->getThen(i));
	Builder->CreateBr(MergeBB);

	function->getBasicBlockList().push_back(ElseBB);
	Builder->SetInsertPoint(ElseBB);

	ElseBB = Builder->GetInsertBlock();
	for (int i = 0;;i++)
		if (!if_expr->getElse(i)) break;
		else generateStatement(if_expr->getElse(i));
	Builder->CreateBr(MergeBB);

	function->getBasicBlockList().push_back(MergeBB);
	Builder->SetInsertPoint(MergeBB);
	return CondV;
}


/**
  * While文生成メソッド
  * @param WhileExprAST
  * @return 生成したValueのポインタ
  */
Value *CodeGen::generateWhileExpr(WhileExprAST *while_expr) {
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);
	Value *CondV = generateCondition(while_expr->getCond());

	Function *function = Builder->GetInsertBlock()->getParent();

	BasicBlock *PreheaderBB = Builder->GetInsertBlock();
	BasicBlock *LoopBB = BasicBlock::Create(GlobalContext, "loop", function);
	BasicBlock *AfterBB = BasicBlock::Create(GlobalContext, "afterloop");

	Builder->CreateBr(LoopBB);
	Builder->SetInsertPoint(LoopBB);

	LoopBB = Builder->GetInsertBlock();
	for (int i = 0;;i++)
		if (!while_expr->getLoop(i)) break;
		else generateStatement(while_expr->getLoop(i));

	Builder->CreateCondBr(CondV, LoopBB, AfterBB);

	function->getBasicBlockList().push_back(AfterBB);
	Builder->SetInsertPoint(AfterBB);

	return CondV;
}


/**
  * 二項演算生成メソッド
  * @param  BinaryExprAST
  * @return 生成したValueのポインタ
  */
Value *CodeGen::generateBinaryExpression(BinaryExprAST *bin_expr){
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);
	BaseAST *lhs = bin_expr->getLHS();
	BaseAST *rhs = bin_expr->getRHS();

	Value *lhs_v;
	Value *rhs_v;

	//assignment
	if(bin_expr->getOp() == "="){
		//lhs is variable
		VariableAST *lhs_var = dyn_cast<VariableAST>(lhs);
		lhs_v = CurFunc->getValueSymbolTable()->lookup(lhs_var->getName());
	}else{
		lhs_v = generateExpression(lhs);
	}
	rhs_v = generateExpression(rhs/*, lhs_v->getType()*/);
	if (llvmDebbug) fprintf(stderr, "%d: lhs and rhs exist\n", __LINE__);

	if(bin_expr->getOp() == "="){
		return Builder->CreateStore(rhs_v, lhs_v);
	}else if(bin_expr->getOp() == "+"){
		return Builder->CreateAdd(lhs_v, rhs_v, "add_tmp");
	}else if(bin_expr->getOp() == "-"){
		return Builder->CreateSub(lhs_v, rhs_v, "sub_tmp");
	}else if(bin_expr->getOp() == "*"){
		return Builder->CreateMul(lhs_v, rhs_v, "mul_tmp");
	}else if(bin_expr->getOp() == "/"){
		return Builder->CreateSDiv(lhs_v, rhs_v, "div_tmp");
	}
	return NULL;
}


/**
  * 関数呼び出し(Call命令)生成メソッド
  * @param CallExprAST
  * @return 生成したValueのポインタ　
  */
Value *CodeGen::generateCallExpression(CallExprAST *call_expr){
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);
	std::vector<Value*> arg_vec;
	BaseAST *arg;
	Value *arg_v;
	for(int i=0; ; i++){
		if(!(arg=call_expr->getArgs(i)))
			break;

		//isBinaryExpr
		if(isa<BinaryExprAST>(arg)){
			BinaryExprAST *bin_expr = dyn_cast<BinaryExprAST>(arg);

			//二項演算命令を生成
			arg_v = generateBinaryExpression(dyn_cast<BinaryExprAST>(arg));

			//代入の時はLoad命令を追加
			if(bin_expr->getOp() == "="){
				VariableAST *var = dyn_cast<VariableAST>(bin_expr->getLHS());
				arg_v = Builder->CreateLoad(CurFunc->getValueSymbolTable()->lookup(var->getName()), "arg_val");
			}
		}else{
			arg_v = generateExpression(arg);
		}
		arg_vec.push_back(arg_v);
	}
	return Builder->CreateCall(Mod->getFunction(call_expr->getCallee()), arg_vec, "call_tmp");
}


/**
  * return命令生成メソッド
  * @param  JumpStmtAST
  * @return 生成したValueのポインタ
  */
Value *CodeGen::generateJumpStatement(JumpStmtAST *jump_stmt){
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);
	BaseAST *expr = jump_stmt->getExpr();
	Value *ret_v;
	ret_v = generateExpression(expr);
	return Builder->CreateRet(ret_v);
}


/**
  * ステートメント生成メソッド
  * @param  BaseAST
  * @return 生成したValueのポインタ
  */
Value *CodeGen::generateStatement(BaseAST *stmt){
	if(isa<VariableDeclAST>(stmt))
		return generateVariableDeclaration(dyn_cast<VariableDeclAST>(stmt));
	else if(isa<BinaryExprAST>(stmt))
		return generateBinaryExpression(dyn_cast<BinaryExprAST>(stmt));
	else if(isa<CallExprAST>(stmt))
		return generateCallExpression(dyn_cast<CallExprAST>(stmt));
	else if(isa<JumpStmtAST>(stmt))
		return generateJumpStatement(dyn_cast<JumpStmtAST>(stmt));
	else if(isa<WhileExprAST>(stmt))
		return generateWhileExpr(dyn_cast<WhileExprAST>(stmt));
	else if(isa<IfExprAST>(stmt))
		return generateIfExpr(dyn_cast<IfExprAST>(stmt));
	else
		return NULL;
}


/**
  * 式生成メソッド
  * @param  BaseAST
  * @return 生成したValueのポインタ
  */
Value *CodeGen::generateExpression(BaseAST *expr/*, Type *type = Type::getInt32Ty(GlobalContext)*/) {
	if(isa<BinaryExprAST>(expr))
		return generateBinaryExpression(dyn_cast<BinaryExprAST>(expr));
	else if(isa<CallExprAST>(expr))
		return generateCallExpression(dyn_cast<CallExprAST>(expr));
	else if(isa<CastExprAST>(expr))
		return generateCastExpression(dyn_cast<CastExprAST>(expr));
	else if(isa<VariableAST>(expr))
		return generateVariable(dyn_cast<VariableAST>(expr));
	else if(isa<NumberAST>(expr))
		return generateNumber(dyn_cast<NumberAST>(expr)->getValue());
	else if(isa<BooleanAST>(expr))
		return generateBoolean(dyn_cast<BooleanAST>(expr)->getValue());
	else if(isa<NoneAST>(expr))
		return generateNone(dyn_cast<NoneAST>(expr));
	return NULL;
}


Value *CodeGen::generateCondition(BaseAST* Cond) {
	if (isa<BooleanAST>(Cond)) {
		return generateBoolean(dyn_cast<BooleanAST>(Cond)->getValue());

	}else if (isa<BinaryExprAST>(Cond)) {
		auto CondBinary = dyn_cast<BinaryExprAST>(Cond);
		Value *lhs_v = generateExpression(CondBinary->getLHS());
		Value *rhs_v = generateExpression(CondBinary->getRHS());

		if (CondBinary->getOp() == "==")
			return Builder->CreateICmpEQ(lhs_v, rhs_v, "ifcond");
		else if (CondBinary->getOp() == "!=")
			return Builder->CreateICmpNE(lhs_v, rhs_v, "ifcond");
		else if (CondBinary->getOp() == ">")
			return Builder->CreateICmpSGT(lhs_v, rhs_v, "ifcond");
		else if (CondBinary->getOp() == "<")
			return Builder->CreateICmpSLT(lhs_v, rhs_v, "ifcond");
		else if (CondBinary->getOp() == ">=")
			return Builder->CreateICmpSGE(lhs_v, rhs_v, "ifcond");
		else if (CondBinary->getOp() == "<=")
			return Builder->CreateICmpSLE(lhs_v, rhs_v, "ifcond");
		else
			return NULL;
	}else{
		return NULL;
	}
}


Value *CodeGen::generateCastExpression(CastExprAST *cast) {
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);

	Value *expr = generateExpression(cast->getSource());
	Type *DestTy = generateType(cast->getDestType());
	if (cast->getSource()->getType().getPrimType() == Type_int || cast->getSource()->getType().getPrimType() == Type_uint) {
		if (cast->getDestType().getPrimType() == Type_int || cast->getDestType().getPrimType() == Type_uint) {
			if (cast->getSource()->getType().getBits() >= cast->getDestType().getBits()) {
				return Builder->CreateTrunc(expr, DestTy);
			}else{
				return Builder->CreateZExt(expr, DestTy);
			}
		}else if (cast->getDestType().getPrimType() == Type_float) {
			return Builder->CreateSIToFP(expr, DestTy);
		}
	}else if (cast->getSource()->getType().getPrimType() == Type_float) {
		if (cast->getDestType().getPrimType() == Type_int) {
			return Builder->CreateFPToSI(expr, DestTy);
		}else if (cast->getDestType().getPrimType() == Type_uint) {
			return Builder->CreateFPToUI(expr, DestTy);
		}else if (cast->getDestType().getPrimType() == Type_float) {
			if (cast->getSource()->getType().getBits() >= cast->getDestType().getBits()) {
				return Builder->CreateFPTrunc(expr, DestTy);
			}else{
				return Builder->CreateFPExt(expr, DestTy);
			}
		}
	}
	return expr;
}
/*
Value * 	CreateTrunc (Value *V, Type *DestTy, const Twine &Name="")
ビットを小さくする
Value * 	CreateZExt (Value *V, Type *DestTy, const Twine &Name="")
ビットを大きくする（普通に）
Value * 	CreateSExt (Value *V, Type *DestTy, const Twine &Name="")
ビットを大きくする（よくわからんけど、signにするみたい）
Value * 	CreateZExtOrTrunc (Value *V, Type *DestTy, const Twine &Name="")
Value * 	CreateSExtOrTrunc (Value *V, Type *DestTy, const Twine &Name="")
どっちも出来る！
Value * 	CreateFPToUI (Value *V, Type *DestTy, const Twine &Name="")
Value * 	CreateFPToSI (Value *V, Type *DestTy, const Twine &Name="")
Value * 	CreateUIToFP (Value *V, Type *DestTy, const Twine &Name="")
Value * 	CreateSIToFP (Value *V, Type *DestTy, const Twine &Name="")
Value * 	CreateFPTrunc (Value *V, Type *DestTy, const Twine &Name="")
floatのtrunc
Value * 	CreateFPExt (Value *V, Type *DestTy, const Twine &Name="")
floatの大きくする
*/


/**
  * 変数参照(load命令)生成メソッド
  * @param VariableAST
  * @return  生成したValueのポインタ
  */
Value *CodeGen::generateVariable(VariableAST *var){
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);
	return Builder->CreateLoad(
			CurFunc->getValueSymbolTable()->lookup(var->getName()),
			"var_tmp");
}


// Number
Value *CodeGen::generateNumber(int value){
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);
	return ConstantInt::get(
			Type::getInt32Ty(GlobalContext),
			value);
}


// Boolean
Value *CodeGen::generateBoolean(bool value) {
	if (llvmDebbug) fprintf(stderr, "%d: %s %d\n", __LINE__, __func__, value);
	return Builder->getInt1(value);
}


// Null
Value *CodeGen::generateNone(NoneAST *expr) {
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);
	Type *nulltype = generateType(expr->getType());
	return ConstantInt::get(
			nulltype,
			0);
}


bool CodeGen::linkModule(Module *dest, std::string file_name){
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);
	SMDiagnostic err;
	std::unique_ptr<Module> link_mod = llvm::parseIRFile(file_name, err, GlobalContext);
	if(!link_mod)
		return false;

	if(Linker::linkModules(*dest, std::move(link_mod), 0))
		return false;

	return true;
}