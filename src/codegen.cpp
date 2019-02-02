#include "codegen.hpp"

bool llvmDebbug = false;

CodeGen::CodeGen() {
	Builder = new IRBuilder<>(GlobalContext);
	Mod = NULL;
}

CodeGen::~CodeGen() {
	SAFE_DELETE(Builder);
	SAFE_DELETE(Mod);
}

/**
  * コード生成実行
  * @param  TranslationUnitAST　Module名(入力ファイル名)
  * @return 成功時：true　失敗時:false
  */
bool CodeGen::doCodeGen(TranslationUnitAST &tunit, std::string name, std::string link_file, bool with_jit = false) {
	if (!generateTranslationUnit(tunit, name)) return false;

	//LinkFileの指定があったらModuleをリンク
	if (!link_file.empty() && !linkModule(Mod, link_file) )
		return false;

	//JITのフラグが立っていたらJIT
	if (with_jit) {
		ExecutionEngine *EE = EngineBuilder().create();
		EngineBuilder().create();
			Function *F;
		if (!(F = Mod->getFunction("main")))
			return false;

		int (*fp)() = (int (*)())EE->getPointerToFunction(F);
		fprintf(stderr, "%d\n", fp());
	}

	return true;
}


Module &CodeGen::getModule() {
	if (Mod)
		return *Mod;
	else
		return *(new Module("null", GlobalContext));
}


/**
  * Module生成メソッド
  * @param  TranslationUnitAST Module名(入力ファイル名)
  * @return 成功時：true　失敗時：false　
  */
bool CodeGen::generateTranslationUnit(TranslationUnitAST &tunit, std::string name) {
	Mod = new Module(name, GlobalContext);
	//import
	for (int i=0; ; i++) {
		ImportAST *import = tunit.getImport(i);
		if (!import)
			break;
		std::string link_file = import->getFileName();
		if (!link_file.empty() && !linkModule(Mod, link_file))
			return false;
	}

	//funtion declaration
	for (int i=0; ; i++) {
		PrototypeAST *proto = tunit.getPrototype(i);
		if (!proto)
			break;
		else if (!generatePrototype(proto, Mod)) {
			SAFE_DELETE(Mod);
			return false;
		}
	}

	//function definition
	for (int i=0; ; i++) {
		FunctionAST *func = tunit.getFunction(i);
		if (!func)
			break;
		else if (!(generateFunctionDefinition(func, Mod))) {
			SAFE_DELETE(Mod);
			return false;
		}
	}
	return true;
}


/**
  * LLVM IRとリンク
  */
bool CodeGen::linkModule(Module *dest, std::string file_name) {
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);
	SMDiagnostic err;
	std::unique_ptr<Module> link_mod = llvm::parseIRFile(file_name, err, GlobalContext);
	if (!link_mod)
		return false;

	if (Linker::linkModules(*dest, std::move(link_mod), 0))
		return false;

	return true;
}


/**
  * 関数宣言生成メソッド
  * @param  PrototypeAST, Module
  * @return 生成したFunctionのポインタ
  */
Function *CodeGen::generatePrototype(PrototypeAST *proto, Module *mod) {
	//already declared?
	Function *func = mod->getFunction(proto->getName());
	if (func) {
		if (func->arg_size() == proto->getParamSize()/* && func->empty()*/) {
			return func;
		}else{
			fprintf(stderr, "error: function %s is redefined\n", proto->getName().c_str());
			return NULL;
		}
	}

	//create arg_types
	std::vector<Type*> arg_types;
	for (int i = 0; ;i++)
		if (proto->getParamType(i) != Types(Type_null)) arg_types.push_back(generateType(proto->getParamType(i)));
		else break;

	//create func type
	FunctionType *func_type = FunctionType::get(generateType(proto->getType()), arg_types, false);
	//create function
	func = Function::Create(func_type, Function::ExternalLinkage, proto->getName(), mod);

	//set names
	Function::arg_iterator arg_iter = func->arg_begin();
	for (size_t i = 0; i < proto->getParamSize(); i++) {
		arg_iter->setName(proto->getParamName(i).append("_arg"));
		++arg_iter;
	}
	return func;
}


/**
  * 関数定義生成メソッド
  * @param  FunctionAST Module
  * @return 生成したFunctionのポインタ
  */
Function *CodeGen::generateFunctionDefinition(FunctionAST *func_ast, Module *mod) {
	Function *func = generatePrototype(func_ast->getPrototype(), mod);
	if (!func)
		return NULL;
	CurFunc = func;
	CurBB = BasicBlock::Create(GlobalContext, "entry", func);
	Builder->SetInsertPoint(CurBB);
	generateFunctionStatement(func_ast->getBody());

	return func;
}


/**
  * 型生成メソッド
  * @param  Types
  * @return 生成したTypeのポインタ
  */
Type *CodeGen::generateType(Types type) {
	Type *dest_type;
	if (type.getPrimType() == Type_void) {
		dest_type = Type::getVoidTy(GlobalContext);
		
	}else if (type.getPrimType() == Type_int) {
		dest_type = Type::getIntNTy(GlobalContext, type.getBits());
	
	}else if (type.getPrimType() == Type_uint) {
		dest_type = Type::getIntNTy(GlobalContext, type.getBits() + 1);

	}else if (type.getPrimType() == Type_float) {
		if (type.getBits() == 16) {
			dest_type = Type::getHalfTy(GlobalContext);
		}else if (type.getBits() == 32) {
			dest_type = Type::getFloatTy(GlobalContext);
		}else if (type.getBits() == 64) {
			dest_type = Type::getDoubleTy(GlobalContext);
		}else if (type.getBits() == 128) {
			dest_type = Type::getFP128Ty(GlobalContext);
		}else if (type.getBits() == 80) {
			dest_type = Type::getX86_FP80Ty(GlobalContext);
		}else{
			fprintf(stderr, "Type is not found '%s'\n", type.printType().c_str());
			dest_type = Type::getFloatTy(GlobalContext);
		}

	}else if (type.getPrimType() == Type_bool) {
		dest_type = Type::getInt1Ty(GlobalContext);

	}else{
		fprintf(stderr, "Type is not found '%s'\n", type.printType().c_str());
		dest_type = Type::getInt32Ty(GlobalContext);
	}

	if (type.getArraySize() != 0) {
		dest_type = PointerType::getUnqual(ArrayType::get(dest_type, type.getArraySize()));
	}
	return dest_type;
}


/**
  * 関数生成メソッド
  * 変数宣言、ステートメントの順に生成　
  * @param  FunctionStmtAST
  * @return 最後に生成したValueのポインタ
  */
Value *CodeGen::generateFunctionStatement(FunctionStmtAST *func_stmt) {
	Value *v = NULL;

	//insert expr statement
	BaseAST *stmt;
	// generateArray();
	for (int i=0; ; i++) {
		if (llvmDebbug) fprintf(stderr, "%d\n", i+1);
		stmt = func_stmt->getStatement(i);
		if (!stmt)
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
Value *CodeGen::generateVariableDeclaration(VariableDeclAST *vdecl) {
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);
	//create alloca
	Type* type = generateType(vdecl->getType());
	Value *alloca;
	if (vdecl->getType().getArraySize() != 0) {
		// alloca = Builder->CreateAlloca(type, Mod->getDataLayout().getAllocaAddrSpace(), vdecl->getName());
	}else{
		alloca = Builder->CreateAlloca(type, nullptr, vdecl->getName());
	}
	VariableTable.insert(std::make_pair(vdecl->getName(), alloca));
	generateArray();

	//if args alloca
	if (vdecl->getDeclType() == VariableDeclAST::param) {
		//store args
		Builder->CreateStore(CurFunc->getValueSymbolTable()->lookup(vdecl->getName().append("_arg")), alloca);
	}
	return alloca;
}


/**
  * test Array生成メソッド
  * @param VariableDeclAST
  * @return 生成したValueのポインタ
  */
Value *CodeGen::generateArray() {
	// auto arrayType = llvm::ArrayType::get(llvm::IntegerType::get(GlobalContext, 32), size);
	// auto arrayPtr = new llvm::AllocaInst(arrayType, "", Builder->GetInsertBlock());
	// auto zero = llvm::ConstantInt::get(GlobalContext, llvm::APInt(64, 0, true));
	// auto index = llvm::ConstantInt::get(GlobalContext, llvm::APInt(32, 1, true));
	// auto ptr = llvm::GetElementPtrInst::Create(arrayPtr, { zero, index }, "", Builder->GetInsertBlock());
	// auto store = new llvm::StoreInst(index, ptr, false, Builder->GetInsertBlock());
	// ptr = llvm::GetElementPtrInst::Create(arrayPtr, { zero, index }, "", Builder->GetInsertBlock());
	// return Builder->CreateLoad(ptr);
	// Builder->CreateInsertValue(array, one, {0});
	// return Builder->CreateExtractValue(Builder->CreateLoad(array), {0});
}


/**
  * If文生成メソッド
  * @param IfStmtAST
  * @return 生成したValueのポインタ
  */
Value *CodeGen::generateIfStmt(IfStmtAST *if_expr) {
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);

	Value *CondV = generateCondition(if_expr->getCond());

	Function *function = Builder->GetInsertBlock()->getParent();
	BasicBlock *ThenBB = BasicBlock::Create(GlobalContext, "then", function);
	BasicBlock *ElseBB = BasicBlock::Create(GlobalContext, "else");
	BasicBlock *MergeBB = BasicBlock::Create(GlobalContext, "ifcont");

	Builder->CreateCondBr(CondV, ThenBB, ElseBB);

	Builder->SetInsertPoint(ThenBB);
	// if文がネストしたとき
	auto lastThenBB = Builder->GetInsertBlock();
	// thenBBs.push_back(BB_Value(lastThenBB, thenValue));

	ThenBB = Builder->GetInsertBlock();
	for (int i = 0;;i++)
		if (!if_expr->getThen(i)) break;
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
  * @param WhileStmtAST
  * @return 生成したValueのポインタ
  */
Value *CodeGen::generateWhileStmt(WhileStmtAST *while_expr) {
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);

	Function *function = Builder->GetInsertBlock()->getParent();

	BasicBlock *PreheaderBB = Builder->GetInsertBlock();
	BasicBlock *LoopBB = BasicBlock::Create(GlobalContext, "loop", function);
	BasicBlock *AfterBB = BasicBlock::Create(GlobalContext, "afterloop");

	Value *CondV = generateCondition(while_expr->getCond());
	Builder->CreateCondBr(CondV, LoopBB, AfterBB);

	Builder->SetInsertPoint(LoopBB);
	LoopBB = Builder->GetInsertBlock();
	
	for (int i = 0;;i++)
		if (!while_expr->getLoop(i)) break;
		else generateStatement(while_expr->getLoop(i));
	CondV = generateCondition(while_expr->getCond());

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
Value *CodeGen::generateBinaryExpression(BinaryExprAST *bin_expr) {
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);
	BaseAST *lhs = bin_expr->getLHS();
	BaseAST *rhs = bin_expr->getRHS();

	Value *lhs_v;
	Value *rhs_v;

	//assignment
	if (bin_expr->getOp() == "=") {
		//lhs is variable
		if (llvmDebbug) fprintf(stderr, "%d: lhs is variable\n", __LINE__);
		VariableAST *lhs_var = dyn_cast<VariableAST>(lhs);
		if (lhs_var->getIndex() != -1) {
			return Builder->CreateInsertValue(VariableTable.at(lhs_var->getName()), rhs_v, lhs_var->getIndex());
		}
		lhs_v = CurFunc->getValueSymbolTable()->lookup(lhs_var->getName());
		rhs_v = generateExpression(rhs);
	}else{
		lhs_v = generateExpression(lhs);
		rhs_v = generateExpression(rhs);
	}

	if (llvmDebbug) fprintf(stderr, "%d: lhs and rhs exist\n", __LINE__);
	Types type = bin_expr->getType();

	if (bin_expr->getOp() == "=") {
		return Builder->CreateStore(rhs_v, lhs_v);

	}else if (bin_expr->getOp() == "+") {
		if (type.getPrimType() == Type_int || type.getPrimType() == Type_uint)
			return Builder->CreateAdd(lhs_v, rhs_v, "add_tmp");
		if (type.getPrimType() == Type_float)
			return Builder->CreateFAdd(lhs_v, rhs_v, "add_tmp");

	}else if (bin_expr->getOp() == "-") {
		if (type.getPrimType() == Type_int || type.getPrimType() == Type_uint)
			return Builder->CreateSub(lhs_v, rhs_v, "sub_tmp");
		if (type.getPrimType() == Type_float)
			return Builder->CreateFSub(lhs_v, rhs_v, "sub_tmp");

	}else if (bin_expr->getOp() == "*") {
		if (type.getPrimType() == Type_int || type.getPrimType() == Type_uint)
			return Builder->CreateMul(lhs_v, rhs_v, "mul_tmp");
		if (type.getPrimType() == Type_float)
			return Builder->CreateFMul(lhs_v, rhs_v, "mul_tmp");

	}else if (bin_expr->getOp() == "/") {
		if (type.getPrimType() == Type_int)
			return Builder->CreateSDiv(lhs_v, rhs_v, "div_tmp");
		else if (type.getPrimType() == Type_uint)
			return Builder->CreateUDiv(lhs_v, rhs_v, "div_tmp");
		else if (type.getPrimType() == Type_float)
			return Builder->CreateFDiv(lhs_v, rhs_v, "div_tmp");

	}else if (bin_expr->getOp() == "%") {
		if (type.getPrimType() == Type_uint)
			return Builder->CreateURem(lhs_v, rhs_v, "rem_tmp");
		else if (type.getPrimType() == Type_int)
			return Builder->CreateSRem(lhs_v, rhs_v, "rem_tmp");
		else if (type.getPrimType() == Type_float)
			return Builder->CreateFRem(lhs_v, rhs_v, "rem_tmp");
	
	}else if (bin_expr->getOp() == "<<") {
		return Builder->CreateShl(lhs_v, rhs_v, "shl_tmp");
	
	}else if (bin_expr->getOp() == ">>") {
		return Builder->CreateAShr(lhs_v, rhs_v, "ashr_tmp");

	}else if (bin_expr->getOp() == "&") {
		return Builder->CreateAnd(lhs_v, rhs_v, "and_tmp");

	}else if (bin_expr->getOp() == "|") {
		return Builder->CreateOr(lhs_v, rhs_v, "or_tmp");

	}else if (bin_expr->getOp() == "^") {
		return Builder->CreateXor(lhs_v, rhs_v, "xor_tmp");
	}
	if (type == Types(Type_bool)) {
		if (isa<PlaceholderAST>(bin_expr->getLHS()) || isa<PlaceholderAST>(bin_expr->getRHS())) {
			return ConstantInt::get(Type::getInt1Ty(GlobalContext), true, false);
		}
		Value *lhs_v = generateExpression(bin_expr->getLHS());
		Value *rhs_v = generateExpression(bin_expr->getRHS());

		Types type = Types(Type_null);

		if (bin_expr->getLHS()->getType() == bin_expr->getRHS()->getType()) {
			type = bin_expr->getLHS()->getType();
		}

		if (bin_expr->getOp() == "==")
			if (type == Types(Type_int) || type == Types(Type_uint) || type == Types(Type_bool))
				return Builder->CreateICmpEQ(lhs_v, rhs_v, "ifcond");
			if (type == Types(Type_float))
				return Builder->CreateFCmpOEQ(lhs_v, rhs_v, "ifcond");
	
		else if (bin_expr->getOp() == "!=")
			if (type == Types(Type_int) || type == Types(Type_uint) || type == Types(Type_bool))
				return Builder->CreateICmpNE(lhs_v, rhs_v, "ifcond");
			if (type == Types(Type_float))
				return Builder->CreateFCmpONE(lhs_v, rhs_v, "ifcond");
		
		else if (bin_expr->getOp() == ">")
			if (type == Types(Type_int))
				return Builder->CreateICmpSGT(lhs_v, rhs_v, "ifcond");
			if (type == Types(Type_uint) || type == Types(Type_bool))
				return Builder->CreateICmpUGT(lhs_v, rhs_v, "ifcond");
			if (type == Types(Type_float))
				return Builder->CreateFCmpOGT(lhs_v, rhs_v, "ifcond");

		else if (bin_expr->getOp() == "<")
			if (type == Types(Type_int))
				return Builder->CreateICmpSLT(lhs_v, rhs_v, "ifcond");
			if (type == Types(Type_uint) || type == Types(Type_bool))
				return Builder->CreateICmpULT(lhs_v, rhs_v, "ifcond");
			if (type == Types(Type_float))
				return Builder->CreateFCmpOLT(lhs_v, rhs_v, "ifcond");

		else if (bin_expr->getOp() == ">=")
			if (type == Types(Type_int))
				return Builder->CreateICmpSGE(lhs_v, rhs_v, "ifcond");
			if (type == Types(Type_uint) || type == Types(Type_bool))
				return Builder->CreateICmpUGE(lhs_v, rhs_v, "ifcond");
			if (type == Types(Type_float))
				return Builder->CreateFCmpOGE(lhs_v, rhs_v, "ifcond");

		else if (bin_expr->getOp() == "<=")
			if (type == Types(Type_int))
				return Builder->CreateICmpSLE(lhs_v, rhs_v, "ifcond");
			if (type == Types(Type_uint) || type == Types(Type_bool))
				return Builder->CreateICmpULE(lhs_v, rhs_v, "ifcond");
			if (type == Types(Type_float))
				return Builder->CreateFCmpOLE(lhs_v, rhs_v, "ifcond");

		else fprintf(stderr, "not found");
	}
	
	return NULL;
}


/**
  * 演算生成メソッド
  * @param  BinaryExprAST
  * @return 生成したValueのポインタ
  */
Value *CodeGen::generateSingleExpression(SingleExprAST *sin_expr) {
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);
	BaseAST *lhs = sin_expr->getLHS();
	
	Value *lhs_v = generateExpression(lhs);

	if (sin_expr->getOp() == "!") {
		Value *sint = generateValue(new ValueAST(true, Types(Type_bool)));
		return Builder->CreateXor(sint, lhs_v, "xor_tmp");
	}else{
		return NULL;
	}
}


/**
  * 関数呼び出し(Call命令)生成メソッド
  * @param CallExprAST
  * @return 生成したValueのポインタ　
  */
Value *CodeGen::generateCallExpression(CallExprAST *call_expr) {
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);
	std::vector<Value*> arg_vec;
	BaseAST *arg;
	Value *arg_v;
	for (int i=0; ; i++) {
		arg = call_expr->getArgs(i);
		if (!arg) break;

		if (isa<BinaryExprAST>(arg)) {
			BinaryExprAST *bin_expr = dyn_cast<BinaryExprAST>(arg);
			//二項演算命令を生成
			arg_v = generateBinaryExpression(bin_expr);
			//代入の時はLoad命令を追加
			if (bin_expr->getOp() == "=") {
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
Value *CodeGen::generateJumpStatement(JumpStmtAST *jump_stmt) {
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
Value *CodeGen::generateStatement(BaseAST *stmt) {
	if (isa<VariableDeclAST>(stmt))
		return generateVariableDeclaration(dyn_cast<VariableDeclAST>(stmt));
	else if (isa<BinaryExprAST>(stmt))
		return generateBinaryExpression(dyn_cast<BinaryExprAST>(stmt));
	else if (isa<CallExprAST>(stmt))
		return generateCallExpression(dyn_cast<CallExprAST>(stmt));
	else if (isa<JumpStmtAST>(stmt))
		return generateJumpStatement(dyn_cast<JumpStmtAST>(stmt));
	else if (isa<IfStmtAST>(stmt))
		return generateIfStmt(dyn_cast<IfStmtAST>(stmt));
	else if (isa<WhileStmtAST>(stmt))
		return generateWhileStmt(dyn_cast<WhileStmtAST>(stmt));
	return NULL;
}


/**
  * 式生成メソッド
  * @param  BaseAST
  * @return 生成したValueのポインタ
  */
Value *CodeGen::generateExpression(BaseAST *expr) {
	if (isa<BinaryExprAST>(expr))
		return generateBinaryExpression(dyn_cast<BinaryExprAST>(expr));
	else if (isa<SingleExprAST>(expr))
		return generateSingleExpression(dyn_cast<SingleExprAST>(expr));
	else if (isa<CallExprAST>(expr))
		return generateCallExpression(dyn_cast<CallExprAST>(expr));
	else if (isa<CastExprAST>(expr)) {
		if (dyn_cast<CastExprAST>(expr)->getNestin()) return generateValue(new ValueAST(true, Types(Type_bool)));
		BaseAST *source = dyn_cast<CastExprAST>(expr)->getSource();
		return generateCastExpression(generateExpression(source), source->getType(), dyn_cast<CastExprAST>(expr)->getDestType());
	}else if (isa<VariableAST>(expr))
		return generateVariable(dyn_cast<VariableAST>(expr));
	else if (isa<ValueAST>(expr))
		return generateValue(dyn_cast<ValueAST>(expr));
	return NULL;
}


/**
  * 条件式生成メソッド
  * @param  BaseAST
  * @return 生成したValueのポインタ
  */
Value *CodeGen::generateCondition(BaseAST* Cond) {
	if (isa<BinaryExprAST>(Cond)) {
		auto CondBinary = dyn_cast<BinaryExprAST>(Cond);
		if (CondBinary->getOp() == "&&") {
			return Builder->CreateAnd(
				generateCondition(CondBinary->getLHS()),
				generateCondition(CondBinary->getRHS()),
				"and_cond"
				);
		}else if (CondBinary->getOp() == "||") {
			return Builder->CreateOr(
				generateCondition(CondBinary->getLHS()),
				generateCondition(CondBinary->getRHS()),
				"or_cond"
				);
		}
	}
	return generateCompare(Cond);
}


/**
  * 条件生成メソッド
  * @param  BaseAST
  * @return 生成したValueのポインタ
  */
Value *CodeGen::generateCompare(BaseAST* Cond) {
	if (isa<BinaryExprAST>(Cond)) {
		return generateBinaryExpression(dyn_cast<BinaryExprAST>(Cond));
	}else{
		return generateExpression(Cond);
	}
}


/**
  * キャスト命令生成メソッド
  * @param VariableAST
  * @return  生成したValueのポインタ
  */
Value *CodeGen::generateCastExpression(Value *src, Types SrcType, Types DestType) {
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);

	Type *DestTy = generateType(DestType);

	if (SrcType.getPrimType() == DestType.getPrimType() && SrcType.getBits() == DestType.getBits() && SrcType.getNonNull() < DestType.getNonNull()) {
		return src;
	}

	if (SrcType.getPrimType() == Type_int || SrcType.getPrimType() == Type_uint || SrcType.getPrimType() == Type_bool) {
		if (DestType.getPrimType() == Type_int || DestType.getPrimType() == Type_uint || SrcType.getPrimType() == Type_bool) {
			if (SrcType.getBits() > DestType.getBits()) {
				// bit数を小さくする
				if (llvmDebbug) fprintf(stderr, "Trunc\n");
				return Builder->CreateTrunc(src, DestTy);
			}else if (SrcType.getBits() < DestType.getBits()) {
				// bit数を大きくする（SExtもあるがこっちが普通らしい
				if (llvmDebbug) fprintf(stderr, "ZExt\n");
				return Builder->CreateZExt(src, DestTy);
			}
		}else if (DestType.getPrimType() == Type_float) {
			// Signed Int to Floating Point
			if (llvmDebbug) fprintf(stderr, "SIToFP\n");
			return Builder->CreateSIToFP(src, DestTy);
		}
	}else if (SrcType.getPrimType() == Type_float) {
		if (DestType.getPrimType() == Type_int) {
			// Floating Point to Signed Int
			if (llvmDebbug) fprintf(stderr, "FPToSI\n");
			return Builder->CreateFPToSI(src, DestTy);
		}else if (DestType.getPrimType() == Type_uint || SrcType.getPrimType() == Type_bool) {
			// Floating Point to Unsigned Int
			if (llvmDebbug) fprintf(stderr, "FPToUI\n");
			return Builder->CreateFPToUI(src, DestTy);
		}else if (DestType.getPrimType() == Type_float) {
			if (llvmDebbug) fprintf(stderr, "FP\n");
			return Builder->CreateFPCast(src, DestTy);
		}
	}
	if (llvmDebbug) fprintf(stderr, "Cast\n");
	return src;
}


/**
  * 変数参照(load命令)生成メソッド
  * @param VariableAST
  * @return  生成したValueのポインタ
  */
Value *CodeGen::generateVariable(VariableAST *var) {
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);
	if (var->getIndex() != -1) {
		return Builder->CreateExtractValue(VariableTable.at(var->getName()), {var->getIndex()});
	}
	return Builder->CreateLoad(
			CurFunc->getValueSymbolTable()->lookup(var->getName()),
			"var_tmp");
}


/**
  * 値生成メソッド
  * @param ValueAST
  * @return  生成したValueのポインタ
  */
Value *CodeGen::generateValue(ValueAST *val) {
	if (llvmDebbug) fprintf(stderr, "%d: %s\n", __LINE__, __func__);
	Type *type = generateType(val->getType());
	if (val->getType().getPrimType() == Type_bool) {
		return ConstantInt::get(type, val->getValue(), false);
	}else if (val->getType().getPrimType() == Type_int) {
		return ConstantInt::get(type, val->getValue(), true);
	}else if (val->getType().getPrimType() == Type_uint) {
		return ConstantInt::get(type, val->getValue(), false);
	}else if (val->getType().getPrimType() == Type_float) {
		return ConstantFP::get(type, val->getValue());
	}else if (val->getType().getPrimType() == Type_char) {
		return ConstantInt::get(type, val->getValue(), false);
	}else if (val->getType().getPrimType() == Type_null) {
		return Constant::getNullValue(type);
	}else if (val->getType().getPrimType() == Type_all) {
		return Constant::getNullValue(type);
	}
	return NULL;
}