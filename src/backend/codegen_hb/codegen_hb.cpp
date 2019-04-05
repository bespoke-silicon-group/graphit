
#include <graphit/backend/codegen_hb/codegen_hb.h>

namespace graphit {
    int CodeGenHB::genHBCode() {
        genIncludeStmts();
        genEdgeSets();
        //genElementData();
        //TODO(Emily): need to implement the visit patterns to call Struct Type Decls
        //genStructTypeDecls();
        
        //TODO(Emily): need to implement this pass through the AST to generate code
        /*
        //Processing the constants, generting declartions
        for (auto constant : mir_context_->getLoweredConstants()) {
            if ((std::dynamic_pointer_cast<mir::VectorType>(constant->type)) != nullptr) {
                mir::VectorType::Ptr type = std::dynamic_pointer_cast<mir::VectorType>(constant->type);
                // if the constant decl is a field property of an element (system vector)
                if (type->element_type != nullptr) {
                    //genPropertyArrayImplementationWithInitialization(constant);
                    //NOTE: here we only generate the declaration, not the allocation and initialization
                    // even through we have all the information.
                    // This is because we want to do the allocation and initialization steps in the main function,
                    // when we are using command line arguments and variables. This also allows flexibility of array of structs
                    // and struct of arrays.
                    // To support this feature, we have specialized the code generation of main function (see func_decl visit method).
                    // We first generate allocation, and then initialization (init_stmts) for global variables.
                    genPropertyArrayDecl(constant);
                }
            } else if (std::dynamic_pointer_cast<mir::VertexSetType>(constant->type)) {
                // if the constant is a vertex set  decl
                // currently, no code is generated
            } else {
                // regular constant declaration
                //constant->accept(this);
                genScalarDecl(constant);
            }
        }
        
        // Generate global declarations for socket-local buffers used by NUMA optimization
        for (auto iter : mir_context_->edgeset_to_label_to_merge_reduce) {
            for (auto inner_iter : iter.second) {
                if (inner_iter.second->numa_aware) {
                    inner_iter.second->scalar_type->accept(this);
                    oss << " **local_" << inner_iter.second->field_name << ";" << std::endl;
                }
            }
        }
        
        //Generates function declarations for various edgeset apply operations with different schedules
        // TODO: actually complete the generation, fow now we will use libraries to test a few schedules
        auto gen_edge_apply_function_visitor = EdgesetApplyFunctionDeclGenerator(mir_context_, oss);
        gen_edge_apply_function_visitor.genEdgeApplyFuncDecls();
        
        //Processing the functions
        std::map<std::string, mir::FuncDecl::Ptr>::iterator it;
        std::vector<mir::FuncDecl::Ptr> functions = mir_context_->getFunctionList();
        
        for (auto it = functions.begin(); it != functions.end(); it++) {
            it->get()->accept(this);
        }
        */
        oss << std::endl;
        return 0;
    };
    
    void CodeGenHB::genIncludeStmts() {
        oss << "#include <string.h> " << std::endl;
        oss << "#include \"bsg_manycore.h\"" << std::endl;
        oss << "#include \"bsg_set_tile_x_y.h\"" << std::endl;
        oss << "#include \"hammerblade-grt/printing.h\"" << std::endl;
        oss << "#include \"hammerblade-grt/algraph.h\"" << std::endl;
        oss << "#include \"hammerblade-grt/csrgraph.h\"" << std::endl;
        oss << "#include \"hammerblade-grt/array_size.h\"" << std::endl;
        oss << "#include \"hammerblade-grt/swap.h\"" << std::endl;
        oss << "#include \"hammerblade-grt/syscheck.h\"" << std::endl;
        oss << "#include \"hammerblade-grt/core_id.h\"" << std::endl;
        oss << "#include \"reference/serial_breadth_first_search.h\"" << std::endl;
        
    }
    
    void CodeGenHB::genEdgeSets() {
        for (auto edgeset : mir_context_->getEdgeSets()) {
            
            auto edge_set_type = mir::to<mir::EdgeSetType>(edgeset->type);
            if (edge_set_type->weight_type != nullptr) {
                //weighted edgeset
                //unweighted edgeset
                oss << "WGraph " << edgeset->name << ";" << std::endl;
            } else {
                //unweighted edgeset
                oss << "Graph " << edgeset->name << "; " << std::endl;
            }
        }
    }
    
    
    /**
     * Generate the struct types before the arrays are generated
     */
    /*void CodeGenHB::genStructTypeDecls() {
        for (auto const &struct_type_decl_entry : mir_context_->struct_type_decls) {
            auto struct_type_decl = struct_type_decl_entry.second;
            oss << "typedef struct ";
            oss << struct_type_decl->name << " { " << std::endl;
            
            for (auto var_decl : struct_type_decl->fields) {
                indent();
                printIndent();
                var_decl->type->accept(this);
                //we don't initialize in the struct declarations anymore
                // the initializations are done in the main function
                oss << var_decl->name;
                // << " = ";
                //var_decl->initVal->accept(this);
                oss << ";" << std::endl;
                dedent();
            }
            oss << "} " << struct_type_decl->name << ";" << std::endl;
        }
    }*/
    
}

