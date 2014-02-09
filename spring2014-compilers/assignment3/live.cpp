// Assignment 3: Live.cpp
// 
//
// Based on code from Todd C. Mowry
// Modified by Arthur Peters
////////////////////////////////////////////////////////////////////////////////

/******************************************************************************
    Implementation details:

    Liveness: OUT[n] = UNION_{s E succ[n]} IN[s]  //meet
              IN[n] = GEN[n] U (OUT[n] - KILL[n]) //transfer function
   
              Flow Direction: Backward
              A BitVector stored at each node for IN and OUT. Bit vector contains an entry for all the values
            
              Boundary Conditions: empty set for flow value. identified by no successors.
            
*********************************************************************************/



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
#include "llvm/ADT/BitVector.h"


#include "dataflow.h"

#include <ostream>
#include <fstream>
#include <iostream>
#include <stdlib.h>

using namespace llvm;

namespace {

    class Live : public FunctionPass, public DataFlow<BitVector>{

        public:
            static char ID;

            /* set forward false in the constructor for liveness */
            Live() : DataFlow<BitVector>(false), FunctionPass(ID) {
            }

            /* domain variable. [all the definitions and function arguments in case of liveness] */
            std::vector<Value*> domain;
            int domainSize;
            int numArgs;
    
/*-----------------------------------implement framework methods-----------------------------------*/
            
            //set the boundary condition for block. exit block in this case.
            //an empty set for all nodes in case of backward analysis.
            //TODO check the validity
            //call the explicit constructor. Since the bitvector has already been initialized
            virtual void setBoundaryCondition(BitVector *blockBoundary) {
                *blockBoundary = BitVector(domainSize, false); 
            }

            //union aka bitwise OR. operator '|=' overriden in BitVector class
            virtual void meetOp(BitVector* lhs, const BitVector* rhs){
                *lhs |= *rhs; 
            }

            //empty set initially. each bit represent a value
            virtual BitVector* initializeFlowValue(BasicBlock& b, SetType setType){ 
                return new BitVector(domainSize, false); 
            }

            virtual BitVector* transferFn(BasicBlock& b) {
                return NULL;
            }

/*------------------------------------------------------------------------------------------------*/

            //check if the instruction is a definition
            bool isDefinition(Instruction *ii) {
                return (!(isa<TerminatorInst>(ii) || isa<StoreInst>(ii) || (isa<CallInst>(ii) && cast<CallInst>(ii)->getCalledFunction()->getReturnType()->isVoidTy())));
            }

            void printDomain(std::vector<Value*> domain){
                errs() << "------------\n";
                for(int i=0; i< domain.size(); i++){
                    errs() << domain[i]->getName() << "    ";
                    errs() << *domain[i] << "\n";
                }
                errs() << "-------------";
            }

            virtual bool runOnFunction(Function &F) {
                domain.clear();
                numArgs = 0;
                for (Function::arg_iterator arg = F.arg_begin(); arg != F.arg_end(); ++arg){
                    numArgs++;
                    domain.push_back(arg);
                }
                for (inst_iterator instruction = inst_begin(F), e = inst_end(F); instruction != e; ++instruction) {
                    if (isDefinition(&*instruction))
                        domain.push_back(&*instruction);
                }
                
                domainSize = domain.size();
                printDomain(domain);
            
 
                DataFlow<BitVector>::runOnFunction(F);
                return false;
            }

            virtual void getAnalysisUsage(AnalysisUsage &AU) const {
                AU.setPreservesAll();
            }

    };

    // LLVM uses the address of this static member to identify the pass, so the
    // initialization value is unimportant.
    char Live::ID = 0;

    // Register this pass to be used by language front ends.
    // This allows this pass to be called using the command:
    //    clang -c -Xclang -load -Xclang ./Live.so loop.c
    static void registerMyPass(const PassManagerBuilder &,
            PassManagerBase &PM) {
        PM.add(new Live());
    }
    RegisterStandardPasses
        RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                registerMyPass);

    // Register the pass name to allow it to be called with opt:
    //    clang -c -emit-llvm loop.c
    //    opt -load ./Live.so -live loop.bc > /dev/null
    // See http://llvm.org/releases/3.4/docs/WritingAnLLVMPass.html#running-a-pass-with-opt for more info.
    RegisterPass<Live> X("live", "liveness pass");

}
