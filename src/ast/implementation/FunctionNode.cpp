//
// Created by jobinrjohnson on 05/05/21.
//

#include "FunctionNode.h"

NodeType ast::FunctionNode::getNodeType() {
    return FUNCTION_NODE;
}

llvm::Value *ast::FunctionNode::codeGen(Context *cxt) {

    this->printCallStack(cxt, "FunctionNode", __FUNCTION__);

    cxt->pushFunction(this);


    std::vector<llvm::Type *> argVector(0, llvm::Type::getDoubleTy(context));
    llvm::FunctionType *functionRetType = llvm::FunctionType::get(llvm::Type::getInt32Ty(context),
                                                                  argVector, false);

    this->function = llvm::Function::Create(functionRetType, llvm::GlobalValue::ExternalLinkage,
                                            this->name, module.get());

    this->prepareBlocks();

    builder.SetInsertPoint(this->entryBlock);
    this->retValue = new AllocaInst(function->getReturnType(), 0, "retVal", this->entryBlock);
    builder.CreateStore(ConstantInt::get(context, APInt(32, 0)), retValue);
    this->statementListNode->codeGen(cxt);
    if (builder.GetInsertBlock()->getTerminator() == nullptr) {
        builder.CreateBr(this->exitBlock);
    }

    builder.SetInsertPoint(this->exitBlock);
    LoadInst *l = builder.CreateLoad(this->retValue);
    builder.CreateRet(l);

    llvm::verifyFunction(*function, &(llvm::errs()));

    cxt->popFunction();

    return nullptr;
}

ast::FunctionNode::FunctionNode(const char *name, ast::StatementListNode *statements) {
    this->name = name;
    this->statementListNode = statements;
}

void ast::FunctionNode::prepareBlocks() {

    this->entryBlock = llvm::BasicBlock::Create(context, "entry", this->function);
    this->exitBlock = llvm::BasicBlock::Create(context, "exit", this->function);

}

void ast::FunctionNode::setHasMultipleExits() {
    this->hasMultipleExits = true;
}

void ast::FunctionNode::createLocal(const std::string& varName, Value *value) {
    this->locals.insert(std::pair<std::string, Value *>(varName, value));
}

Value *ast::FunctionNode::findLocal(const std::string& varName) {
    auto val = this->locals.find(varName);
    if (val != this->locals.end()) {
        return val->second;
    }
    return nullptr;
}

