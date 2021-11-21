#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>
using namespace llvm;

class NExpr {
	public:
	virtual ~NExpr() = default;
	virtual Value *codegen() = 0;
};

class NNumber : public NExpr {
	double val;

	public:
	NNumber(double val) : val(val) {}
	Value* codegen() override;
};

class NIdentifier : public NExpr {
	std::string name;

	public:
	NIdentifier(const std::string &name) : name(name) {}
	Value* codegen() override;
	const std::string &getName() const { return name; }
};

class NBinaryExpr : public NExpr {
	int Opcode;
	NExpr *LHS, *RHS;
	
	public:
	NBinaryExpr(int Opcode, NExpr *LHS, NExpr *RHS)
		: Opcode(Opcode), LHS(LHS), RHS(RHS) {}
	Value* codegen() override;
};

class NPrototype {
	std::string Name;
	std::vector<std::string> Args;
	
	public:
	NPrototype(std::string Name, std::vector<std::string> Args)
		: Name(Name), Args(std::move(Args)) {}
	Function *codegen();
	const std::string &getName() const { return Name; }
};

class NFunction {
	NPrototype *Proto;
	NExpr *Body;

	public:
	NFunction(NPrototype *Proto, NExpr *Body)
		: Proto(std::move(Proto)), Body(std::move(Body)) {}
	Function* codegen();
};