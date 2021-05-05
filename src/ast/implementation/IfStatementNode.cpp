//
// Created by jobinrjohnson on 8/8/20.
//

#include "IfStatementNode.h"

namespace ast {


    NodeType IfStatementNode::getNodeType() {
        return IF_NODE;
    }

    IfStatementNode::IfStatementNode(Node *cond, Node *smt) {

        this->statementList.push_back(smt);
        this->condition.push_back(cond);

    }

    void IfStatementNode::addBranch(Node *cond, Node *smt) {

        this->statementList.push_back(smt);
        this->condition.push_back(cond);

    }

    void IfStatementNode::addElseBranch(Node *smt) {
        this->statementList.push_back(smt);

        auto node = new ValuePlaceholderNode(ConstantInt::get(Type::getInt1Ty(context), 1, false));


        this->condition.push_back(node);
    }

    llvm::Value *IfStatementNode::codegenIfElseIf(Context *cxt) {

        llvm::Function *function = builder.GetInsertBlock()->getParent();
        llvm::BasicBlock *mergeBlock = llvm::BasicBlock::Create(context, "merge");

        for (unsigned long i = 0; i < this->condition.size(); ++i) {

            llvm::BasicBlock *elseIfBlock = llvm::BasicBlock::Create(context, "elseif");
            llvm::BasicBlock *thenBlock = llvm::BasicBlock::Create(context, "then");

            builder.CreateCondBr(
                    this->condition[i]->codeGen(cxt->nextLevel()),
                    thenBlock,
                    elseIfBlock
            );

            function->getBasicBlockList().push_back(thenBlock);
            builder.SetInsertPoint(thenBlock);
            this->statementList[i]->codeGen(cxt->nextLevel());
            if (thenBlock->getTerminator() == nullptr) {
                builder.CreateBr(mergeBlock);
            }
            function->getBasicBlockList().push_back(elseIfBlock);
            builder.SetInsertPoint(elseIfBlock);

            if (i == this->condition.size() - 1 && elseIfBlock->getTerminator() == nullptr) {
                builder.CreateBr(mergeBlock);
            }

        }

        function->getBasicBlockList().push_back(mergeBlock);
        builder.SetInsertPoint(mergeBlock);

        return nullptr;

    }


    llvm::Value *IfStatementNode::codeGen(Context *cxt) {
        this->printCallStack(cxt, "IfStatementNode", __FUNCTION__);
        return this->codegenIfElseIf(cxt);
    }
}