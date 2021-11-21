#include <iostream>
#include "node.h"

using namespace std;

extern int yyparse();
extern NFunction *body; //body为AST的根节点
extern std::unique_ptr<LLVMContext> TheContext;
extern std::unique_ptr<Module> TheModule;
extern std::unique_ptr<IRBuilder<>> Builder;

int main() {
	//调用yyparse， 生成AST
	yyparse();

	TheContext = std::make_unique<LLVMContext>();
	TheModule = std::make_unique<Module>("dfc - ddstkm functional compiler", *TheContext);
	Builder = std::make_unique<IRBuilder<>>(*TheContext);

	//开始生成中间码
	body->codegen();

	TheModule->print(errs(), nullptr);

	//下面是生成机器码的部分

	InitializeAllTargetInfos();
	InitializeAllTargets();
	InitializeAllTargetMCs();
	InitializeAllAsmParsers();
	InitializeAllAsmPrinters();

	auto TargetTriple = sys::getDefaultTargetTriple();
	TheModule->setTargetTriple(TargetTriple);

	std::string Error;
	auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);

	// Print an error and exit if we couldn't find the requested target.
	// This generally occurs if we've forgotten to initialise the
	// TargetRegistry or we have a bogus target triple.
	if (!Target) {
	  errs() << Error;
	  return 1;
	}

	auto CPU = "generic";
	auto Features = "";

	TargetOptions opt;
	auto RM = Optional<Reloc::Model>();
	auto TheTargetMachine =
		Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

	TheModule->setDataLayout(TheTargetMachine->createDataLayout());

	auto Filename = "output.o";
	std::error_code EC;
	raw_fd_ostream dest(Filename, EC, sys::fs::OF_None);

	if (EC) {
		errs() << "Could not open file: " << EC.message();
		return 1;
	}

	legacy::PassManager pass;
	auto FileType = CGFT_ObjectFile;

	if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
		errs() << "TheTargetMachine can't emit a file of this type";
		return 1;
	}

	pass.run(*TheModule);
	dest.flush();

	outs() << "Wrote " << Filename << "\n";

	return 0;
}

