#ifndef GRAPHIT_CODEGEN_HB_H
#define GRAPHIT_CODEGEN_HB_H

#include <graphit/midend/mir.h>
#include <graphit/midend/mir_visitor.h>
#include <graphit/midend/mir_context.h>

#include <iostream>
#include <sstream>

#define DEFAULT(T) virtual void visit(mir::T::Ptr x) { std::cerr << "Is a " #T "\n";}


namespace graphit {
    class CodeGenHB : mir::MIRVisitor {
    public:
        CodeGenHB(std::ostream &input_oss, MIRContext *mir_context) : oss(input_oss), mir_context_(mir_context), indentLevel(0), current_context(mir::FuncDecl::CONTEXT_HOST){
            //TODO(Emily): look at the C++ codegen backend to see what happens here
            //edgeset_apply_func_gen_ = new EdgesetApplyFunctionDeclGenerator(mir_context_, oss);
        }
        int genHBCode(void);
        
    protected:
        
        
        //TODO(Emily): implement these function stubs
        /*
        int genVertexSets(void);
        int genPropertyArrayDecl(mir::VarDecl::Ptr);
        int genPropertyArrayAlloca(mir::VarDecl::Ptr);
        int fillLambdaBody(mir::FuncDecl::Ptr, std::vector<std::string>);
        std::string getAllGlobals(void);
        
        
        
         virtual void visit(mir::ForStmt::Ptr);
         virtual void visit(mir::WhileStmt::Ptr);
         virtual void visit(mir::IfStmt::Ptr);
         
         
         virtual void visit(mir::ExprStmt::Ptr);
         virtual void visit(mir::AssignStmt::Ptr);
         virtual void visit(mir::ReduceStmt::Ptr);
         virtual void visit(mir::CompareAndSwapStmt::Ptr);
         
         virtual void visit(mir::PrintStmt::Ptr);
         virtual void visit(mir::BreakStmt::Ptr);
         */
         virtual void visit(mir::FuncDecl::Ptr);
         /*
         virtual void visit(mir::Call::Ptr);
         
         //virtual void visit(mir::TensorReadExpr::Ptr);
         virtual void visit(mir::TensorStructReadExpr::Ptr);
         virtual void visit(mir::TensorArrayReadExpr::Ptr);
         
         virtual void visit(mir::VertexSetAllocExpr::Ptr);
         virtual void visit(mir::ListAllocExpr::Ptr);
         
         //functional operators
         virtual void visit(mir::VertexSetApplyExpr::Ptr);
         virtual void visit(mir::PullEdgeSetApplyExpr::Ptr);
         virtual void visit(mir::PushEdgeSetApplyExpr::Ptr);
         
         virtual void visit(mir::VertexSetWhereExpr::Ptr);
         //virtual void visit(mir::EdgeSetWhereExpr::Ptr);
         
         virtual void visit(mir::VarExpr::Ptr);
         virtual void visit(mir::EdgeSetLoadExpr::Ptr);
         
         virtual void visit(mir::NegExpr::Ptr);
         virtual void visit(mir::EqExpr::Ptr);
         
         
         virtual void visit(mir::MulExpr::Ptr);
         virtual void visit(mir::DivExpr::Ptr);
         virtual void visit(mir::AddExpr::Ptr);
         virtual void visit(mir::SubExpr::Ptr);
         
         
         virtual void visit(mir::BoolLiteral::Ptr);
         virtual void visit(mir::StringLiteral::Ptr);
         virtual void visit(mir::FloatLiteral::Ptr);
         virtual void visit(mir::IntLiteral::Ptr);
         
         
         virtual void visit(mir::VarDecl::Ptr); */
         virtual void visit(mir::ElementType::Ptr element_type);
         /*
         virtual void visit(mir::VertexSetType::Ptr vertexset_type);
         virtual void visit(mir::ListType::Ptr list_type);
         
         virtual void visit(mir::StructTypeDecl::Ptr struct_type); */
         virtual void visit(mir::ScalarType::Ptr scalar_type);
         virtual void visit(mir::VectorType::Ptr vector_type);
         
         virtual void visit(mir::EdgeSetType::Ptr edgeset_type);
          
    private:
        MIRContext * mir_context_;
        std::ostream &oss;
        unsigned indentLevel;
        void indent() { ++indentLevel; }
        void dedent() { --indentLevel; }
        void printIndent() { oss << std::string(2 * indentLevel, ' '); }
        void printBeginIndent() { oss << std::string(2 * indentLevel, ' ') << "{" << std::endl; }
        void printEndIndent() { oss << std::string(2 * indentLevel, ' ') << "}"; }
        
        enum mir::FuncDecl::function_context current_context;
        
        void genIncludeStmts(void);
        
        //void genPropertyArrayImplementationWithInitialization(mir::VarDecl::Ptr shared_ptr);
        
        //void genElementData();
        
        void genEdgeSets();
        
        //void genStructTypeDecls();
        
        //void genEdgesetApplyFunctionCall(mir::EdgeSetApplyExpr::Ptr apply);
        
        void genPropertyArrayDecl(mir::VarDecl::Ptr shared_ptr);
        
        //void genPropertyArrayAlloc(mir::VarDecl::Ptr shared_ptr);
        
        void genScalarDecl(mir::VarDecl::Ptr var_decl);
        
        //void genScalarAlloc(mir::VarDecl::Ptr shared_ptr);
    };
}

#endif

