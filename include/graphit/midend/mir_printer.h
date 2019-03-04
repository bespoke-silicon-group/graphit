//
// Created by Yunming Zhang on 2/13/17.
//

#ifndef GRAPHIT_MIR_PRINTER_H
#define GRAPHIT_MIR_PRINTER_H
#include <iostream>
#include <graphit/midend/mir.h>
#include <graphit/midend/mir_visitor.h>

namespace graphit {
    namespace mir {
        struct MIRPrinter : public MIRVisitor {
            MIRPrinter(std::ostream &oss) : oss(oss), indentLevel(0) {}

            //void printMIR(MIR::Program::Ptr program){program->accept(this);};

        protected:


            //Delayed for later
            virtual void visit(Stmt::Ptr);
            
            virtual void visit(NameNode::Ptr);
            
            virtual void visit(ForStmt::Ptr);
            virtual void visit(WhileStmt::Ptr);
            virtual void visit(IfStmt::Ptr);
            
            
            virtual void visit(ForDomain::Ptr);
            virtual void visit(AssignStmt::Ptr);
            virtual void visit(ReduceStmt::Ptr);
            virtual void visit(CompareAndSwapStmt::Ptr);
            
            virtual void visit(PrintStmt::Ptr);
            virtual void visit(BreakStmt::Ptr);
            virtual void visit(ExprStmt::Ptr);
            virtual void visit(StmtBlock::Ptr);
            virtual void visit(Expr::Ptr);
            virtual void visit(Call::Ptr);
            
            virtual void visit(VertexSetApplyExpr::Ptr);
            virtual void visit(EdgeSetApplyExpr::Ptr);
            
            virtual void visit(PushEdgeSetApplyExpr::Ptr);
            virtual void visit(PullEdgeSetApplyExpr::Ptr);
            virtual void visit(HybridDenseEdgeSetApplyExpr::Ptr);
            virtual void visit(HybridDenseForwardEdgeSetApplyExpr::Ptr);
            
            
            virtual void visit(VertexSetWhereExpr::Ptr);
            virtual void visit(EdgeSetWhereExpr::Ptr);
            
            
            virtual void visit(TensorReadExpr::Ptr);
            virtual void visit(TensorArrayReadExpr::Ptr);
            virtual void visit(TensorStructReadExpr::Ptr);
            
            virtual void visit(BoolLiteral::Ptr);
            virtual void visit(StringLiteral::Ptr);
            virtual void visit(FloatLiteral::Ptr);
            virtual void visit(IntLiteral::Ptr);
            virtual void visit(VertexSetAllocExpr::Ptr);
            virtual void visit(ListAllocExpr::Ptr);
            virtual void visit(VarExpr::Ptr);
            virtual void visit(EdgeSetLoadExpr::Ptr);
            
            
            virtual void visit(NegExpr::Ptr);
            virtual void visit(EqExpr::Ptr);
            virtual void visit(AddExpr::Ptr);
            virtual void visit(SubExpr::Ptr);
            virtual void visit(MulExpr::Ptr);
            virtual void visit(DivExpr::Ptr);
            virtual void visit(Type::Ptr){};
            virtual void visit(ScalarType::Ptr);
            virtual void visit(StructTypeDecl::Ptr);
            virtual void visit(VarDecl::Ptr);
            virtual void visit(IdentDecl::Ptr);
            virtual void visit(FuncDecl::Ptr);
            virtual void visit(ElementType::Ptr);
            virtual void visit(VertexSetType::Ptr);
            virtual void visit(ListType::Ptr);
            virtual void visit(EdgeSetType::Ptr);
            virtual void visit(VectorType::Ptr);
            
            
            //virtual void visitBinaryExpr(BinaryExpr::Ptr);
            //virtual void visitNaryExpr(NaryExpr::Ptr);


            void indent() { ++indentLevel; }
            void dedent() { --indentLevel; }
            void printIndent() { oss << "\n" << std::string(2 * indentLevel, ' '); }

            void printBinaryExpr(BinaryExpr::Ptr expr, const std::string op);


            std::ostream &oss;
            unsigned      indentLevel;

        };
    }
}

#endif //GRAPHIT_MIR_PRINTER_H
