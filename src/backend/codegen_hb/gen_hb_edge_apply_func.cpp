#include <graphit/backend/codegen_hb/gen_hb_edge_apply_func.h>

namespace graphit {
    using namespace std;
    
    void HBEdgesetApplyFunctionGenerator::visit(mir::PushEdgeSetApplyExpr::Ptr push_apply) {
        genEdgeApplyFunctionDeclaration(push_apply);
    }

    void HBEdgesetApplyFunctionGenerator::genEdgeApplyFunctionDeclaration(mir::EdgeSetApplyExpr::Ptr apply) {
        auto func_name = genFunctionName(apply);
        
        // these schedules are still supported by runtime libraries
        if (func_name == "edgeset_apply_push_parallel_sliding_queue_from_vertexset_with_frontier"
            || func_name == "edgeset_apply_push_parallel_sliding_queue_weighted_deduplicatied_from_vertexset_with_frontier"){
            return;
        }
        
        
        genEdgeApplyFunctionSignature(apply);
        oss_ << "{ " << endl; //the end of the function declaration
        genEdgeApplyFunctionDeclBody(apply);
        oss_ << "} //end of edgeset apply function " << endl; //the end of the function declaration
        
    }

    void HBEdgesetApplyFunctionGenerator::genEdgeApplyFunctionDeclBody(mir::EdgeSetApplyExpr::Ptr apply) {
        //TODO(Emily): implement other directions
        /*if (mir::isa<mir::PullEdgeSetApplyExpr>(apply)) {
            genEdgePullApplyFunctionDeclBody(apply);
        }*/
        
        if (mir::isa<mir::PushEdgeSetApplyExpr>(apply)) {
            genEdgePushApplyFunctionDeclBody(apply);
        }
        /*
        if (mir::isa<mir::HybridDenseEdgeSetApplyExpr>(apply)) {
            genEdgeHybridDenseApplyFunctionDeclBody(apply);
        }
        
        if (mir::isa<mir::HybridDenseForwardEdgeSetApplyExpr>(apply)) {
            genEdgeHybridDenseForwardApplyFunctionDeclBody(apply);
        }
         */
    }

    void HBEdgesetApplyFunctionGenerator::setupFlags(mir::EdgeSetApplyExpr::Ptr apply,
                                                       bool & apply_expr_gen_frontier,
                                                       bool &from_vertexset_specified,
                                                       std::string &dst_type) {
        
        // set up the flag for checking if a from_vertexset has been specified
        if (apply->from_func != "")
            if (!mir_context_->isFunction(apply->from_func))
                from_vertexset_specified = true;
        
        // Check if the apply function has a return value
        auto apply_func = mir_context_->getFunction(apply->input_function_name);
        dst_type = apply->is_weighted ? "d.v" : "d";
        
        if (apply_func->result.isInitialized()) {
            // build an empty vertex subset if apply function returns
            apply_expr_gen_frontier = true;
        }
    }

    // Set up the global variables numVertices, numEdges, outdegrees
    void HBEdgesetApplyFunctionGenerator::setupGlobalVariables(mir::EdgeSetApplyExpr::Ptr apply,
                                                                 bool apply_expr_gen_frontier,
                                                                 bool from_vertexset_specified) {
        oss_ << "    int64_t numVertices = g.num_nodes(), numEdges = g.num_edges();\n";
        
        
        if (!mir::isa<mir::PullEdgeSetApplyExpr>(apply)) {
            
            if (mir::isa<mir::PushEdgeSetApplyExpr>(apply)){
                //we still need to convert the from_vertexset to sparse, and compute m for SparsePush
                // even when it does not return a frontier
                if (from_vertexset_specified) {
                    //TODO(Emily): we will need to implement our own toSparse() (or remove this)
                    oss_ << "    from_vertexset->toSparse();" << std::endl;
                    oss_ << "    long m = from_vertexset->size();\n";
                    
                } else {
                    oss_ << "    long m = numVertices; \n";
                }
            }
        }
    }

    // Print the code for traversing the edges in the push direction and return the new frontier
    // the apply_func_name is used for hybrid schedule, when a special push_apply_func is used
    // usually, the apply_func_name is fixed to "apply_func" (see the default argument)
    void HBEdgesetApplyFunctionGenerator::printPushEdgeTraversalReturnFrontier(
                                                                                 mir::EdgeSetApplyExpr::Ptr apply,
                                                                                 bool from_vertexset_specified,
                                                                                 bool apply_expr_gen_frontier,
                                                                                 std::string dst_type,
                                                                                 std::string apply_func_name) {
        
        
        
        //set up logic fo enabling deduplication with CAS on flags (only if it returns a frontier)
        if (apply->enable_deduplication && apply_expr_gen_frontier) {
            oss_ << "    if (g.flags_ == nullptr){\n"
            "      g.flags_ = new int[numVertices]();\n"
            "      parallel_for(int i = 0; i < numVertices; i++) g.flags_[i]=0;\n"
            "    }\n";
        }
        
        // If apply function has a return value, then we need to return a temporary vertexsubset
        if (apply_expr_gen_frontier) {
            // build an empty vertex subset if apply function returns
            //set up code for outputing frontier for push based edgeset apply operations
            //TODO(Emily): need to update this code here to use our data types
            oss_ <<
            "    VertexSubset<NodeID> *next_frontier = new VertexSubset<NodeID>(g.num_nodes(), 0);\n"
            "    if (numVertices != from_vertexset->getVerticesRange()) {\n"
            "        cout << \"edgeMap: Sizes Don't match\" << endl;\n"
            "        abort();\n"
            "    }\n"
            "    if (outDegrees == 0) return next_frontier;\n"
            "    uintT *offsets = degrees;\n"
            "    long outEdgeCount = sequence::plusScan(offsets, degrees, m);\n"
            "    uintE *outEdges = newA(uintE, outEdgeCount);\n";
        }
        
        
        indent();
        
        printIndent();
        
        //TODO(Emily): need to use our parallel blocked macro here
        std::string for_type = "for";
        if (apply->is_parallel)
            for_type = "parallel_for";
        
        std::string node_id_type = "NodeID";
        if (apply->is_weighted) node_id_type = "WNode";
        
        
        if (from_vertexset_specified)
            oss_ << for_type << " (long i=0; i < m; i++) {" << std::endl;
        else
            oss_ << for_type << " (NodeID s=0; s < g.num_nodes(); s++) {" << std::endl;
        
        indent();
        
        if (from_vertexset_specified){
            oss_ << "    NodeID s = from_vertexset->dense_vertex_set_[i];\n"
            "    int j = 0;\n";
            if (apply_expr_gen_frontier){
                oss_ <<  "    uintT offset = offsets[i];\n";
            }
        }
        
        
        if (apply->from_func != "" && !from_vertexset_specified) {
            printIndent();
            oss_ << "if (from_func(s)){ " << std::endl;
            indent();
        }
        
        printIndent();
        
        oss_ << "for(" << node_id_type << " d : g.out_neigh(s)){" << std::endl;
        
        
        // print the checks on filtering on sources s
        if (apply->to_func != "") {
            indent();
            printIndent();
            
            oss_ << "if";
            //TODO: move this logic in to MIR at some point
            if (mir_context_->isFunction(apply->to_func)) {
                //if the input expression is a function call
                oss_ << " (to_func(" << dst_type << ")";
                
            } else {
                //the input expression is a vertex subset
                oss_ << " (to_vertexset->bool_map_[s] ";
            }
            oss_ << ") { " << std::endl;
        }
        
        indent();
        printIndent();
        if (apply_expr_gen_frontier) {
            oss_ << "if( ";
        }
        
        // generating the C++ code for the apply function call
        if (apply->is_weighted) {
            oss_ << apply_func_name << " ( s , d.v, d.w )";
        } else {
            oss_ << apply_func_name << " ( s , d  )";
            
        }
        
        if (!apply_expr_gen_frontier) {
            oss_ << ";" << std::endl;
            
        } else {
            
            
            //need to return a frontier
            if (apply->enable_deduplication && apply_expr_gen_frontier) {
                oss_ << " && CAS(&(g.flags_[" << dst_type << "]), 0, 1) ";
            }
            
            indent();
            //generate the code for adding destination to "next" frontier
            oss_ << " ) { " << std::endl;
            printIndent();
            oss_ << "outEdges[offset + j] = " << dst_type << "; " << std::endl;
            dedent();
            printIndent();
            oss_ << "} else { outEdges[offset + j] = UINT_E_MAX; }" << std::endl;
            
        }
        
        
        
        // end of from filtering
        if (apply->to_func != "") {
            dedent();
            printIndent();
            oss_ << "} //end of to func" << std::endl;
            
            if (apply_expr_gen_frontier){
                printIndent();
                oss_ << " else { outEdges[offset + j] = UINT_E_MAX;  }" << std::endl;
            }
            
        }
        
        //increment the index for each source vertex
        if (apply_expr_gen_frontier){
            printIndent();
            oss_ << "j++;" << std::endl;
        }
        
        //end of for loop on the neighbors
        dedent();
        printIndent();
        oss_ << "} //end of for loop on neighbors" << std::endl;
        
        if (apply->from_func != "" && !from_vertexset_specified) {
            dedent();
            printIndent();
            oss_ << "} //end of from func " << std::endl;
        }
        
        
        dedent();
        printIndent();
        oss_ << "}" << std::endl;
        
        
        
        
        //return a new vertexset if no subset vertexset is returned
        if (apply_expr_gen_frontier) {
            oss_ << "  uintE *nextIndices = newA(uintE, outEdgeCount);\n"
            "  long nextM = sequence::filter(outEdges, nextIndices, outEdgeCount, nonMaxF());\n"
            "  free(outEdges);\n"
            "  free(degrees);\n"
            "  next_frontier->num_vertices_ = nextM;\n"
            "  next_frontier->dense_vertex_set_ = nextIndices;\n";
            
            //set up logic fo enabling deduplication with CAS on flags (only if it returns a frontier)
            if (apply->enable_deduplication && from_vertexset_specified) {
                //clear up the indices that are set
                oss_ << "  parallel_for(int i = 0; i < nextM; i++){\n"
                "     g.flags_[nextIndices[i]] = 0;\n"
                "  }\n";
            }
            oss_ << "  return next_frontier;\n";
        }
    }


    // Iterate through per-socket local buffers and merge the result into the global buffer
    //NOTE(Emily): we will probably want to leverage code like this for our own memory system
    //For now we shouldn't ever be using this
    void HBEdgesetApplyFunctionGenerator::printNumaMerge(mir::EdgeSetApplyExpr::Ptr apply) {
        oss_ << "}// end of per-socket parallel region\n\n";
        auto edgeset_name = mir::to<mir::VarExpr>(apply->target)->var.getName();
        auto merge_reduce = mir_context_->edgeset_to_label_to_merge_reduce[edgeset_name][apply->scope_label_name];
        oss_ << "  parallel_for (int n = 0; n < numVertices; n++) {\n";
        oss_ << "    for (int socketId = 0; socketId < omp_get_num_places(); socketId++) {\n";
        oss_ << "      " << apply->merge_reduce->field_name << "[n] ";
        switch (apply->merge_reduce->reduce_op) {
            case mir::ReduceStmt::ReductionOp::SUM:
                oss_ << "+= local_" << apply->merge_reduce->field_name  << "[socketId][n];\n";
                break;
            case mir::ReduceStmt::ReductionOp::MIN:
                oss_ << "= min(" << apply->merge_reduce->field_name << "[n], local_"
                << apply->merge_reduce->field_name  << "[socketId][n]);\n";
                break;
            default:
                // TODO: fill in the missing operators when they are actually used
                abort();
        }
        oss_ << "    }\n  }" << std::endl;
    }

    //NOTE(Emily): also probably useful for distributing data on our machine
    void HBEdgesetApplyFunctionGenerator::printNumaScatter(mir::EdgeSetApplyExpr::Ptr apply) {
        oss_ << "parallel_for (int n = 0; n < numVertices; n++) {\n";
        oss_ << "    for (int socketId = 0; socketId < omp_get_num_places(); socketId++) {\n";
        oss_ << "      local_" << apply->merge_reduce->field_name  << "[socketId][n] = "
        << apply->merge_reduce->field_name << "[n];\n";
        oss_ << "    }\n  }\n";
    }


    // Generate the code for pushed based program
    void HBEdgesetApplyFunctionGenerator::genEdgePushApplyFunctionDeclBody(mir::EdgeSetApplyExpr::Ptr apply) {
        bool apply_expr_gen_frontier = false;
        bool from_vertexset_specified = false;
        string dst_type;
        setupFlags(apply, apply_expr_gen_frontier, from_vertexset_specified, dst_type);
        setupGlobalVariables(apply, apply_expr_gen_frontier, from_vertexset_specified);
        printPushEdgeTraversalReturnFrontier(apply, from_vertexset_specified, apply_expr_gen_frontier, dst_type);
    }

    void HBEdgesetApplyFunctionGenerator::genEdgeApplyFunctionSignature(mir::EdgeSetApplyExpr::Ptr apply) {
        auto func_name = genFunctionName(apply);
        
        auto mir_var = std::dynamic_pointer_cast<mir::VarExpr>(apply->target);
        vector<string> templates = vector<string>();
        vector<string> arguments = vector<string>();
        
        //TODO(Emily): are we going to use their Graph class or will we need our own
        if (apply->is_weighted) {
            arguments.push_back("WGraph & g");
        } else {
            arguments.push_back("Graph & g");
        }
        
        if (apply->from_func != "") {
            if (mir_context_->isFunction(apply->from_func)) {
                // the schedule is an input from function
                templates.push_back("typename FROM_FUNC");
                arguments.push_back("FROM_FUNC from_func");
            } else {
                // the input is an input from vertexset
                arguments.push_back("VertexSubset<NodeID>* from_vertexset");
            }
        }
        
        if (apply->to_func != "") {
            if (mir_context_->isFunction(apply->to_func)) {
                // the schedule is an input to function
                templates.push_back("typename TO_FUNC");
                arguments.push_back("TO_FUNC to_func");
            } else {
                // the input is an input to vertexset
                arguments.push_back("VertexSubset<NodeID>* to_vertexset");
            }
        }
        
        
        templates.push_back("typename APPLY_FUNC");
        arguments.push_back("APPLY_FUNC apply_func");
        
        oss_ << "template <";
        
        bool first = true;
        for (auto temp : templates) {
            if (first) {
                oss_ << temp << " ";
                first = false;
            } else
                oss_ << ", " << temp;
        }
        oss_ << "> ";
        oss_ << (mir_context_->getFunction(apply->input_function_name)->result.isInitialized() ?
                 "VertexSubset<NodeID>* " : "void ")  << func_name << "(";
        
        first = true;
        for (auto arg : arguments) {
            if (first) {
                oss_ << arg << " ";
                first = false;
            } else
                oss_ << ", " << arg;
        }
        
        oss_ << ") " << endl;
        
        
    }

    //NOTE(Emily): we may want to somehow signify this is a parallel kernel in the name, but otherwise we don't need to change this
    //generates different function name for different schedules
    // important for cases where we split the kernel iterations and assign different schedules to different iters
    std::string HBEdgesetApplyFunctionGenerator::genFunctionName(mir::EdgeSetApplyExpr::Ptr apply) {
        // A total of 48 schedules for the edgeset apply operator for now
        // Direction first: "push", "pull" or "hybrid_dense"
        // Parallel: "parallel" or "serial"
        // Weighted: "" or "weighted"
        // Deduplicate: "deduplicated" or ""
        // From: "" (no from func specified) or "from_vertexset" or "from_filter_func"
        // To: "" or "to_vertexset" or "to_filter_func"
        // Frontier: "" (no frontier tracking) or "with_frontier"
        // Weighted: "" (unweighted) or "weighted"
        
        string output_name = "edgeset_apply";
        auto apply_func = mir_context_->getFunction(apply->input_function_name);
        
        //check direction
        if (mir::isa<mir::PushEdgeSetApplyExpr>(apply)) {
            output_name += "_push";
        } else if (mir::isa<mir::PullEdgeSetApplyExpr>(apply)) {
            output_name += "_pull";
        } else if (mir::isa<mir::HybridDenseForwardEdgeSetApplyExpr>(apply)) {
            output_name += "_hybrid_denseforward";
        } else if (mir::isa<mir::HybridDenseEdgeSetApplyExpr>(apply)) {
            output_name += "_hybrid_dense";
        }
        
        //check parallelism specification
        if (apply->is_parallel) {
            output_name += "_parallel";
        } else {
            output_name += "_serial";
        }
        
        if (apply->use_sliding_queue) {
            output_name += "_sliding_queue";
        }
        
        //check if it is weighted
        if (apply->is_weighted) {
            output_name += "_weighted";
        }
        
        // check for deduplication
        if (apply->enable_deduplication && apply_func->result.isInitialized()) {
            output_name += "_deduplicatied";
        }
        
        if (apply->from_func != "") {
            if (mir_context_->isFunction(apply->from_func)) {
                // the schedule is an input from function
                output_name += "_from_filter_func";
            } else {
                // the input is an input from vertexset
                output_name += "_from_vertexset";
            }
        }
        
        if (apply->to_func != "") {
            if (mir_context_->isFunction(apply->to_func)) {
                // the schedule is an input to function
                output_name += "_to_filter_func";
            } else {
                // the input is an input to vertexset
                output_name += "_to_vertexset";
            }
        }
        
        if (mir::isa<mir::HybridDenseEdgeSetApplyExpr>(apply)) {
            auto apply_expr = mir::to<mir::HybridDenseEdgeSetApplyExpr>(apply);
            if (apply_expr->push_to_function_ != "") {
                if (mir_context_->isFunction(apply->to_func)) {
                    // the schedule is an input to function
                    output_name += "_push_to_filter_func";
                } else {
                    // the input is an input to vertexset
                    output_name += "_push_to_vertexset";
                }
            }
        }
        
        
        if (apply_func->result.isInitialized()) {
            //if frontier tracking is enabled (when apply function returns a boolean value)
            output_name += "_with_frontier";
        }
        
        if (apply->use_pull_frontier_bitvector){
            output_name += "_pull_frontier_bitvector";
        }
        
        if (apply->use_pull_edge_based_load_balance){
            output_name += "_pull_edge_based_load_balance";
        }
        
        return output_name;
    }

}
