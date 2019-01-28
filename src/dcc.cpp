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
		std::string LLVMFileName;
		std::string OutputFileName;
		std::string LinkFileName;
		std::string ArchName;
		bool Lazy;
		bool WithJit;
		int Argc;
		char **Argv;

	public:
		OptionParser(int argc, char **argv) : WithJit(false), Argc(argc), Argv(argv) {}
		void printHelp();
		std::string getInputFileName() {return InputFileName;} 		//入力ファイル名取得
		std::string getLLVMFileName() {return LLVMFileName;} 	//LLVM IRファイル名取得
		std::string getOutputFileName() {return OutputFileName;} 	//出力ファイル名取得
		std::string getLinkFileName() {return LinkFileName;} 	//リンク用ファイル名取得
		std::string getArchName() {return ArchName;}
		bool getWithJit() {return WithJit;}		//JIT実行有無
		bool getLazy() {return Lazy;}
		bool parseOption();
};

/**
 * ヘルプ表示
 */
void OptionParser::printHelp() {
	fprintf(stdout, "Compiler for Arow...\n" );
	fprintf(stdout, "試作中なのでバグがあったらご報告を\n" );
}

/**
 * オプション切り出し
 * @return 成功時：true　失敗時：false
 */
bool OptionParser::parseOption() {
	if (Argc < 2) {
		fprintf(stderr,"引数が足りません\n");
		return false;
	}

	Lazy = true;
	for (int i=1; i<Argc; i++) {
		// -o 出力ファイル名の選択
		if (Argv[i][0]=='-' && Argv[i][1] == 'o' && Argv[i][2] == '\0') {
			OutputFileName.assign(Argv[++i]);
		// -arch アーキテクチャ指定
		}else if (Argv[i][0]=='-' && Argv[i][1] == 'a' && Argv[i][2] == 'r' && Argv[i][3] == 'c' && Argv[i][4] == 'h' && Argv[i][5] == '\0') {
			ArchName.assign(Argv[++i]);
		// -h ヘルプの表示
		}else if (Argv[i][0]=='-' && Argv[i][1] == 'h' && Argv[i][2] == '\0') {
			printHelp();
			return false;
		// -l リンクオプション
		}else if (Argv[i][0]=='-' && Argv[i][1] == 'l' && Argv[i][2] == '\0') {
			LinkFileName.assign(Argv[++i]);
		// -jit JITコンパイル
		}else if (Argv[i][0]=='-' && Argv[i][1] == 'j' && Argv[i][2] == 'i' && Argv[i][3] == 't' && Argv[i][4] == '\0') {
			WithJit = true;
		// -O LLVMでの最適化適用
		}else if (Argv[i][0]=='-' && Argv[i][1] == 'O' && Argv[i][2] == '\0') {
			Lazy = false;
		}else if (Argv[i][0]=='-') {
			fprintf(stderr,"%s は不明なオプションです\n", Argv[i]);
			return false;
		}else{
			InputFileName.assign(Argv[i]);
		}
	}

	//OutputFileName
	std::string ifn = InputFileName;
	int len = ifn.length();
	if (LLVMFileName.empty() && (len > 5) &&
		ifn[len-5] == '.' && ifn[len-4] == 'a' && ifn[len-3] == 'r' && ifn[len-2] == 'o' && ifn[len-1] == 'w') {
		LLVMFileName = std::string(ifn.begin(), ifn.end()-5); 
		LLVMFileName += ".ll";
	} else if (LLVMFileName.empty()) {
		LLVMFileName = ifn;
		LLVMFileName += ".ll";
	}
	//OutputFileName
	if (OutputFileName.empty() && (len > 5) &&
		ifn[len-5] == '.' && ifn[len-4] == 'a' && ifn[len-3] == 'r' && ifn[len-2] == 'o' && ifn[len-1] == 'w') {
		OutputFileName = std::string(ifn.begin(), ifn.end()-5);
	} else if (OutputFileName.empty()) {
		OutputFileName = ifn;
	}
	return true;
}

bool printASTs(TranslationUnitAST &tunit) {
	for (int i=0; ; i++) {
		FunctionAST *func = tunit.getFunction(i);
		if (!func)
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
		Compile(OptionParser opt) : opt(opt), filetext(opt.getInputFileName()), parser(), codegen() {}
		Parser *lex(std::string filetext);
		TranslationUnitAST &parse();
		CodeGen *LLVMGen();
		OptionParser getOption() {return opt;}
};

Parser *Compile::lex(std::string filetext) {
	parser = new Parser(filetext);
	return parser;
}

TranslationUnitAST &Compile::parse() {
	if (!parser->doParse()) {
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
	if (!codegen->doCodeGen(tunit, filetext, opt.getLinkFileName(), opt.getWithJit())) {
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
	bool Progress = false;
	bool printStruct = false;
	clock_t start = clock();
	clock_t left, right = clock();
	llvm::InitializeNativeTarget();
	llvm::sys::PrintStackTraceOnErrorSignal(argv[0]);
	llvm::PrettyStackTraceProgram X(argc, argv);

	llvm::EnableDebugBuffering = true;

	OptionParser opt(argc, argv);
	if (!opt.parseOption())
		exit(1);

	std::string filetext = opt.getInputFileName();
	Compile compile(opt);
	//check
	if (filetext.length() == 0) {
		fprintf(stderr,"入力ファイル名が指定されていません\n");
		exit(1);
	}

	left = clock();
	if (Progress) fprintf(stderr, "%.3f ms : File Read\n", (double)(left - right) * 1000 / CLOCKS_PER_SEC);
	right = left;

	// lex
	Parser *parser = compile.lex(filetext);
	// parser
	TranslationUnitAST &tunit = compile.parse();
	// print AST
	if (printStruct) printASTs(tunit);

	left = clock();
	if (Progress) fprintf(stderr, "%.3f ms : Lex and Parse\n", (double)(left - right) * 1000 / CLOCKS_PER_SEC);
	right = left;

	CodeGen *codegen = compile.LLVMGen();
	llvm::Module &mod = codegen->getModule();

	left = clock();
	if (Progress) fprintf(stderr, "%.3f ms : generate the LLVM IR\n", (double)(left - right) * 1000 / CLOCKS_PER_SEC);
	right = left;

	//llvm::PassManager<AnalysisManager<>> pm;
	llvm::legacy::PassManager pm;

	if (!opt.getLazy())
		pm.add(llvm::createPromoteMemoryToRegisterPass());
	
	//出力
	std::error_code error;
	llvm::raw_fd_ostream raw_stream(opt.getLLVMFileName().c_str(), error, llvm::sys::fs::OpenFlags::F_None);
	pm.add(llvm::createPrintModulePass(raw_stream));
	pm.run(mod);
	raw_stream.close();

	//delete
	SAFE_DELETE(parser);
	SAFE_DELETE(codegen);
	left = clock();
	if (Progress) fprintf(stderr, "%.3f ms : write the LLVM file\n", (double)(left - right) * 1000 / CLOCKS_PER_SEC);
	right = left;

	if (opt.getArchName().empty()) {
		system(("llc -o test.o " + opt.getLLVMFileName() + " -filetype=obj").c_str());
		system(("gcc test.o lib.o -o " + opt.getOutputFileName()).c_str());
		system("rm test.o");
	}else{
		system(("llc -o " + opt.getOutputFileName() + "-diffarch.o " + opt.getLLVMFileName() + " -filetype=obj -mtriple=" + opt.getArchName() + "-unknown-linux-gnueabihf").c_str());
		system(("scp ./" + opt.getOutputFileName() + "-diffarch.o pi@raspberrypi.local:~/GPIO/").c_str());
	}
	left = clock();
	if (Progress) fprintf(stderr, "%.3f ms : compile the file\n", (double)(left - right) * 1000 / CLOCKS_PER_SEC);

	if (Progress) fprintf(stderr, "compile has been completed! total compile time is %.3f ms\n", (double)(left - start) * 1000 / CLOCKS_PER_SEC);
	return 0;
}
