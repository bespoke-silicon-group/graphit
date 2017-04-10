//
// Created by Yunming Zhang on 1/20/17.
//

#include <gtest.h>
#include <graphit/frontend/frontend.h>
#include <graphit/midend/mir_context.h>
#include <graphit/frontend/error.h>

using namespace std;
using namespace graphit;


class FrontendTest : public ::testing::Test {
protected:
    virtual void SetUp(){
        context_ = new graphit::FIRContext();
        errors_ = new std::vector<ParseError>();
        fe_ = new Frontend();
    }

    virtual void TearDown() {
        // Code here will be called immediately after each test (right
        // before the destructor).


    }

    bool basicTest(std::istream & is){
        fe_->parseStream(is, context_, errors_);
        bool output =  fe_->parseStream(is, context_, errors_);
        //prints out the FIR, just a hack for now
        //TODO: fix the bug with unable to print FIR outside of the function
//        std::cout << "fir: " << std::endl;
//        std::cout << *context_->getProgram();
//        std::cout << std::endl;
        return output;
    }

    std::vector<ParseError> * errors_;
    graphit::FIRContext* context_;
    Frontend * fe_;
};




//tests front end
TEST_F(FrontendTest, SimpleVarDecl ) {
    istringstream is("const a : int = 3 + 4;");
    EXPECT_EQ (0,  basicTest(is));
}


TEST_F(FrontendTest, SimpleFunctionDecl ) {
    istringstream is("func add(a : int, b: int) -> c : int  end");
    EXPECT_EQ (0,  basicTest(is));
}


TEST_F(FrontendTest, SimpleFunctionDeclWithNoReturn ) {
    istringstream is("func add(a : int, b: int) end");
    EXPECT_EQ (0,  basicTest(is));
}

TEST_F(FrontendTest, SimpleFunctionDecFail) {
    istringstream is("func add(a : int, b: int) ");
    EXPECT_EQ (1,  basicTest(is));
}

TEST_F(FrontendTest, SimpleFunctionDecFailNoEnd) {
    istringstream is("func add(a : int, b: int) -> c : int ");
    EXPECT_EQ (1,  basicTest(is));
}

TEST_F(FrontendTest, SimpleFunctionWithVarDecl) {
    istringstream is("func add(a : int, b: int) -> c : int var d : int = 3; end");
    EXPECT_EQ (0,  basicTest(is));
}

TEST_F(FrontendTest, SimpleFunctionWithAdd) {
    istringstream is("func add(a : int, b: int) -> c : int c = a + b; end");
    EXPECT_EQ (0,  basicTest(is));
}

TEST_F(FrontendTest, MainFunctionWithPrint) {
    istringstream is("func main() print 4; end");
    EXPECT_EQ (0,  basicTest(is));
}