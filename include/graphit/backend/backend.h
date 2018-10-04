//
// Created by Yunming Zhang on 2/14/17.
//

#ifndef GRAPHIT_BACKEND_H
#define GRAPHIT_BACKEND_H

#include <graphit/midend/mir_context.h>
<<<<<<< HEAD
#include <graphit/backend/codegen_cpp.h>
#include <graphit/backend/codegen_python.h>
#include <graphit/backend/codegen_gpu/codegen_gpu.h>
=======
//#include <graphit/backend/codegen_cpp/codegen_cpp.h>
>>>>>>> 6fd63244... Squashing all commits of hb-backend and rebasing on common ancestor commit with upstream-master

namespace graphit {
    class Backend {
    public:
        Backend(MIRContext* mir_context) : mir_context_(mir_context){

        }

<<<<<<< HEAD
        int emitCPP(std::ostream &oss = std::cout, std::string module_name="");
    	int emitPython(std::ostream &oss = std::cout, std::string module_name="", std::string module_path="");
	    int emitGPU(std::ostream &oss = std::cout, std::string module_name="", std::string module_path="");
=======
        int emit(std::ostream &oss = std::cout, std::ostream &oss_device = std::cout);
>>>>>>> 6fd63244... Squashing all commits of hb-backend and rebasing on common ancestor commit with upstream-master

    private:
        MIRContext* mir_context_;
    };
}
#endif //GRAPHIT_BACKEND_H
