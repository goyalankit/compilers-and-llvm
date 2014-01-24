// 15-745 S13 Assignment 1: FunctionInfo.cpp
// 
// Based on code from Todd C. Mowry
// Modified by Arthur Peters
// Modified by Ankit Goyal
////////////////////////////////////////////////////////////////////////////////

#include "llvm/Pass.h"
#include "llvm/PassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/DebugInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/DebugInfo.h"
#include "llvm/Assembly/AssemblyAnnotationWriter.h"


#include <ostream>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>

using namespace llvm;

namespace {

    class FunctionInfo : public ModulePass{

        public:
            static char ID;
            FunctionInfo() : ModulePass(ID) {} //default constructor

            virtual bool runOnFunction(Function &F) {
                int arg_size = F.arg_size();
                int num_call_sites = F.getNumUses();
                int num_basic_blocks = F.size(); //defined in value class.
                //count the number of instructions.
                int number_of_instructions = 0;
                for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I)
                    number_of_instructions += 1;

                errs() << F.getName() <<": arguments=" << arg_size << " call sites=" <<  num_call_sites << " basic blocks=" << num_basic_blocks << " instructions=" << number_of_instructions << "\n";
                return false;
            }

            virtual bool runOnModule(Module &M){
                for (Module::iterator MI = M.begin(), ME = M.end(); MI != ME; ++MI)
                {
                    runOnFunction(*MI);
                }
                return false;
            }


    // We don't modify the program, so we preserve all analyses
    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
        AU.setPreservesAll();
    }
};

// LLVM uses the address of this static member to identify the pass, so the
// initialization value is unimportant.
char FunctionInfo::ID = 0;

// Register this pass to be used by language front ends.
// This allows this pass to be called using the command:
//    clang -c -Xclang -load -Xclang ./FunctionInfo.so loop.c
static void registerMyPass(const PassManagerBuilder &,
        PassManagerBase &PM) {
    PM.add(new FunctionInfo());
}
RegisterStandardPasses
RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
        registerMyPass);

// Register the pass name to allow it to be called with opt:
//    clang -c -emit-llvm loop.c
//    opt -load ./FunctionInfo.so -function-info loop.bc > /dev/null
// See http://llvm.org/releases/3.4/docs/WritingAnLLVMPass.html#running-a-pass-with-opt for more info.
RegisterPass<FunctionInfo> X("function-info", "Function Information");

}
