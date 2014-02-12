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
#include "llvm/ADT/ValueMap.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/Assembly/Writer.h"
#include "llvm/ADT/DenseMap.h"


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
                bvIndexToInstrArg = new std::vector<Value*>();
                valueToBitVectorIndex = new ValueMap<Value*, int>();
                instrInSet = new ValueMap<Instruction*, BitVector*>();
            }

            /* domain variable. [all the definitions and function arguments in case of liveness] */
            std::vector<Value*> domain;
            std::vector<Value*> *bvIndexToInstrArg; //map values to their bitvector. Each values has it's own bit vector
            ValueMap<Value*, int> *valueToBitVectorIndex;        //map values (args and variables) to their bit vector index
            ValueMap<Instruction*, BitVector*> *instrInSet;     //In set for a instruction inside basic block : lower level of granularity

            int domainSize;
            int numArgs;
            int numInstr;
            /*-----------------------------------implement framework methods-----------------------------------*/
            virtual void printBV(const BitVector *bv) {
                errs() << "{ ";
                for (int i=0; i < bv->size(); i++) {
                    if ( (*bv)[i] ) {
                        WriteAsOperand(errs(), (*bvIndexToInstrArg)[i], false);
                        errs() << " ";
                    }
                }
                errs() << "}\n";
            }


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


            //transfer function:
            //IN[n] = USE[n] U (OUT[n] - DEF[n]) //transfer function
            
            virtual BitVector* transferFn(BasicBlock& bb) {
                BitVector* next = new BitVector(*((*out)[&bb]));
 //               errs() << "----------------"<< bb.getName() <<"---------------\n";
     //           printBV(next);
   //             errs() << "-----------------------------------\n";
                 
                                   
                BitVector* instVec = next; // for empty blocks
                Instruction* inst;
                bool breakme=false;
                // go through instructions in reverse
                BasicBlock::iterator ii = --(bb.end()), ib = bb.begin();
                while (true) {

                    // inherit data from next instruction
                    //                    errs() << "INSTRUCTION " << *ii << "\n";
                    inst = &*ii;
                    instVec = (*instrInSet)[inst];            
                    *instVec = *next;

                    // if this instruction is a new definition, remove it
                    if (isDefinition(inst)){
                        (*instVec)[(*valueToBitVectorIndex)[inst]] = false;
                    }

                    // add the arguments, unless it is a phi node
                    if (!isa<PHINode>(*ii)) {
                        User::op_iterator OI, OE;
                        for (OI = inst->op_begin(), OE=inst->op_end(); OI != OE; ++OI) {
                            if (isa<Instruction>(*OI) || isa<Argument>(*OI)) {
                                (*instVec)[(*valueToBitVectorIndex)[*OI]] = true;
                            }
                        }
                    }else if(isa<PHINode>(*ii)){
                        PHINode* phiNode = cast<PHINode>(&*ii);
                        for (int incomingIdx = 0; incomingIdx < phiNode->getNumIncomingValues(); incomingIdx++) {
                            Value* val = phiNode->getIncomingValue(incomingIdx);
                            if (isa<Instruction>(val) || isa<Argument>(val)) {
                                int valIdx = (*valueToBitVectorIndex)[val];
                                BasicBlock* incomingBlock = phiNode->getIncomingBlock(incomingIdx);
                                if ((*neighbourSpecificValues).find(incomingBlock) == (*neighbourSpecificValues).end())
                                    (*neighbourSpecificValues)[incomingBlock] = new BitVector(domainSize);
                                (*(*neighbourSpecificValues)[incomingBlock]).set(valIdx);                                
                            }
                        }
                    }

                    errs() << *ii << "  " ;
                    printBV(instVec);
                    
                    next = instVec;

                    if (ii == ib) break;
                    
                    --ii;
                }


                /*if(bb.getName() == "entry"){
                   errs() << "FORCONDITION:" ;   
                   printBV(instVec);
                }*/
                // remove the phi nodes from in 
                // remove the phi nodes from in
                //instVec = new BitVector(*instVec);

/*                for (BasicBlock::iterator ii = bb.begin(), ib = bb.end(); ii != ib; ++ii) {
                    if (isa<PHINode>(*ii)) {
                        PHINode* phiNode = cast<PHINode>(&*ii);
                        for (int incomingIdx = 0; incomingIdx < phiNode->getNumIncomingValues(); incomingIdx++) {
                            Value* val = phiNode->getIncomingValue(incomingIdx);
                            if (isa<Instruction>(val) || isa<Argument>(val)) {
                                int valIdx = (*valueToBitVectorIndex)[val];
                                BasicBlock* incomingBlock = phiNode->getIncomingBlock(incomingIdx);
                                (*instVec)[(*valueToBitVectorIndex)[val]] = false;
                            }
                        }
                    }
                }
*/
       //         errs() << "final bit vector: ";
      //          printBV(instVec);

                return instVec;
            }

            bool isDefinition(Instruction *ii) {
                return (!(isa<TerminatorInst>(ii) || isa<StoreInst>(ii) || (isa<CallInst>(ii) && cast<CallInst>(ii)->getCalledFunction()->getReturnType()->isVoidTy())));
            }


            /*-------------------------------------------------------------------------------------------------*/


            void displayResults(Function &F) {
                // iterate over basic blocks
                Function::iterator bi = F.begin(), be = (F.end());
                printBV( (*in)[&*bi] ); // entry node
                for (; bi != be; ) {            
                    errs() << bi->getName() << ":\n"; //Display labels for basic blocks

                    // iterate over remaining instructions except very first one
                    BasicBlock::iterator ii = bi->begin(), ie = (bi->end());
                    errs() << "\t" << *ii << "\n";
                    for (ii++; ii != ie; ii++) {
                        if (!isa<PHINode>(*(ii))) {
                            printBV( (*instrInSet)[&*ii] );
                        }
                        errs() << "\t" << *ii << "\n";
                    }

                    // display in[bb]
                    ++bi;

                    if (bi != be && !isa<PHINode>(*((bi)->begin())))
                        printBV( (*out)[&*bi] );

                    errs() << "\n";
                }
                printBV( (*out)[&*(--bi)] );
            }


            /*------------------------------------------------------------------------------------------------*/

            //check if the instruction is a definition
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
                int index = 0;
                for (Function::arg_iterator arg = F.arg_begin(); arg != F.arg_end(); ++arg){
                    domain.push_back(arg);
                    bvIndexToInstrArg->push_back(&*arg);
                    (*valueToBitVectorIndex)[&*arg] = index;
                    index++;
                }

                for (inst_iterator instruction = inst_begin(F), e = inst_end(F); instruction != e; ++instruction) {
                    //if (isDefinition(&*instruction)){
                    domain.push_back(&*instruction);
                    bvIndexToInstrArg->push_back(&*instruction);
                    (*valueToBitVectorIndex)[&*instruction] = index;
                    index++;
                    //}
                }

                domainSize = domain.size();

                //DEBUG: printing all args and variables.     
                //                printDomain(domain);

                //initialize the IN set set inside the block for each instruction.     
                for (inst_iterator instruction = inst_begin(F), e = inst_end(F); instruction != e; ++instruction) {
                    (*instrInSet)[&*instruction] = new BitVector(domainSize, false); 
                }

                DataFlow<BitVector>::runOnFunction(F); //call the analysis method in dataflow

                displayResults(F);
                return false; //not changing anything
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
