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

    class Live : public FunctionPass, public DataFlow<BitVector>, public AssemblyAnnotationWriter{

        public:
            static char ID;

            /* set forward false in the constructor for liveness */
            Live() : DataFlow<BitVector>(false), FunctionPass(ID) {
                bvIndexToInstrArg = new std::vector<Value*>();
                valueToBitVectorIndex = new ValueMap<Value*, int>();
                instrInSet = new ValueMap<const Instruction*, BitVector*>();
            }

            /* domain variable. [all the definitions and function arguments in case of liveness] */
            std::vector<Value*> domain;
            std::vector<Value*> *bvIndexToInstrArg; //map values to their bitvector. Each values has it's own bit vector
            ValueMap<Value*, int> *valueToBitVectorIndex;        //map values (args and variables) to their bit vector index
            ValueMap<const Instruction*, BitVector*> *instrInSet;     //In set for a instruction inside basic block : lower level of granularity

            int domainSize;
            int numArgs;
            int numInstr;

            /*----------------------------------------implement display methods--------------------------------*/

            virtual void emitBasicBlockStartAnnot(const BasicBlock *bb, formatted_raw_ostream &os) {
                os << "; ";
                if (!isa<PHINode>(*(bb))) {
                    const BitVector *bv = (*in)[&*bb];
                    for (int i=0; i < bv->size(); i++) {
                        if ( (*bv)[i] ) {
                            WriteAsOperand(os, (*bvIndexToInstrArg)[i], false);
                            os << ", ";
                        }
                    }
                }
                os << "\n";
            }

            virtual void emitInstructionAnnot(const Instruction *i, formatted_raw_ostream &os) {
                os << "; ";
                if (!isa<PHINode>(*(i))) {
                    const BitVector *bv = (*instrInSet)[&*i];
                    for (int i=0; i < bv->size(); i++) {
                        if ( (*bv)[i] ) {

                            WriteAsOperand(os, (*bvIndexToInstrArg)[i], false);
                            os << ", ";
                        }
                    }
                }
                os << "\n";
            }

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


            //transfer function:
            //IN[n] = USE[n] U (OUT[n] - DEF[n]) //transfer function
            
            virtual BitVector* transferFn(BasicBlock& bb) {
                BitVector* outNowIn = new BitVector(*((*out)[&bb]));
                                   
                BitVector* immIn = outNowIn; // for empty blocks
                Instruction* tempInst;
                bool breakme=false;
                // go through instructions in reverse
                BasicBlock::iterator ii = --(bb.end()), ib = bb.begin();
                while (true) {

                    // inherit data from next instruction
                    tempInst = &*ii;
                    immIn = (*instrInSet)[tempInst];            
                    *immIn = *outNowIn;

                    // if this instruction is a new definition, remove it
                    if (isDefinition(tempInst)){
                        (*immIn)[(*valueToBitVectorIndex)[tempInst]] = false;
                    }

                    // add the arguments, unless it is a phi node
                    if (!isa<PHINode>(*ii)) {
                        User::op_iterator OI, OE;
                        for (OI = tempInst->op_begin(), OE=tempInst->op_end(); OI != OE; ++OI) {
                            if (isa<Instruction>(*OI) || isa<Argument>(*OI)) {
                                (*immIn)[(*valueToBitVectorIndex)[*OI]] = true;
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

                    outNowIn = immIn;

                    if (ii == ib) break;

                    --ii;
                }

                return immIn;
            }

            bool isDefinition(Instruction *ii) {
                return !(isa<TerminatorInst>(ii)) ;
            }


            /*-------------------------------------------------------------------------------------------------*/


            /*------------------------------------------------------------------------------------------------*/

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
                    domain.push_back(&*instruction);
                    bvIndexToInstrArg->push_back(&*instruction);
                    (*valueToBitVectorIndex)[&*instruction] = index;
                    index++;
                }

                domainSize = domain.size();

                //initialize the IN set set inside the block for each instruction.     
                for (inst_iterator instruction = inst_begin(F), e = inst_end(F); instruction != e; ++instruction) {
                    (*instrInSet)[&*instruction] = new BitVector(domainSize, false); 
                }

                DataFlow<BitVector>::runOnFunction(F); //call the analysis method in dataflow
                F.print(errs(), this);
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
    RegisterPass<Live> X("dce-pass", "liveness pass");

}
