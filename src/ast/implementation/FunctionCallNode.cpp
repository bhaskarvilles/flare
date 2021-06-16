//
// Created by jobinrjohnson on 08/05/21.
//

#include <ast/FunctionCallNode.h>
#include <exceptions/SemanticException.h>
#include <ast/ClassDeclNode.h>
#include <ast/helpers/VariableHelper.h>

namespace flare::ast {


    llvm::Value *FunctionCallNode::codeGenObjectCreate(Context *cxt) {

        auto *node = cxt->findClassDeclaration(this->className);
        if (node == nullptr) {
            throw new exceptions::SemanticException("No declarations for class '"
                                                    + this->className
                                                    + "' found",
                                                    this->lineNumber);
        }

        auto *cNode = dynamic_cast<ClassDeclNode *>(node);
        auto *function = cNode->getInitFunction();

        return this->performCall(function, None);

    }

    llvm::Value *FunctionCallNode::codeGen(flare::ast::Context *cxt) {

        if (this->isObjectCreation()) {
            return this->codeGenObjectCreate(cxt);
        }

        if (this->isClassFunction()) {
            return this->codeGenObjectFunction(cxt);
        }

        auto calleeFunction = module->getFunction(this->functionName);
        if (calleeFunction == nullptr) {
            throw "Function not declared in the scope";
        }
        if (argumentList == nullptr) {
            return this->performCall(calleeFunction, None);
        }

        std::vector<Value *> calleeArgs;
        for (ExprNode *element: *(this->argumentList)) {
            calleeArgs.push_back(element->codeGen(cxt->nextLevel()));
        }

        return this->performCall(calleeFunction, calleeArgs);

    }

    FunctionCallNode::FunctionCallNode(std::string functionName) {
        this->functionName = functionName;
    }

    FunctionCallNode::FunctionCallNode(std::string functionName, std::vector<ExprNode *> *argumentList) {
        this->functionName = functionName;
        this->argumentList = argumentList;
    }

    FunctionCallNode::FunctionCallNode() {}

    llvm::Value *FunctionCallNode::codeGenObjectFunction(Context *cxt) {


        Node *vNode = cxt->findVariable(this->objectName);
        if (vNode == nullptr) {
            throw "no global variable declared in the scope";
        }
        auto *variable = dynamic_cast<VarDeclNode *>(vNode);
        VarType *varType = variable->getVariableType();

        if (varType->type != VariableType::VARTYPE_OBJECT || varType->typeRef->node == nullptr) {
            throw new exceptions::SemanticException("Function '"
                                                    + this->functionName
                                                    + "' is not a class method",
                                                    this->lineNumber);
        }

        ClassDeclNode *cnode = dynamic_cast<ClassDeclNode *>(varType->typeRef->node);

        auto calleeFunction = module->getFunction(cnode->getQualifiedClassName() + "::" + this->functionName);
        if (calleeFunction == nullptr) {
            throw "Function not declared in the scope";
        }

        std::vector<Value *> calleeArgs;
        calleeArgs.push_back(builder.CreateLoad(variable->getLLVMVarRef()));
        if (this->argumentList != nullptr) {
            for (ExprNode *element: *(this->argumentList)) {
                calleeArgs.push_back(element->codeGen(cxt->nextLevel()));
            }
        }

        return this->performCall(calleeFunction, calleeArgs);
    }

    llvm::Value *FunctionCallNode::performCall(Function *calleeFunction, ArrayRef<Value *> calleeArgs) {
        if (calleeFunction->getReturnType() == llvm::Type::getVoidTy(context)) {
            return builder.CreateCall(calleeFunction, calleeArgs);
        }

        return builder.CreateCall(calleeFunction, calleeArgs, this->functionName);
    }

}