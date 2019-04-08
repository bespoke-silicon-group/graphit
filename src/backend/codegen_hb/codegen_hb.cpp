
#include <graphit/backend/codegen_hb/codegen_hb.h>

namespace graphit {
    int CodeGenHB::genHBCode() {
        genIncludeStmts();
        genEdgeSets();
        //genElementData();
        //TODO(Emily): need to implement the visit patterns to call Struct Type Decls
        //genStructTypeDecls();
        
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
        
        //TODO(Emily): need to implement this pass through the AST to generate code
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
        */
        //Processing the functions
        std::map<std::string, mir::FuncDecl::Ptr>::iterator it;
        std::vector<mir::FuncDecl::Ptr> functions = mir_context_->getFunctionList();
        
        for (auto it = functions.begin(); it != functions.end(); it++) {
            it->get()->accept(this);
        }
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
    
    void CodeGenHB::visit(mir::FuncDecl::Ptr func_decl) {
        // Generate function signature
        if (func_decl->name == "main") {
            func_decl->isFunctor = false;
            oss << "int " << func_decl->name << "(int argc, char * argv[])";
        } else {
            // Use functors for better compiler inlining
            func_decl->isFunctor = true;
            oss << "struct " << func_decl->name << std::endl;
            printBeginIndent();
            indent();
            oss << std::string(2 * indentLevel, ' ');
            
            if (func_decl->result.isInitialized()) {
                func_decl->result.getType()->accept(this);
                
                //insert an additional var_decl for returning result
                const auto var_decl = std::make_shared<mir::VarDecl>();
                var_decl->name = func_decl->result.getName();
                var_decl->type = func_decl->result.getType();
                if (func_decl->body->stmts == nullptr) {
                    func_decl->body->stmts = new std::vector<mir::Stmt::Ptr>();
                }
                auto it = func_decl->body->stmts->begin();
                func_decl->body->stmts->insert(it, var_decl);
            } else {
                oss << "void ";
            }
            
            oss << "operator() (";
            bool printDelimiter = false;
            for (auto arg : func_decl->args) {
                if (printDelimiter) {
                    oss << ", ";
                }
                
                arg.getType()->accept(this);
                oss << arg.getName();
                printDelimiter = true;
            }
            oss << ") ";
        }
        
        oss << std::endl;
        printBeginIndent();
        indent();
        
        //TODO(Emily): need to implement this to fit our needs for functors/functions
        /*
        
        if (func_decl->name == "main") {
            //generate special initialization code for main function
            //TODO: this is probably a hack that could be fixed for later
            
            //First, allocate the edgesets (read them from outside files if needed)
            for (auto stmt : mir_context_->edgeset_alloc_stmts) {
                stmt->accept(this);
            }
            
            // Initialize graphSegments if necessary
            auto segment_map = mir_context_->edgeset_to_label_to_num_segment;
            for (auto edge_iter = segment_map.begin(); edge_iter != segment_map.end(); edge_iter++) {
                auto edgeset = mir_context_->getConstEdgeSetByName((*edge_iter).first);
                auto edge_set_type = mir::to<mir::EdgeSetType>(edgeset->type);
                bool is_weighted = (edge_set_type->weight_type != nullptr);
                for (auto label_iter = (*edge_iter).second.begin();
                     label_iter != (*edge_iter).second.end(); label_iter++) {
                    auto edge_iter_first = (*edge_iter).first;
                    auto label_iter_first = (*label_iter).first;
                    auto label_iter_second = (*label_iter).second;
                    auto numa_aware_flag = mir_context_->edgeset_to_label_to_merge_reduce[edge_iter_first][label_iter_first]->numa_aware;
                    
                    if (label_iter_second < 0) {
                        //do a specical case for negative number of segments. I
                        // in the case of negative integer, we use the number as argument to runtimve argument argv
                        // this is the only place in the generated code that we set the number of segments
                        oss << "  " << edgeset->name << ".buildPullSegmentedGraphs(\"" << label_iter_first
                        << "\", " << "atoi(argv[" << -1*label_iter_second << "])"
                        << (numa_aware_flag ? ", true" : "") << ");" << std::endl;
                    } else {
                        // just use the positive integer as argument to number of segments
                        oss << "  " << edgeset->name << ".buildPullSegmentedGraphs(\"" << label_iter_first
                        << "\", " << label_iter_second
                        << (numa_aware_flag ? ", true" : "") << ");" << std::endl;
                    }
                }
            }
            
            //generate allocation statemetns for field vectors
            for (auto constant : mir_context_->getLoweredConstants()) {
                if ((std::dynamic_pointer_cast<mir::VectorType>(constant->type)) != nullptr) {
                    mir::VectorType::Ptr type = std::dynamic_pointer_cast<mir::VectorType>(constant->type);
                    // if the constant decl is a field property of an element (system vector)
                    if (type->element_type != nullptr) {
                        //genPropertyArrayImplementationWithInitialization(constant);
                        //genPropertyArrayDecl(constant);
                        if (constant->needs_allocation)
                            genPropertyArrayAlloc(constant);
                    }
                } else if (std::dynamic_pointer_cast<mir::VertexSetType>(constant->type)) {
                    // if the constant is a vertex set  decl
                    // currently, no code is generated
                } else {
                    // regular constant declaration
                    //constant->accept(this);
                    genScalarAlloc(constant);
                }
            }
            
            // the stmts that initializes the field vectors
            for (auto stmt : mir_context_->field_vector_init_stmts) {
                stmt->accept(this);
            }
            
            for (auto iter : mir_context_->edgeset_to_label_to_merge_reduce) {
                for (auto inner_iter : iter.second) {
                    
                    if ((inner_iter.second)->numa_aware) {
                        auto merge_reduce = inner_iter.second;
                        std::string local_field = "local_" + merge_reduce->field_name;
                        oss << "  " << local_field << " = new ";
                        merge_reduce->scalar_type->accept(this);
                        oss << "*[omp_get_num_places()];\n";
                        
                        oss << "  for (int socketId = 0; socketId < omp_get_num_places(); socketId++) {\n";
                        oss << "    " << local_field << "[socketId] = (";
                        merge_reduce->scalar_type->accept(this);
                        oss << "*)numa_alloc_onnode(sizeof(";
                        merge_reduce->scalar_type->accept(this);
                        oss << ") * ";
                        auto count_expr = mir_context_->getElementCount(
                                                                        mir_context_->getElementTypeFromVectorOrSetName(merge_reduce->field_name));
                        count_expr->accept(this);
                        oss << ", socketId);\n";
                        
                        oss << "    parallel_for (int n = 0; n < ";
                        count_expr->accept(this);
                        oss << "; n++) {\n";
                        oss << "      " << local_field << "[socketId][n] = " << merge_reduce->field_name << "[n];\n";
                        oss << "    }\n  }\n";
                        
                        oss << "  omp_set_nested(1);" << std::endl;
                    }
                }
            }
        }
        
        
        //if the function has a body
        if (func_decl->body->stmts) {
            
            
            func_decl->body->accept(this);
            
            //print a return statemetn if there is a result
            if (func_decl->result.isInitialized()) {
                printIndent();
                oss << "return " << func_decl->result.getName() << ";" << std::endl;
            }
            
            
        }
        
        if (func_decl->isFunctor) {
            dedent();
            printEndIndent();
            oss << ";";
            oss << std::endl;
        }
        
        if (func_decl->name == "main") {
            for (auto iter : mir_context_->edgeset_to_label_to_merge_reduce) {
                for (auto inner_iter : iter.second) {
                    if (inner_iter.second->numa_aware) {
                        auto merge_reduce = inner_iter.second;
                        oss << "  for (int socketId = 0; socketId < omp_get_num_places(); socketId++) {\n";
                        oss << "    numa_free(local_" << merge_reduce->field_name << "[socketId], sizeof(";
                        merge_reduce->scalar_type->accept(this);
                        oss << ") * ";
                        mir_context_->getElementCount(mir_context_->getElementTypeFromVectorOrSetName(merge_reduce->field_name))->accept(this);
                        oss << ");\n  }\n";
                    }
                }
            }
        }
        */
        dedent();
        printEndIndent();
        oss << ";";
        oss << std::endl;
         
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

