#include <graphit/frontend/scanner.h>
#include <graphit/midend/midend.h>
#include <fstream>
#include <graphit/frontend/frontend.h>

#include <graphit/utils/command_line.h>
#include <graphit/backend/backend.h>
#include <graphit/frontend/error.h>
#include <fstream>
#include <graphit/frontend/high_level_schedule.h>

#include <graphit/midend/mir_printer.h>
#include <string>
#include <iostream>
#include <sstream>

using namespace graphit;


namespace graphit {
	extern void user_defined_schedule (fir::high_level_schedule::ProgramScheduleNode::Ptr program);
}

int main(int argc, char* argv[]) {
    // Set up various data structures
    CLBase cli(argc, argv, "graphit compiler");
    graphit::FIRContext* context = new graphit::FIRContext();
    std::vector<ParseError> * errors = new std::vector<ParseError>();
    Frontend * fe = new Frontend();
    graphit::MIRContext* mir_context  = new graphit::MIRContext();

    //parse the arguments
    if (!cli.ParseArgs())
        return -1;
    
    //TODO(Emily): this should ideally be a compiler flag
    //need to figure out the integration of this file with graphitc.py
    bool verbose = true;
    if(cli.verbose_filename() == "")
    {
        std::cout << "a verbose file was not provided \n";
        verbose = false;
    }
    //read input file into buffer
    std::ifstream file(cli.input_filename());
    std::stringstream buffer;
    if(!file) {
        std::cout << "error reading the input file" << std::endl;
    }
    buffer << file.rdbuf();
    file.close();

    //set up the output file
    std::ofstream output_file;
    output_file.open(cli.output_filename());

    //compile the input file
    fe->parseStream(buffer, context, errors);

    fir::high_level_schedule::ProgramScheduleNode::Ptr program
            = std::make_shared<fir::high_level_schedule::ProgramScheduleNode>(context);

#ifndef USE_DEFAULT_SCHEDULE    
    //Call the user provided schedule for the algorithm
    user_defined_schedule(program);
#endif

    graphit::Midend* me = new graphit::Midend(context, program->getSchedule());
    me->emitMIR(mir_context);
    
    //NOTE(Emily): adding in printer here
    if(verbose)
    {
        std::string mir_print_file = cli.verbose_filename();
        
        std::filebuf fb;
        fb.open(mir_print_file, std::ios::out);
        
        std::ostream os(&fb);
        os << "testing MIR printer here: \n";
        
        //NOTE(Emily): get functions here
        std::vector<mir::FuncDecl::Ptr> functions = mir_context->getFunctionList();
        
        //NOTE(Emily): initialize printer here
        graphit::mir::MIRPrinter printer(os);
        
        //NOTE(Emily): print AST for each function here
        for (auto it = functions.begin(); it != functions.end(); it++) {
            printer.printMIR(it->get());
        }
        
        fb.close();
    }
    
    graphit::Backend* be = new graphit::Backend(mir_context);
    be->emitCPP(output_file);

    output_file.close();

    return 0;

}

