#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/TargetSelect.h"
#include "time.h"
#include "lexer.hpp"
#include "AST.hpp"
#include "parser.hpp"
#include "codegen.hpp"


/**
 * オプション切り出し用クラス
 */
class OptionParser {
	private:
		std::string InputFileName;
		std::string OutputFileName;
		std::string LinkFileName;
		bool WithJit;
		int Argc;
		char **Argv;

	public:
		OptionParser(int argc, char **argv):Argc(argc), Argv(argv), WithJit(false){}
		void printHelp();
		std::string getInputFileName(){return InputFileName;} 		//入力ファイル名取得
		std::string getOutputFileName(){return OutputFileName;} 	//出力ファイル名取得
		std::string getLinkFileName(){return LinkFileName;} 	//リンク用ファイル名取得
		bool getWithJit(){return WithJit;}		//JIT実行有無
		bool parseOption();
};


/**
 * ヘルプ表示
 */
void OptionParser::printHelp(){
	fprintf(stdout, "Compiler for DummyC...\n" );
	fprintf(stdout, "試作中なのでバグがあったらご報告を\n" );
}


/**
 * オプション切り出し
 * @return 成功時：true　失敗時：false
 */
bool OptionParser::parseOption(){
	if(Argc < 2){
		fprintf(stderr,"引数が足りません\n");
		return false;
	}


	for(int i=1; i<Argc; i++){
		if(Argv[i][0]=='-' && Argv[i][1] == 'o' && Argv[i][2] == '\0'){
			//output filename
			OutputFileName.assign(Argv[++i]);
		}else if(Argv[i][0]=='-' && Argv[i][1] == 'h' && Argv[i][2] == '\0'){
			printHelp();
			return false;
		}else if(Argv[i][0]=='-' && Argv[i][1] == 'l' && Argv[i][2] == '\0'){
			LinkFileName.assign(Argv[++i]);
		}else if(Argv[i][0]=='-' && Argv[i][1] == 'j' && Argv[i][2] == 'i' && Argv[i][3] == 't' && Argv[i][4] == '\0'){
			WithJit = true;
		}else if(Argv[i][0]=='-'){
			fprintf(stderr,"%s は不明なオプションです\n", Argv[i]);
			return false;
		}else{
			InputFileName.assign(Argv[i]);
		}
	}

	//OutputFileName
	std::string ifn = InputFileName;
	int len = ifn.length();
	if (OutputFileName.empty() && (len > 2) &&
		ifn[len-3] == '.' &&
		((ifn[len-2] == 'd' && ifn[len-1] == 'c'))) {
		OutputFileName = std::string(ifn.begin(), ifn.end()-3); 
		OutputFileName += ".s";
	} else if(OutputFileName.empty()){
		OutputFileName = ifn;
		OutputFileName += ".s";
	}

	return true;
}


void printAST(std::vector<BaseAST*> stmt_list, int nest){
	for(int j=0; j < stmt_list.size(); j++) {
		BaseAST *stmt = stmt_list[j];
		for(int i=0; i < nest; i++) fprintf(stderr, "	");
		if(!stmt) {
			fprintf(stderr, "break\n");
			break;
		}
		else if(isa<VariableDeclAST>(stmt))
			fprintf(stderr, "VariableDeclaration\n");
		else if(isa<BinaryExprAST>(stmt))
			fprintf(stderr, "BinaryExpression\n");
		else if(isa<NullExprAST>(stmt))
			fprintf(stderr, "NullExpression\n");
		else if(isa<CallExprAST>(stmt))
			fprintf(stderr, "CallExpression\n");
		else if(isa<JumpStmtAST>(stmt))
			fprintf(stderr, "JumpStatement\n");
		else if(isa<VariableAST>(stmt))
			fprintf(stderr, "Variable\n");
		else if(isa<IfExprAST>(stmt)){
			fprintf(stderr, "IfExpression\n");
			std::vector<BaseAST*> arg_list;arg_list.push_back(dyn_cast<IfExprAST>(stmt)->getCond());
			printAST(arg_list, nest);
			printAST(dyn_cast<IfExprAST>(stmt)->getThen(), nest+1);
			printAST(dyn_cast<IfExprAST>(stmt)->getElse(), nest+1);
		}
		else if(isa<WhileExprAST>(stmt)) {
			fprintf(stderr, "WhileExpression\n");
			std::vector<BaseAST*> arg_list;arg_list.push_back(dyn_cast<WhileExprAST>(stmt)->getCond());
			printAST(arg_list, nest);
			printAST(dyn_cast<WhileExprAST>(stmt)->getLoop(), nest+1);
		}
		else if(isa<NumberAST>(stmt))
			fprintf(stderr, "Number\n");
		else if(isa<BooleanAST>(stmt))
			fprintf(stderr, "Boolean\n");
		else if(isa<NoneAST>(stmt))
			fprintf(stderr, "None\n");
		else
			fprintf(stderr, "unknown\n");
	}
}

/**
 * main関数
 */
int main(int argc, char **argv) {
	bool printStruct = true;
	clock_t start = clock();
	clock_t left, right = clock();
	llvm::InitializeNativeTarget();
	llvm::sys::PrintStackTraceOnErrorSignal(argv[0]);
	llvm::PrettyStackTraceProgram X(argc, argv);

	llvm::EnableDebugBuffering = true;

	OptionParser opt(argc, argv);
	if(!opt.parseOption())
		exit(1);

	//check
	if(opt.getInputFileName().length() == 0){
		fprintf(stderr,"入力ファイル名が指定されていません\n");
		exit(1);
	}
	left = clock();
	fprintf(stderr, "%.3f ms : File Read\n", (double)(left - right) * 1000 / CLOCKS_PER_SEC);
	right = left;

	//lex and parse
	Parser *parser = new Parser(opt.getInputFileName());
	if(!parser->doParse()){
		fprintf(stderr, "err at Parser or Lexer\n");
		SAFE_DELETE(parser);
		exit(1);
	}
	left = clock();
	fprintf(stderr, "%.3f ms : Lex and Parse\n", (double)(left - right) * 1000 / CLOCKS_PER_SEC);
	right = left;


	//get AST
	TranslationUnitAST &tunit = parser->getAST();
	// print AST
	if (printStruct) {
		for(int i=0; ; i++) {
			FunctionAST *func = tunit.getFunction(i);
			if(!func)
				break;
			fprintf(stderr, "%s\n", func->getName().c_str());
			FunctionStmtAST *func_stmt = func->getBody();
			printAST(func_stmt->getStatements(), 1);
		}
	}
	if(tunit.empty()){
		fprintf(stderr,"TranslationUnit is empty\n");
		SAFE_DELETE(parser);
		exit(1);
	}


	CodeGen *codegen = new CodeGen();
	if(!codegen->doCodeGen(tunit, opt.getInputFileName(), 
				opt.getLinkFileName(), opt.getWithJit())){
		fprintf(stderr, "err at codegen\n");
		SAFE_DELETE(parser);
		SAFE_DELETE(codegen);
		exit(1);
	}
	left = clock();
	fprintf(stderr, "%.3f ms : generate the LLVM IR\n", (double)(left - right) * 1000 / CLOCKS_PER_SEC);
	right = left;


	//get Module
	llvm::Module &mod=codegen->getModule();
	if(mod.empty()){
		fprintf(stderr,"Module is empty\n");
		SAFE_DELETE(parser);
		SAFE_DELETE(codegen);
		exit(1);
	}


	//llvm::PassManager<AnalysisManager<>> pm;
	llvm::legacy::PassManager pm;

	//SSA化
	pm.add(llvm::createPromoteMemoryToRegisterPass());

	//出力
	std::error_code error;
	llvm::raw_fd_ostream raw_stream(opt.getOutputFileName().c_str(), error, llvm::sys::fs::OpenFlags::F_None);
	pm.add(llvm::createPrintModulePass(raw_stream));
	pm.run(mod);
	raw_stream.close();

	//delete
	SAFE_DELETE(parser);
	SAFE_DELETE(codegen);
	left = clock();
	fprintf(stderr, "%.3f ms : write the LLVM file\n", (double)(left - right) * 1000 / CLOCKS_PER_SEC);
	fprintf(stderr, "compile has been completed! total compile time is %.3f ms\n", (double)(left - start) * 1000 / CLOCKS_PER_SEC);
	return 0;
}
