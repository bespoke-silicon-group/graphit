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
            expr->initVal->accept(this);
            dedent();
        }

        void MIRPrinter::visit(ExprStmt::Ptr stmt) {
            indent();
            printIndent();
            oss << "expr stmt: " << stmt->stmt_label;
            stmt->expr->accept(this);
            dedent();
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
        }

        void MIRPrinter::visit(Expr::Ptr expr) {
            indent();
            printIndent();
            oss << "expr ";
            //TODO(Emily): is this terminal?
            //expr->accept(this);
            dedent();
        }

        void MIRPrinter::visit(IntLiteral::Ptr lit) {
            indent();
            printIndent();
            oss << lit->val;
            dedent();
        }

        void MIRPrinter::visit(AddExpr::Ptr expr) {
            indent();
            printIndent();
            printBinaryExpr(expr, "+");
            dedent();
        }

        void MIRPrinter::visit(SubExpr::Ptr expr) {
            indent();
            printIndent();
            printBinaryExpr(expr, "-");
            dedent();
        }

        void MIRPrinter::printBinaryExpr(BinaryExpr::Ptr expr, const std::string op) {
            oss << "(";
            expr->lhs->accept(this);
            oss << ") " << op << " (";
            expr->rhs->accept(this);
            oss << ")";
        }
        
        void MIRPrinter::visit(Stmt::Ptr stmt){
            indent();
            printIndent();
            oss << "statement: " << stmt->stmt_label;
            //TODO(Emily): is this terminal?
            //stmt->accept(this);
            dedent();
        }
        
        void MIRPrinter::visit(NameNode::Ptr name){
            indent();
            printIndent();
            oss << "name node: " << name->stmt_label;
            name->body->accept(this);
            dedent();
        }
        
        void MIRPrinter::visit(ForStmt::Ptr stmt) {
            indent();
            printIndent();
            oss << "for stmt: " << stmt->stmt_label;
            stmt->domain->accept(this);
            stmt->body->accept(this);
            dedent();
        }
        
        void MIRPrinter::visit(WhileStmt::Ptr stmt){
            indent();
            printIndent();
            oss << "while stmt: " << stmt->stmt_label;
            stmt->cond->accept(this);
            stmt->body->accept(this);
            dedent();
        }
        
        void MIRPrinter::visit(IfStmt::Ptr stmt){
            indent();
            printIndent();
            oss << "if stmt: " << stmt->stmt_label;
            stmt->cond->accept(this);
            stmt->ifBody->accept(this);
            stmt->elseBody->accept(this);
            dedent();
        }
        
        
        void MIRPrinter::visit(ForDomain::Ptr domain){
            indent();
            printIndent();
            oss << "for domain";
            domain->upper->accept(this);
            domain->lower->accept(this);
            dedent();
        }
        
        void MIRPrinter::visit(AssignStmt::Ptr stmt){
            indent();
            printIndent();
            oss << "assign stmt: " << stmt->stmt_label;
            stmt->lhs->accept(this);
            dedent();
        }
        
        void MIRPrinter::visit(ReduceStmt::Ptr stmt){
            indent();
            printIndent();
            oss << "reduce stmt: " << stmt->stmt_label;
            //TODO(Emily): is this terminal?
            //stmt->reduce_op_;
            dedent();
        }
        
        void MIRPrinter::visit(CompareAndSwapStmt::Ptr stmt){
            indent();
            printIndent();
            oss << "comp and swap: " << stmt->stmt_label;
            stmt->compare_val_expr->accept(this);
            dedent();
        }
        
        void MIRPrinter::visit(PrintStmt::Ptr stmt){
            indent();
            printIndent();
            oss << "print stmt: " << stmt->stmt_label;
            stmt->expr->accept(this);
            dedent();
        }
        
        void MIRPrinter::visit(BreakStmt::Ptr stmt){
            indent();
            printIndent();
            oss << "break stmt: " << stmt->stmt_label;
            //TODO(Emily): is this terminal
            dedent();
        }
        
        void MIRPrinter::visit(StmtBlock::Ptr stmt){
            indent();
            printIndent();
            oss << "stmt block: " << stmt->stmt_label;
            //TODO(Emily): need to determine how to iterate through stmts in block
            /*for(auto s : stmt->stmts)
            {
             s->accept(this);
            }
             */
            dedent();
        }
        
        void MIRPrinter::visit(Call::Ptr call){
            indent();
            printIndent();
            oss << "call: " << call->name;
            for(auto arg : call->args)
            {
                arg->accept(this);
            }
            dedent();
            
        }
        
        void MIRPrinter::visit(VertexSetApplyExpr::Ptr expr){
            indent();
            printIndent();
            oss << "vertex set apply expr: " << expr->input_function_name;
            expr->target->accept(this);
            dedent();
        }
        
        void MIRPrinter::visit(EdgeSetApplyExpr::Ptr expr){
            indent();
            printIndent();
            oss << "edge set apply expr: " << expr->input_function_name;
            expr->target->accept(this);
            dedent();
        }
        
        void MIRPrinter::visit(PushEdgeSetApplyExpr::Ptr expr){
            indent();
            printIndent();
            oss << "push edge set apply expr: " << expr->input_function_name;
            expr->target->accept(this);
            dedent();
        }
        
        void MIRPrinter::visit(PullEdgeSetApplyExpr::Ptr expr){
            indent();
            printIndent();
            oss << "pull edge set apply expr: " << expr->input_function_name;
            expr->target->accept(this);
            dedent();
        }
        
        void MIRPrinter::visit(HybridDenseEdgeSetApplyExpr::Ptr expr){
            indent();
            printIndent();
            oss << "hybrid dense edge set apply expr: " << expr->input_function_name;
            expr->target->accept(this);
            dedent();
        }
        
        void MIRPrinter::visit(HybridDenseForwardEdgeSetApplyExpr::Ptr expr){
            indent();
            printIndent();
            oss << "hybrid dense forward edge set apply expr: " << expr->input_function_name;
            expr->target->accept(this);
            dedent();
        }
        
        
        void MIRPrinter::visit(VertexSetWhereExpr::Ptr expr){
            indent();
            printIndent();
            oss << "vertex set where expr: " << expr->input_func;
            //TODO(Emily): is this terminal
            dedent();
        }
        
        void MIRPrinter::visit(EdgeSetWhereExpr::Ptr expr){
            indent();
            printIndent();
            oss << "edge set where expr: " << expr->input_func;
            //TODO(Emily): is this terminal
            dedent();
        }
        
        
        void MIRPrinter::visit(TensorReadExpr::Ptr expr){
            indent();
            printIndent();
            oss << expr->getIndexNameStr() << " " << expr->getTargetNameStr();
            expr->index->accept(this);
            expr->target->accept(this);
            dedent();
        }
        
        void MIRPrinter::visit(TensorArrayReadExpr::Ptr expr){
            indent();
            printIndent();
            oss << expr->getIndexNameStr() << " " << expr->getTargetNameStr();
            expr->index->accept(this);
            expr->target->accept(this);
            dedent();
        }
        
        void MIRPrinter::visit(TensorStructReadExpr::Ptr expr){
            indent();
            printIndent();
            oss << expr->getIndexNameStr() << " " << expr->getTargetNameStr();
            expr->field_target->accept(this);
            dedent();
        }
        
        void MIRPrinter::visit(BoolLiteral::Ptr lit){
            indent();
            printIndent();
            oss << lit->val;
            dedent();
        }
        
        void MIRPrinter::visit(StringLiteral::Ptr lit){
            indent();
            printIndent();
            oss << lit->val;
            dedent();
        }
        
        void MIRPrinter::visit(FloatLiteral::Ptr lit){
            indent();
            printIndent();
            oss << lit->val;
            dedent();
        }
        
        void MIRPrinter::visit(VertexSetAllocExpr::Ptr expr){
            indent();
            printIndent();
            oss << "vertex set alloc expr";
            expr->size_expr->accept(this);
            dedent();
        }
        
        void MIRPrinter::visit(ListAllocExpr::Ptr expr){
            indent();
            printIndent();
            oss << "list alloc expr";
            expr->element_type->accept(this);
            expr->size_expr->accept(this);
            dedent();
        }
        
        void MIRPrinter::visit(VarExpr::Ptr expr){
            indent();
            printIndent();
            oss << expr->var.getName();
            //TODO(Emily): anything else to do here?
            dedent();
            
        }
        
        void MIRPrinter::visit(EdgeSetLoadExpr::Ptr expr){
            indent();
            printIndent();
            oss << "edge set load expr";
            expr->file_name->accept(this);
            dedent();
        }
        
        
        void MIRPrinter::visit(NegExpr::Ptr expr){
            indent();
            printIndent();
            oss << "!";
            expr->operand->accept(this);
            dedent();
        }
        
        void MIRPrinter::visit(EqExpr::Ptr expr){
            indent();
            printIndent();
            oss << "==";
            //TODO(Emily): expr->ops? list of Op type
            for(auto arg : expr->operands)
            {
                arg->accept(this);
            }
            dedent();
        }
        
        void MIRPrinter::visit(MulExpr::Ptr expr){
            indent();
            printIndent();
            printBinaryExpr(expr, "*");
            dedent();
        }
        
        void MIRPrinter::visit(DivExpr::Ptr expr){
            indent();
            printIndent();
            printBinaryExpr(expr, "/");
            dedent();
        }
        
        void MIRPrinter::visit(ScalarType::Ptr type){
            indent();
            printIndent();
            //TODO(Emily): confirm this is terminal
            oss << "Scalar Type";
            dedent();
            
        }
        
        void MIRPrinter::visit(StructTypeDecl::Ptr decl){
            indent();
            printIndent();
            oss << decl->name;
            for(auto field : decl->fields)
            {
                field->accept(this);
            }
            dedent();
        }
        
        void MIRPrinter::visit(IdentDecl::Ptr decl){
            indent();
            printIndent();
            oss << decl->name;
            decl->type->accept(this);
            dedent();
        }
        
        void MIRPrinter::visit(ElementType::Ptr type){
            indent();
            printIndent();
            oss << type->ident;
            dedent();
        }
        
        void MIRPrinter::visit(VertexSetType::Ptr type){
            indent();
            printIndent();
            oss << "Vertex set type " << type->element->ident;
            //TODO(Emily): not sure this is the correct way to print this
            dedent();
        }
        
        void MIRPrinter::visit(ListType::Ptr type){
            indent();
            printIndent();
            oss << "List type ";
            type->element_type->accept(this);
            dedent();
        }
        
        void MIRPrinter::visit(EdgeSetType::Ptr type){
            indent();
            printIndent();
            oss << "edge set type: ";
            type->element->accept(this);
            type->weight_type->accept(this);
            dedent();
        }
        
        void MIRPrinter::visit(VectorType::Ptr type){
            indent();
            printIndent();
            oss << type->typedef_name_; //TODO(Emily): is this what we want to print?
            type->element_type->accept(this);
            type->vector_element_type->accept(this);
            dedent();
        }
        

        std::ostream &operator<<(std::ostream &oss, MIRNode &node) {
            MIRPrinter printer(oss);
            node.accept(&printer);
            return oss;
        }

    }
}
