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
		ExecutionEngine *EE = EngineBuilder(/*Mod*/).create();
		EngineBuilder(/*Mod*/).create();
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


/**
  * Module生成メソッド
  * @param  TranslationUnitAST Module名(入力ファイル名)
  * @return 成功時：true　失敗時：false　
  */
bool CodeGen::generateTranslationUnit(TranslationUnitAST &tunit, std::string name){
	Mod = new Module(name, GlobalContext);
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
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);
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
	Value *v=NULL;

	//insert expr statement
	BaseAST *stmt;
	for(int i=0; ; i++){
		if (llvmDebbug) fprintf(stderr, "%d\n", i);
		stmt=func_stmt->getStatement(i);
		if(!stmt)
			break;
		else if(!isa<NullExprAST>(stmt))
			v = generateStatement(stmt);
	}
	if (llvmDebbug) fprintf(stderr, "finish\n");

	return v;
}


/**
  * ステートメント生成メソッド
  * 実際にはASTの種類を確認して各種生成メソッドを呼び出し
  * @param  JumpStmtAST
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
  * 変数宣言(alloca命令)生成メソッド
  * @param VariableDeclAST
  * @return 生成したValueのポインタ
  */
Value *CodeGen::generateVariableDeclaration(VariableDeclAST *vdecl){
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);
	//create alloca
	AllocaInst *alloca;
	Type* type;
	if (vdecl->getType()[0] == Types::i1 || vdecl->getType()[0] == Types::$i1) {
		type = Type::getInt1Ty(GlobalContext);
	}else if (vdecl->getType()[0] == Types::i2) {
		//type = Type::getInt2Ty(GlobalContext);
	}else if (vdecl->getType()[0] == Types::i4) {
		//type = Type::getInt4Ty(GlobalContext);
	}else if (vdecl->getType()[0] == Types::i8) {
		type = Type::getInt8Ty(GlobalContext);
	}else if (vdecl->getType()[0] == Types::i16) {
		type = Type::getInt16Ty(GlobalContext);
	}else if (vdecl->getType()[0] == Types::i32 || vdecl->getType()[0] == Types::$i32) {
		type = Type::getInt32Ty(GlobalContext);
	}

	if (vdecl->getSize()) {
		fprintf(stderr, "%d: array\n", __LINE__);
		alloca = new AllocaInst(
				ArrayType::get(type, vdecl->getSize()),
				1000,
				vdecl->getName(),
				CurBB);
		//auto zero = ConstantInt::get(GlobalContext, APInt(64, 0, true));
		//auto index = ConstantInt::get(GlobalContext, APInt(32, vdecl->getSize(), true));
		//auto ptr = GetElementPtrInst::Create(alloca, {zero, index}, "", bblock);
		//auto store = new StoreInst(index, ptr, false, bblock);
	}else{
		alloca = Builder->CreateAlloca(type, 0, vdecl->getName());
	}

	//if args alloca
	if(vdecl->getDeclType() == VariableDeclAST::param){
		//store args
		Builder->CreateStore(CurFunc->getValueSymbolTable()->lookup(vdecl->getName().append("_arg")), alloca);
	}
	//ValueMap[vdecl->getName()]=alloca;
	return alloca;
}



/**
  * If文生成メソッド
  * @param IfExprAST
  * @return 生成したValueのポインタ
  */
Value *CodeGen::generateIfExpr(IfExprAST *if_expr) {
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);
	Value *CondV;

	if (isa<BooleanAST>(if_expr->getCond())) {
		if (dyn_cast<BooleanAST>(if_expr->getCond())->getValue())
			CondV = generateBoolean(true);
		else
			CondV = generateBoolean(false);

	}else if (isa<BinaryExprAST>(if_expr->getCond())) {
		auto CondBinary = dyn_cast<BinaryExprAST>(if_expr->getCond());
		BaseAST *lhs = CondBinary->getLHS();
		BaseAST *rhs = CondBinary->getRHS();
		Value *lhs_v, *rhs_v;

		lhs_v = generateExpression(lhs);
		rhs_v = generateExpression(rhs);

		if (CondBinary->getOp() == "==")
			CondV = Builder->CreateICmpEQ(lhs_v, rhs_v, "ifcond");
		else if (CondBinary->getOp() == "!=")
			CondV = Builder->CreateICmpNE(lhs_v, rhs_v, "ifcond");
		else if (CondBinary->getOp() == ">")
			CondV = Builder->CreateICmpSGT(lhs_v, rhs_v, "ifcond");
		else if (CondBinary->getOp() == "<")
			CondV = Builder->CreateICmpSLT(lhs_v, rhs_v, "ifcond");
		else if (CondBinary->getOp() == ">=")
			CondV = Builder->CreateICmpSGE(lhs_v, rhs_v, "ifcond");
		else if (CondBinary->getOp() == "<=")
			CondV = Builder->CreateICmpSLE(lhs_v, rhs_v, "ifcond");
	}else{
		return NULL;
	}

	Function *function = Builder->GetInsertBlock()->getParent();

	BasicBlock *ThenBB = BasicBlock::Create(GlobalContext, "then", function);
	BasicBlock *ElseBB = BasicBlock::Create(GlobalContext, "else");
	BasicBlock *MergeBB = BasicBlock::Create(GlobalContext, "ifcont");

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
	return CondV;
}



/**
  * While文生成メソッド
  * @param WhileExprAST
  * @return 生成したValueのポインタ
  */
Value *CodeGen::generateWhileExpr(WhileExprAST *while_expr) {
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);
	Value *CondV;

	if (isa<BooleanAST>(while_expr->getCond())) {
		CondV = generateBoolean(dyn_cast<BooleanAST>(while_expr->getCond())->getValue());

	}else if (isa<BinaryExprAST>(while_expr->getCond())) {
		auto CondBinary = dyn_cast<BinaryExprAST>(while_expr->getCond());
		BaseAST *lhs = CondBinary->getLHS();
		BaseAST *rhs = CondBinary->getRHS();
		Value *lhs_v, *rhs_v;

		lhs_v = generateExpression(lhs);
		rhs_v = generateExpression(rhs);

		if (CondBinary->getOp() == "==")
			CondV = Builder->CreateICmpEQ(lhs_v, rhs_v, "ifcond");
		else if (CondBinary->getOp() == "!=")
			CondV = Builder->CreateICmpNE(lhs_v, rhs_v, "ifcond");
		else if (CondBinary->getOp() == ">")
			CondV = Builder->CreateICmpSGT(lhs_v, rhs_v, "ifcond");
		else if (CondBinary->getOp() == "<")
			CondV = Builder->CreateICmpSLT(lhs_v, rhs_v, "ifcond");
		else if (CondBinary->getOp() == ">=")
			CondV = Builder->CreateICmpSGE(lhs_v, rhs_v, "ifcond");
		else if (CondBinary->getOp() == "<=")
			CondV = Builder->CreateICmpSLE(lhs_v, rhs_v, "ifcond");
	}

	Function *function = Builder->GetInsertBlock()->getParent();

	BasicBlock *PreheaderBB = Builder->GetInsertBlock();
	BasicBlock *LoopBB = BasicBlock::Create(GlobalContext, "loop", function);
	BasicBlock *AfterBB = BasicBlock::Create(GlobalContext, "afterloop");

	Builder->CreateBr(LoopBB);
	Builder->SetInsertPoint(LoopBB);
	//PHINode *Variable = Builder->CreatePHI(Type::getDoubleTy(GlobalContext), 2, "safd");
	//Variable->addIncoming(StartVal, PreheaderBB);

	LoopBB = Builder->GetInsertBlock();
	for (int i = 0;i < while_expr->getLoop().size();i++)
		generateStatement(while_expr->getLoop()[i]);

	if (isa<BooleanAST>(while_expr->getCond())) {
		CondV = generateBoolean(dyn_cast<BooleanAST>(while_expr->getCond())->getValue());

	}else if (isa<BinaryExprAST>(while_expr->getCond())) {
		auto CondBinary = dyn_cast<BinaryExprAST>(while_expr->getCond());
		BaseAST *lhs = CondBinary->getLHS();
		BaseAST *rhs = CondBinary->getRHS();
		Value *lhs_v, *rhs_v;

		lhs_v = generateExpression(lhs);
		rhs_v = generateExpression(rhs);

		if (CondBinary->getOp() == "==")
			CondV = Builder->CreateICmpEQ(lhs_v, rhs_v, "ifcond");
		else if (CondBinary->getOp() == "!=")
			CondV = Builder->CreateICmpNE(lhs_v, rhs_v, "ifcond");
		else if (CondBinary->getOp() == ">")
			CondV = Builder->CreateICmpSGT(lhs_v, rhs_v, "ifcond");
		else if (CondBinary->getOp() == "<")
			CondV = Builder->CreateICmpSLT(lhs_v, rhs_v, "ifcond");
		else if (CondBinary->getOp() == ">=")
			CondV = Builder->CreateICmpSGE(lhs_v, rhs_v, "ifcond");
		else if (CondBinary->getOp() == "<=")
			CondV = Builder->CreateICmpSLE(lhs_v, rhs_v, "ifcond");
	} 

	Builder->CreateCondBr(CondV, LoopBB, AfterBB);

	function->getBasicBlockList().push_back(AfterBB);
	Builder->SetInsertPoint(AfterBB);

	return CondV;
}
/*Value *ForExprAST::codegen() {
  // Emit the start code first, without 'variable' in scope.
  Value *StartVal = Start->codegen();
  if (!StartVal)
    return nullptr;

  // Make the new basic block for the loop header, inserting after current
  // block.
  Function *TheFunction = Builder.GetInsertBlock()->getParent();
  BasicBlock *PreheaderBB = Builder.GetInsertBlock();
  BasicBlock *LoopBB = BasicBlock::Create(TheContext, "loop", TheFunction);

  // Insert an explicit fall through from the current block to the LoopBB.
  Builder.CreateBr(LoopBB);

  // Start insertion in LoopBB.
  Builder.SetInsertPoint(LoopBB);

  // Start the PHI node with an entry for Start.
  PHINode *Variable =
      Builder.CreatePHI(Type::getDoubleTy(TheContext), 2, VarName);
  Variable->addIncoming(StartVal, PreheaderBB);

  // Within the loop, the variable is defined equal to the PHI node.  If it
  // shadows an existing variable, we have to restore it, so save it now.
  Value *OldVal = NamedValues[VarName];
  NamedValues[VarName] = Variable;

  // Emit the body of the loop.  This, like any other expr, can change the
  // current BB.  Note that we ignore the value computed by the body, but don't
  // allow an error.
  if (!Body->codegen())
    return nullptr;

  // Emit the step value.
  Value *StepVal = nullptr;
  if (Step) {
    StepVal = Step->codegen();
    if (!StepVal)
      return nullptr;
  } else {
    // If not specified, use 1.0.
    StepVal = ConstantFP::get(TheContext, APFloat(1.0));
  }

  Value *NextVar = Builder.CreateFAdd(Variable, StepVal, "nextvar");

  // Compute the end condition.
  Value *EndCond = End->codegen();
  if (!EndCond)
    return nullptr;

  // Convert condition to a bool by comparing non-equal to 0.0.
  EndCond = Builder.CreateFCmpONE(
      EndCond, ConstantFP::get(TheContext, APFloat(0.0)), "loopcond");

  // Create the "after loop" block and insert it.
  BasicBlock *LoopEndBB = Builder.GetInsertBlock();
  BasicBlock *AfterBB =
      BasicBlock::Create(TheContext, "afterloop", TheFunction);

  // Insert the conditional branch into the end of LoopEndBB.
  Builder.CreateCondBr(EndCond, LoopBB, AfterBB);

  // Any new code will be inserted in AfterBB.
  Builder.SetInsertPoint(AfterBB);

  // Add a new entry to the PHI node for the backedge.
  Variable->addIncoming(NextVar, LoopEndBB);

  // Restore the unshadowed variable.
  if (OldVal)
    NamedValues[VarName] = OldVal;
  else
    NamedValues.erase(VarName);

  // for expr always returns 0.0.
  return Constant::getNullValue(Type::getDoubleTy(TheContext));
}

Function *PrototypeAST::codegen() {
  // Make the function type:  double(double,double) etc.
  std::vector<Type *> Doubles(Args.size(), Type::getDoubleTy(TheContext));
  FunctionType *FT =
      FunctionType::get(Type::getDoubleTy(TheContext), Doubles, false);

  Function *F =
      Function::Create(FT, Function::ExternalLinkage, Name, TheModule.get());

  // Set names for all arguments.
  unsigned Idx = 0;
  for (auto &Arg : F->args())
    Arg.setName(Args[Idx++]);

  return F;
}*/


/*
 *
 *
 */
Value *CodeGen::generateExpression(BaseAST *expr) {
	if(isa<BinaryExprAST>(expr)){
		return generateBinaryExpression(dyn_cast<BinaryExprAST>(expr));

	}else if(isa<CallExprAST>(expr)){
		return generateCallExpression(dyn_cast<CallExprAST>(expr));

	}else if(isa<VariableAST>(expr)){
		return generateVariable(dyn_cast<VariableAST>(expr));

	}else if(isa<NumberAST>(expr)){
		NumberAST *num = dyn_cast<NumberAST>(expr);
		return generateNumber(num->getValue());

	}else if(isa<BooleanAST>(expr)){
		BooleanAST *boolean = dyn_cast<BooleanAST>(expr);
		return generateBoolean(boolean->getValue());

	}else if(isa<NoneAST>(expr)){
		return generateNone(dyn_cast<NoneAST>(expr));

	}

	//}else if(isa<ArrayAST>(rhs)){
	//	ArrayAST *array = dyn_cast<ArrayAST>(rhs);
	//	rhs_v = generateArray(array);

	return NULL;
}



/**
  * 二項演算生成メソッド
  * @param  JumpStmtAST
  * @return 生成したValueのポインタ
  */
Value *CodeGen::generateBinaryExpression(BinaryExprAST *bin_expr){
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);
	BaseAST *lhs=bin_expr->getLHS();
	BaseAST *rhs=bin_expr->getRHS();

	Value *lhs_v;
	Value *rhs_v;

	//assignment
	if(bin_expr->getOp()=="="){
		//lhs is variable
		VariableAST *lhs_var = dyn_cast<VariableAST>(lhs);
		lhs_v = CurFunc->getValueSymbolTable()->lookup(lhs_var->getName());
		fprintf(stderr, "vs_table \n");
		if (!lhs_v) fprintf(stderr, "null\n");
	}else{
		lhs_v = generateExpression(lhs);
	}

	rhs_v = generateExpression(rhs);
	if (llvmDebbug)fprintf(stderr, "%d: lhs and rhs exist\n", __LINE__);

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
  * ジャンプ(今回はreturn命令のみ)生成メソッド
  * @param  JumpStmtAST
  * @return 生成したValueのポインタ
  */
Value *CodeGen::generateJumpStatement(JumpStmtAST *jump_stmt){
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);
	BaseAST *expr = jump_stmt->getExpr();
	Value *ret_v;
	ret_v = generateExpression(expr);
	Builder->CreateRet(ret_v);
}



/**
  * 変数参照(load命令)生成メソッド
  * @param VariableAST
  * @return  生成したValueのポインタ
  */
Value *CodeGen::generateVariable(VariableAST *var){
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);
	if (var->getSize()){
		var->getIndex();
	}else{
		return Builder->CreateLoad(CurFunc->getValueSymbolTable()->lookup(var->getName()), "var_tmp");
	}
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
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);
	return ConstantInt::get(
			Type::getInt1Ty(GlobalContext),
			value);
}



// Null
Value *CodeGen::generateNone(NoneAST *expr) {
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);
	if (expr->getType() == Types::i32) {
		return ConstantInt::get(
				Type::getInt32Ty(GlobalContext),
				0);
	}else if (expr->getType() == Types::i1) {
		return ConstantInt::get(
				Type::getInt1Ty(GlobalContext),
				0);
	}
	fprintf(stderr, "asdf\n");
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

