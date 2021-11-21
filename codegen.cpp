#include "node.h"
#include "parser.hpp"

std::unique_ptr<LLVMContext> TheContext;
std::unique_ptr<Module> TheModule;
std::unique_ptr<IRBuilder<>> Builder;
std::map<std::string, Value *> IdentTable;

Value *NNumber::codegen() {
	return ConstantFP::get(*TheContext, APFloat(val));
}

Value *NIdentifier::codegen() {
	Value *V = IdentTable[name];
	//找不到该标识符
	if (!V) {
		puts("unknown identifier");
		return nullptr;
	}
	return V;
}

//生成表达式
Value *NBinaryExpr::codegen() {
	// printf("binary expression %d\n", Opcode);
	Value *L = LHS->codegen();
	Value *R = RHS->codegen();
	if (!L || !R) return nullptr;

	switch (Opcode) {
		case TPLUS:
			return Builder->CreateFAdd(L, R, "addtmp");
		case TMINUS:
			return Builder->CreateFSub(L, R, "subtmp");
		case TMUL:
			return Builder->CreateFMul(L, R, "multmp");
		case TDIV:
			return Builder->CreateFDiv(L, R, "divtmp");
	}
	return nullptr;
}

Function *NPrototype::codegen() {
	//函数的参数列表，这里全是double
	std::vector<Type *> Doubles(Args.size(), Type::getDoubleTy(*TheContext));
	FunctionType *FT = FunctionType::get(Type::getDoubleTy(*TheContext), Doubles, false);

	//创建函数
	Function *F = Function::Create(FT, Function::ExternalLinkage, Name, TheModule.get());

	int i = 0;
	for (auto &arg : F->args()) arg.setName(Args[i++]);

	return F;
}

Function *NFunction::codegen() {
	//根据函数声明构造一个函数
	Function *F = Proto->codegen();

	if (!F) return nullptr;

	//添加函数入口
	Builder->SetInsertPoint(BasicBlock::Create(*TheContext, "funcEntry", F));

	//将函数参数的标识符引用保存下来
	IdentTable.clear();
	for (auto &arg : F->args()) IdentTable[std::string(arg.getName())] = &arg;

	Value *ret = Body->codegen();
	if (!ret) return nullptr;
	//添加函数返回
	Builder->CreateRet(ret);

	//验证函数
	verifyFunction(*F);
	return F;
}