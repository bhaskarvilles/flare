//
// Created by jobinrjohnson on 05/05/21.
//

#include <ast/helpers/Context.h>
#include <ast/StatementListNode.h>

#include <types/IntType.h>
#include <types/StringType.h>

#include <iostream>
#include <types/BoolType.h>
#include <types/DoubleType.h>
#include <types/VoidType.h>

namespace flare::ast {

    void Context::pushFunction(FunctionNode *function) {
        functions.push(function);
    }

    void Context::popFunction() {
        functions.pop();
    }

    FunctionNode *Context::getCurrentFunction() {
        if (this->functions.empty()) {
            return nullptr;
        }
        return functions.top();
    }

    void Context::pushStatementList(ast::Node *node) {
        this->statementList.push_back(node);
    }

    void Context::popStatementList() {
        this->statementList.pop_back();
    }

    Node *Context::getCurrentStatementList() {
        if (this->statementList.empty()) {
            return nullptr;
        }
        return statementList.back();
    }

    llvm::Value *Context::findLocal() {
        return nullptr;
    }

    VarDeclNode *Context::findVariable(std::string name) {
        VarDeclNode *variable = nullptr;
        std::vector<Node *>::iterator i = this->statementList.end();
        while (i != this->statementList.begin()) {
            StatementListNode *node = dynamic_cast<StatementListNode *>(*(--i));
            variable = node->findLocal(name);
            if (variable != nullptr) {
                return variable;
            }
        }
        return nullptr;
    }

    BaseType *Context::findType(std::string name) {
        auto val = this->types.find(name);
        if (val != this->types.end()) {
            return val->second;
        }
        return nullptr;
    }

    bool Context::registerType(std::string name, BaseType *type) {

        if (this->findType(name) != nullptr) {
            throw "already in";
        }

        this->types.insert(std::pair<std::string, BaseType *>(name, type));
        return true;
    }


    BaseType *Context::getFlareType(ast::VariableType type) {

        switch (type) {

            case VARTYPE_INT_64:
            case VARTYPE_INT:
                return this->findType("int");
            case VARTYPE_DOUBLE:
            case VARTYPE_NUMBER:
                return this->findType("double");
            case VARTYPE_BOOLEAN:
                return this->findType("boolean");
            case VARTYPE_STRING:
                return this->findType("string");
            case VARTYPE_VOID:
                return this->findType("void");
            case VARTYPE_INT_32:
            case VARTYPE_FLOAT:
            case VARTYPE_ARRAY:
            case OTHER:
                break;
        }

        throw "this wont work";
    }

    BaseType *Context::getFlareType(VarType type) {

        if (type.type == VariableType::OTHER) {
            return this->findType(type.name);
        }

        return this->getFlareType(type.type);
    }

    BaseType *Context::getFlareType(Value *value) {
        Type *valType = value->getType();

        if (valType->isPointerTy()) {
            valType = valType->getPointerElementType();
        }

        for (auto const &x : this->types) {
            if (x.second->getLLVMType(this) == valType) {
                return x.second;
            }

        }

        throw "Type not found";
    }

    void Context::initTypes() {
        this->registerType("int", new IntType());
        this->registerType("string", new StringType());
        this->registerType("boolean", new BoolType());
        this->registerType("double", new DoubleType());
        this->registerType("void", new VoidType());
    }


}