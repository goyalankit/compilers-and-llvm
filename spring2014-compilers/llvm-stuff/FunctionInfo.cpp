// 15-745 S13 Assignment 1: FunctionInfo.cpp
// 
// Based on code from Todd C. Mowry
// Modified by Arthur Peters
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

using namespace llvm;

namespace {


class CommentWriter : public AssemblyAnnotationWriter {
public:
 virtual void emitFunctionAnnot(const Function *F,
                         formatted_raw_ostream &OS) {
    OS << "; [#uses=" << F->getNumUses() << ']';  // Output # uses
    OS << '\n';
  }	

};

class FunctionInfo : public FunctionPass, public AssemblyAnnotationWriter{

public:
  static char ID;
  FunctionInfo() : FunctionPass(ID) {}

 void emitFunctionAnnot(const Function *F, formatted_raw_ostream &OS) {
	errs() << "CALLED";
    OS << "; [#uses=" << F->getNumUses() << ']';  // Output # uses
    OS << '\n';
  }	


  virtual bool runOnFunction(Function &F) {
	for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I){
	  Instruction &II = *I;
//	printInfoComment((Value)&I, ferrs());
	  errs() << *I;
	  errs() << " source_line: " << lineNum(&II) << "\n";
//	  errs().write_escaped(itoa(lineNum(&II)));

	}
    return false;
  }


  unsigned lineNum(Instruction* I){
	if (MDNode *N = I->getMetadata("dbg")) {  // Here I is an LLVM instruction
		DILocation Loc(N);                      // DILocation is in DebugInfo.h
		unsigned Line = Loc.getLineNumber();
		return Line;
	}
}


  // We don't modify the program, so we preserve all analyses
  virtual void getAnalysisUsage(AnalysisUsage &AU) const {
//    AU.setPreservesAll();
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
