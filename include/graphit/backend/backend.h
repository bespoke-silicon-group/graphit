//
// Created by Yunming Zhang on 2/14/17.
//

#ifndef GRAPHIT_BACKEND_H
#define GRAPHIT_BACKEND_H

#include <graphit/midend/mir_context.h>
//#include <graphit/backend/codegen_cpp/codegen_cpp.h>

namespace graphit {
    class Backend {
    public:
        Backend(MIRContext* mir_context) : mir_context_(mir_context){

        }

        int emit(std::ostream &oss = std::cout, std::ostream &oss_device = std::cout);

    private:
        MIRContext* mir_context_;
    };
}
#endif //GRAPHIT_BACKEND_H
