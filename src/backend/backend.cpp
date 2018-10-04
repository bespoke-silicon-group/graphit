//
// Created by Yunming Zhang on 2/14/17.
//

#include <graphit/backend/backend.h>
#include <graphit/backend/codegen_cpp/codegen_cpp.h>
#include <graphit/backend/codegen_gunrock/codegen_gunrock.h>
#include <graphit/backend/codegen_hb/codegen_hb.h>
#include <fstream>

namespace graphit{
<<<<<<< HEAD
    int Backend::emitCPP(std::ostream &oss, std::string module_name) {
        CodeGenCPP* codegen_cpp = new CodeGenCPP(oss, mir_context_, module_name);
        int flag = codegen_cpp->genCPP();
        delete codegen_cpp;
        return flag;
    }
    int Backend::emitPython(std::ostream &oss, std::string module_name, std::string module_path) {
	CodeGenPython *codegen_python = new CodeGenPython(oss, mir_context_, module_name, module_path);
	int flag = codegen_python->genPython();
	delete codegen_python;
	return flag;
    }
    int Backend::emitGPU(std::ostream &oss, std::string module_name, std::string module_path) {
        CodeGenGPU code_gen_gpu(oss, mir_context_, module_name, module_path);
	return code_gen_gpu.genGPU();
    }
=======
int Backend::emit(std::ostream &oss, std::ostream &oss_device) {
		int flag;
		CodeGenCPP *codegen_cpp;
		CodeGenGunrock *codegen_gunrock;
        CodeGenHB *codegen_hb;

	switch(mir_context_->backend_selection) {
		case BACKEND_CPP:
			codegen_cpp = new CodeGenCPP(oss, mir_context_);
			flag = codegen_cpp->genCPP();
			delete codegen_cpp;
			return flag;
			break;
		case BACKEND_GPU:
			codegen_gunrock = new CodeGenGunrock(oss, mir_context_);
			flag = codegen_gunrock->genGunrockCode();
			delete codegen_cpp;
			return flag;
			break;
    case BACKEND_HB:
            std::cerr << "Hammerblade backend not yet fully implemented\n";
            codegen_hb = new CodeGenHB(oss, oss_device, mir_context_);
            flag = codegen_hb->genHBCode();
            delete codegen_hb;
            return flag;
            break;
		default:
			std::cerr << "Invalid backend chosen, failing with error\n";
			return -1;
			break;
	}
}
>>>>>>> 6fd63244... Squashing all commits of hb-backend and rebasing on common ancestor commit with upstream-master
}
