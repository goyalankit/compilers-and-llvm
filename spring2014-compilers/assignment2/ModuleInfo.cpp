// 15-745 S13 Assignment 1: FunctionInfo.cpp
// 
// Based on code from Todd C. Mowry
// Modified by Arthur Peters
// Modified by Ankit Goyal
//
////////////////////////////////////////////////////////////////////////////////

#include "llvm/Pass.h"
#include "llvm/PassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/DebugInfo.h"
#include "llvm/Assembly/AssemblyAnnotationWriter.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/IR/Constants.h"

#include <ostream>
#include <fstream>
#include <iostream>
#include <stdlib.h>

using namespace llvm;

namespace {

    class CommentWriter : public AssemblyAnnotationWriter {
        public:
            //Get the line number from metadata of instruction
            unsigned lineNum(const Instruction* I){
                if (MDNode *N = I->getMetadata("dbg")) {  // Here I is an LLVM instruction
                    DILocation Loc(N);                      // DILocation is in DebugInfo.h
                    unsigned Line = Loc.getLineNumber();
                    return Line;
                }
//                errs() << "Debug Option Not Turned On. Make sure you are using -g option to compile";
            }

            //override to print comment before the instruction
            virtual void emitInstructionAnnot(const Instruction *I, formatted_raw_ostream &OS) {
                //OS << "Source Line #: " << lineNum(I);
            }
            
            //override to print comment after the instruction
             virtual void printInfoComment(const Value &V, formatted_raw_ostream &OS) {
                if(const Instruction *I = dyn_cast<Instruction>(&V)){
                    OS << " ; source code line number: " << lineNum(I);
                }
                
            }
    };


    class ModuleInfo : public ModulePass {
        public:
            static char ID;
            ModuleInfo() : ModulePass(ID) {}
            CommentWriter aaw ;
            virtual bool runOnModule(Module &M) {
                M.print(errs(), &aaw);	
                return false;
            }

            // We don't modify the program, so we preserve all analyses
            virtual void getAnalysisUsage(AnalysisUsage &AU) const {
                AU.setPreservesAll();
            }
    };

    // LLVM uses the address of this static member to identify the pass, so the
    // initialization value is unimportant.
    char ModuleInfo::ID = 0;

    // Register this pass to be used by language front ends.
    // This allows this pass to be called using the command:
    //    clang -c -Xclang -load -Xclang ./FunctionInfo.so loop.c
    static void registerMyPass(const PassManagerBuilder &,
            PassManagerBase &PM) {
        PM.add(new ModuleInfo());
    }
    RegisterStandardPasses
        RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                registerMyPass);

    // Register the pass name to allow it to be called with opt:
    //    clang -c -g -emit-llvm loop.c
    //    opt -load ./ModuleInfo.so -module-info loop.bc > /dev/null
    // See http://llvm.org/releases/3.4/docs/WritingAnLLVMPass.html#running-a-pass-with-opt for more info.
    RegisterPass<ModuleInfo> X("module-info", "module Information");

}
