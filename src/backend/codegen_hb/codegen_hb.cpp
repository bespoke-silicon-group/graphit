
#include <graphit/backend/codegen_hb/codegen_hb.h>

namespace graphit {
    int CodeGenHB::genHBCode() {
        genIncludeStmts();
        genEdgeSets();
        //genElementData();
        //TODO(Emily): need to implement the visit patterns to call Struct Type Decls
        //genStructTypeDecls();
        
        //TODO(Emily): need to implement this pass through the AST to generate code
        
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
        /*
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
    }
    
    void CodeGenHB::visit(mir::VectorType::Ptr vector_type) {
        
    }
    
    void CodeGenHB::visit(mir::ScalarType::Ptr scalar_type) {
        switch (scalar_type->type) {
            case mir::ScalarType::Type::INT:
                oss << "int ";
                break;
            case mir::ScalarType::Type::FLOAT:
                oss << "float ";
                break;
            case mir::ScalarType::Type::DOUBLE:
                oss << "double ";
                break;
            case mir::ScalarType::Type::BOOL:
                oss << "bool ";
                break;
            case mir::ScalarType::Type::STRING:
                oss << "string ";
                break;
            default:
                break;
        }
    }
    
    void CodeGenHB::visit(mir::EdgeSetType::Ptr edgeset_type) {
        oss << " Graph ";
    }
    
    void CodeGenHB::visit(mir::ElementType::Ptr element_type) {
        //currently, we generate an index id into the vectors
        oss << "NodeID ";
    }
    
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
    
    void CodeGenHB::genPropertyArrayDecl(mir::VarDecl::Ptr var_decl) {
        // read the name of the array
        const auto name = var_decl->name;
        
        // read the type of the array
        mir::VectorType::Ptr vector_type = std::dynamic_pointer_cast<mir::VectorType>(var_decl->type);
        assert(vector_type != nullptr);
        auto vector_element_type = vector_type->vector_element_type;
        assert(vector_element_type != nullptr);
        
        /**  Deprecated, now we generate an array declaration, not a vector one
         //generate std::vector implementation
         oss << "std::vector< ";
         vector_element_type->accept(this);
         // pointer declaration
         oss << " >  ";
         oss << name;
         oss << ";" << std::endl;
         **/
        
        if (!mir::isa<mir::VectorType>(vector_element_type)){
            vector_element_type->accept(this);
            oss << " * __restrict " << name << ";" << std::endl;
        } else if (mir::isa<mir::VectorType>(vector_element_type)) {
            //if each element is a vector
            auto vector_vector_element_type = mir::to<mir::VectorType>(vector_element_type);
            assert(vector_vector_element_type->range_indexset != 0);
            int range = vector_vector_element_type->range_indexset;
            
            //first generates a typedef for the vector type
            oss << "typedef ";
            vector_vector_element_type->vector_element_type->accept(this);
            std::string typedef_name = "defined_type_" + mir_context_->getUniqueNameCounterString();
            oss << typedef_name <<  " ";
            oss << "[ " << range << "]; " << std::endl;
            vector_vector_element_type->typedef_name_ = typedef_name;
            
            //use the typedef defined type to declare a new pointer
            oss << typedef_name << " * __restrict  " << name << ";" << std::endl;
            
        } else {
            std::cout << "unsupported type for property: " << var_decl->name << std::endl;
            exit(0);
        }
    }
    
    void CodeGenHB::genScalarDecl(mir::VarDecl::Ptr var_decl){
        //the declaration and the value are separate. The value is generated as a separate assign statement in the main function
        var_decl->type->accept(this);
        oss << var_decl->name << "; " << std::endl;
    }
    
}

