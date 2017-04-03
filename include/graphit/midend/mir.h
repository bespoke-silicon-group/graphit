//
// Created by Yunming Zhang on 2/8/17.
//

#ifndef GRAPHIT_MIR_H
#define GRAPHIT_MIR_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <unordered_set>
#include <graphit/midend/mir_visitor.h>


namespace graphit {
    namespace mir {

        struct MIRNode;

        template <typename T>
        inline bool isa(std::shared_ptr<MIRNode> ptr) {
            return (bool)std::dynamic_pointer_cast<T>(ptr);
        }

        template <typename T>
        inline const std::shared_ptr<T> to(std::shared_ptr<MIRNode> ptr) {
            std::shared_ptr<T> ret = std::dynamic_pointer_cast<T>(ptr);
            return ret;
        }

        struct MIRNode : public std::enable_shared_from_this<MIRNode>{
            typedef std::shared_ptr<MIRNode> Ptr;
            MIRNode() {}
            /** We use the visitor pattern to traverse MIR nodes throughout the
            * compiler, so we have a virtual accept method which accepts
            * visitors.
            */

            virtual void accept(MIRVisitor *) = 0;

            friend std::ostream &operator<<(std::ostream &, MIRNode &);

        protected:
            template <typename T = MIRNode> std::shared_ptr<T> self() {
                return to<T>(shared_from_this());
            }
        };

        struct Expr : public MIRNode {
            typedef std::shared_ptr<Expr> Ptr;
            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<Expr>());
            }
        };

        struct Stmt : public MIRNode {
            typedef std::shared_ptr<Stmt> Ptr;
            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<Stmt>());
            }
        };

        struct Type : public MIRNode {
            typedef std::shared_ptr<Type> Ptr;
        };

        struct VarDecl : public Stmt {
            std::string modifier;
            std::string name;
            Type::Ptr       type;
            Expr::Ptr       initVal;

            typedef std::shared_ptr<VarDecl> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<VarDecl>());
            }
        };
        

        struct IntLiteral : public Expr {
            typedef std::shared_ptr<IntLiteral> Ptr;
            int val = 0;
            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<IntLiteral>());
            }
        };

        struct BinaryExpr : public Expr {
            Expr::Ptr lhs, rhs;
            typedef std::shared_ptr<BinaryExpr> Ptr;
            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<BinaryExpr>());
            }
        };

        struct AddExpr : public BinaryExpr {
            typedef std::shared_ptr<AddExpr> Ptr;
            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<AddExpr>());
            }
        };

        struct SubExpr : public BinaryExpr {
            typedef std::shared_ptr<SubExpr> Ptr;
            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<SubExpr>());
            }
        };

        struct ScalarType : public Type {
            enum class Type {INT, FLOAT, BOOL, COMPLEX, STRING};
            Type type;
            typedef std::shared_ptr<ScalarType> Ptr;
            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<ScalarType>());
            }
        };

    }

}

#endif //GRAPHIT_MIR_H