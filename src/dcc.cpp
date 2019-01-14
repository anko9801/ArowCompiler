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
		bool Lazy;
		bool WithJit;
		int Argc;
		char **Argv;

	public:
		OptionParser(int argc, char **argv) : WithJit(false), Argc(argc), Argv(argv){}
		void printHelp();
		std::string getInputFileName(){return InputFileName;} 		//入力ファイル名取得
		std::string getOutputFileName(){return OutputFileName;} 	//出力ファイル名取得
		std::string getLinkFileName(){return LinkFileName;} 	//リンク用ファイル名取得
		bool getWithJit(){return WithJit;}		//JIT実行有無
		bool getLazy(){return Lazy;}
		bool parseOption();
};

/**
 * ヘルプ表示
 */
void OptionParser::printHelp(){
	fprintf(stdout, "Compiler for Arow...\n" );
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

	Lazy = true;
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
		}else if(Argv[i][0]=='-' && Argv[i][1] == 'O' && Argv[i][2] == '\0'){
			Lazy = false;
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
	if (OutputFileName.empty() && (len > 5) &&
		ifn[len-5] == '.' && ifn[len-4] == 'a' && ifn[len-3] == 'r' && ifn[len-2] == 'o' && ifn[len-1] == 'w') {
		OutputFileName = std::string(ifn.begin(), ifn.end()-5); 
		OutputFileName += ".ll";
	} else if(OutputFileName.empty()){
		OutputFileName = ifn;
		OutputFileName += ".ll";
	}
	return true;
}

bool printASTs(TranslationUnitAST &tunit) {
	for(int i=0; ; i++) {
		FunctionAST *func = tunit.getFunction(i);
		if(!func)
			break;
		PrototypeAST *proto = func->getPrototype();
		fprintf(stderr, "%s(", proto->getName().c_str());
		for (int i = 0; ;i++)
			if (proto->getParamType(i) == Types(Type_null)) break;
			else fprintf(stderr, "%s -> ", proto->getParamType(i).printType().c_str()); 
		fprintf(stderr, "%s)\n", proto->getType().printType().c_str());
		for (int i = 0; ;i++)
			if (func->getBody()->getStatement(i))
				func->getBody()->getStatement(i)->printAST(1);
			else break;
	}
	return true;
}


class Compile {
	private:
		OptionParser opt;
		std::string filetext;
		Parser *parser;
		CodeGen *codegen;

	public:
		Compile(OptionParser opt) : opt(opt), filetext(opt.getInputFileName()), parser(), codegen(){}
		Parser *lex(std::string filetext);
		TranslationUnitAST &parse();
		CodeGen *LLVMGen();
		OptionParser getOption(){return opt;}
};

Parser *Compile::lex(std::string filetext) {
	parser = new Parser(filetext);
	return parser;
}

TranslationUnitAST &Compile::parse() {
	if (!parser->doParse()) {
		fprintf(stderr, "err at Parser or Lexer\n");
		SAFE_DELETE(parser);
		exit(1);
	}
	TranslationUnitAST &tunit = parser->getAST();
	if (tunit.empty()) {
		fprintf(stderr,"TranslationUnit is empty\n");
		SAFE_DELETE(parser);
		exit(1);
	}
	return tunit;
}

CodeGen *Compile::LLVMGen() {
	codegen = new CodeGen();
	TranslationUnitAST &tunit = parser->getAST();
	if (!codegen->doCodeGen(tunit, filetext, opt.getLinkFileName(), opt.getWithJit())){
		fprintf(stderr, "err at codegen\n");
		SAFE_DELETE(parser);
		SAFE_DELETE(codegen);
		exit(1);
	}
	//get Module
	llvm::Module &mod = codegen->getModule();
	if (mod.empty()) {
		fprintf(stderr,"Module is empty\n");
		SAFE_DELETE(parser);
		SAFE_DELETE(codegen);
		exit(1);
	}
	return codegen;
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

	std::string filetext = opt.getInputFileName();
	Compile compile(opt);
	//check
	if(filetext.length() == 0){
		fprintf(stderr,"入力ファイル名が指定されていません\n");
		exit(1);
	}

	left = clock();
	fprintf(stderr, "%.3f ms : File Read\n", (double)(left - right) * 1000 / CLOCKS_PER_SEC);
	right = left;

	// lex
	Parser *parser = compile.lex(filetext);
	// parser
	TranslationUnitAST &tunit = compile.parse();
	// print AST
	if (printStruct) printASTs(tunit);

	left = clock();
	fprintf(stderr, "%.3f ms : Lex and Parse\n", (double)(left - right) * 1000 / CLOCKS_PER_SEC);
	right = left;

	CodeGen *codegen = compile.LLVMGen();
	llvm::Module &mod = codegen->getModule();

	left = clock();
	fprintf(stderr, "%.3f ms : generate the LLVM IR\n", (double)(left - right) * 1000 / CLOCKS_PER_SEC);
	right = left;

	//llvm::PassManager<AnalysisManager<>> pm;
	llvm::legacy::PassManager pm;

	if (!opt.getLazy())
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
