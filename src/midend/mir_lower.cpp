//
// Created by Yunming Zhang on 5/9/17.
//

#include <graphit/midend/mir_lower.h>
#include <graphit/midend/physical_data_layout_lower.h>
#include <graphit/midend/apply_expr_lower.h>
#include <graphit/midend/vector_op_lower.h>

namespace graphit {
    /**
     * Perfomrms the lowering passes on MIR_Context
     * @param mir_context
     * @param schedule
     */
    void MIRLower::lower(MIRContext* mir_context, Schedule* schedule){
        VectorOpLower(mir_context).lower();
        // This pass lowers apply expression into concrete versions, including push, pull hybrid and more
        ApplyExprLower(mir_context, schedule).lower();
        // The pass on lowering abstract data structures to
        // concrete data structures with physical layout information
        PhysicalDataLayoutLower(mir_context, schedule).lower();
    }
}

