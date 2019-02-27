//
// Created by Yunming Zhang on 2/13/17.
//
#include <graphit/midend/mir_printer.h>

namespace graphit {
    namespace mir {

        void MIRPrinter::visit(VarDecl::Ptr expr) {
            indent();
            printIndent();
            oss << "var decl " << expr->name;
            expr->accept(this);
            dedent();
        };

        void MIRPrinter::visit(ExprStmt::Ptr stmt) {
        }

        void MIRPrinter::visit(FuncDecl::Ptr func_decl) {
            indent();
            printIndent();
            oss << "func ";
            oss << func_decl->name << " ";

            oss << "(";

            bool printDelimiter = false;
            for (auto arg : func_decl->args) {
                if (printDelimiter) {
                    oss << ", ";
                }
                oss << arg.getName();
                //arg->accept(this);
                printDelimiter = true;
            }

            oss << ") ";

            func_decl->body->accept(this);
            //if (!func_decl->result) {

            //}
            dedent();
        };

        void MIRPrinter::visit(Expr::Ptr expr) {
            indent();
            printIndent();
            oss << "expr ";
            expr->accept(this);
            dedent();
        };

        void MIRPrinter::visit(IntLiteral::Ptr lit) {
            indent();
            printIndent();
            oss << lit->val;
            dedent();
        }

        void MIRPrinter::visit(AddExpr::Ptr expr) {
            printBinaryExpr(expr, "+");
        }

        void MIRPrinter::visit(SubExpr::Ptr expr) {
            printBinaryExpr(expr, "-");
        }

        void MIRPrinter::printBinaryExpr(BinaryExpr::Ptr expr, const std::string op) {
            oss << "(";
            expr->lhs->accept(this);
            oss << ") " << op << " (";
            expr->rhs->accept(this);
            oss << ")";
        }
        
        void MIRPrinter::visit(Stmt::Ptr stmt){};
        
        void MIRPrinter::visit(NameNode::Ptr name){};
        
        void MIRPrinter::visit(ForStmt::Ptr stmt) {};
        void MIRPrinter::visit(WhileStmt::Ptr stmt){};
        void MIRPrinter::visit(IfStmt::Ptr){};
        
        
        void MIRPrinter::visit(ForDomain::Ptr domain){};
        void MIRPrinter::visit(AssignStmt::Ptr stmt){};
        void MIRPrinter::visit(ReduceStmt::Ptr stmt){};
        void MIRPrinter::visit(CompareAndSwapStmt::Ptr stmt){};
        
        void MIRPrinter::visit(PrintStmt::Ptr stmt){};
        void MIRPrinter::visit(BreakStmt::Ptr stmt){};
        void MIRPrinter::visit(StmtBlock::Ptr stmt){};
        void MIRPrinter::visit(Call::Ptr call){};
        
        void MIRPrinter::visit(VertexSetApplyExpr::Ptr expr){};
        void MIRPrinter::visit(EdgeSetApplyExpr::Ptr expr){};
        
        void MIRPrinter::visit(PushEdgeSetApplyExpr::Ptr expr){};
        void MIRPrinter::visit(PullEdgeSetApplyExpr::Ptr expr){};
        void MIRPrinter::visit(HybridDenseEdgeSetApplyExpr::Ptr expr){};
        void MIRPrinter::visit(HybridDenseForwardEdgeSetApplyExpr::Ptr expr){};
        
        
        void MIRPrinter::visit(VertexSetWhereExpr::Ptr expr){};
        void MIRPrinter::visit(EdgeSetWhereExpr::Ptr expr){};
        
        
        void MIRPrinter::visit(TensorReadExpr::Ptr expr){};
        void MIRPrinter::visit(TensorArrayReadExpr::Ptr expr){};
        void MIRPrinter::visit(TensorStructReadExpr::Ptr expr){};
        
        void MIRPrinter::visit(BoolLiteral::Ptr lit){};
        void MIRPrinter::visit(StringLiteral::Ptr lit){};
        void MIRPrinter::visit(FloatLiteral::Ptr lit){};
        void MIRPrinter::visit(VertexSetAllocExpr::Ptr expr){};
        void MIRPrinter::visit(ListAllocExpr::Ptr expr){};
        void MIRPrinter::visit(VarExpr::Ptr expr){};
        void MIRPrinter::visit(EdgeSetLoadExpr::Ptr expr){};
        
        
        void MIRPrinter::visit(NegExpr::Ptr expr){};
        void MIRPrinter::visit(EqExpr::Ptr expr){};
        void MIRPrinter::visit(MulExpr::Ptr expr){};
        void MIRPrinter::visit(DivExpr::Ptr expr){};
        void MIRPrinter::visit(ScalarType::Ptr type){};
        void MIRPrinter::visit(StructTypeDecl::Ptr decl){};
        void MIRPrinter::visit(IdentDecl::Ptr decl){};
        void MIRPrinter::visit(ElementType::Ptr type){};
        void MIRPrinter::visit(VertexSetType::Ptr type){};
        void MIRPrinter::visit(ListType::Ptr type){};
        void MIRPrinter::visit(EdgeSetType::Ptr type){};
        void MIRPrinter::visit(VectorType::Ptr type){};
        

        std::ostream &operator<<(std::ostream &oss, MIRNode &node) {
            MIRPrinter printer(oss);
            node.accept(&printer);
            return oss;
        }

    }
}
