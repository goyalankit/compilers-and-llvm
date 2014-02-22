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
#include "llvm/IR/Constants.h"


#include "dataflow.h"

#include <ostream>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <queue>
#include <set>

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
                delete_queue = new std::queue<Instruction*>();
            }

            /* domain variable. [all the definitions and function arguments in case of liveness] */
            std::vector<Value*> domain;
            std::vector<Value*> *bvIndexToInstrArg; //map values to their bitvector. Each values has it's own bit vector
            ValueMap<Value*, int> *valueToBitVectorIndex;        //map values (args and variables) to their bit vector index
            ValueMap<const Instruction*, BitVector*> *instrInSet;     //In set for a instruction inside basic block : lower level of granularity
            std::queue<Instruction*> * delete_queue;

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

            //at boundary nobody can be alive
            virtual void setBoundaryCondition(BitVector *blockBoundary) {
                *blockBoundary = BitVector(domainSize, true); 
            }

            //take the intersection. It's a must analysis. we are removing stuff here, better be double sure.
            virtual void meetOp(BitVector* lhs, const BitVector* rhs){
                *lhs &= *rhs; 
            }

            //be optimistic. Assume variable is dead initially
            virtual BitVector* initializeFlowValue(BasicBlock& b, SetType setType){ 
                return new BitVector(domainSize, true); 
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

                    if (!isDefinition(tempInst) || !(*immIn)[(*valueToBitVectorIndex)[tempInst]]) 
                    {
                        User::op_iterator OI, OE;
                        for (OI = tempInst->op_begin(), OE=tempInst->op_end(); OI != OE; ++OI) 
                        {
                            if (isa<Instruction>(*OI) || isa<Argument>(*OI)) 
                            {
                                (*immIn)[(*valueToBitVectorIndex)[*OI]] = false;
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

            
            /*
            bool removeTheDead(Function &F){
                BitVector *killed = (*in)[&(F.getEntryBlock())];
                printBv(*killed);

                std::queue<Instruction*> *delete_queue;

            
//                for(inst_iterator i = inst_begin(F), ie = inst_end(F); i != ie; i++){
                for(inst_iterator i = inst_begin(F), ie = inst_end(F); i != ie;){
//                while(!worklist->empty()){
                   Instruction *I = &*i;
                   inst_iterator next(i);
                   ++next;
                   if(isDefinition(I) && (*killed)[(*valueToBitVectorIndex)[I]]){
                       (*instrInSet).erase(I);
                       I->replaceAllUsesWith(UndefValue::get(I->getType()));
                       I->removeFromParent();
                       delete_queue->push_back(I);
                   }
                    i = next;
                }

//                while(!delete_queue->empty()){
                    //Instruction *i(delete_queue->back());
                    //delete_queue->pop_back();
                    //errs() << "Deleting instruction" << *i;
                    //delete i; 
  //              }
                
                return true;
            }
*/




            /*------------------------------------------------------------------------------------------------*/


            void deleteDeadCodeBlock(BasicBlock *blk, BitVector& killed, std::queue<Instruction*>& to_delete)
            {
                BasicBlock::InstListType &ls(blk->getInstList());
                typedef BasicBlock::InstListType::iterator iter;
     
                for(iter it(ls.begin()), e(ls.end()); it != e;) {
                    Instruction& i(*it);
        
                    iter next(it);
                    ++next; 
                    
                    
                    if(isDefinition(&i) && (killed)[(*valueToBitVectorIndex)[&i]]){
                        (*instrInSet).erase(&i);
                        i.replaceAllUsesWith(UndefValue::get(i.getType())); 
                        i.removeFromParent();
                        to_delete.push(&i); 
                    }

                    it = next;
                }
            }


            bool removeTheDead(Function &fun){
                BitVector *killed = (*in)[&(fun.getEntryBlock())];


                std::queue<BasicBlock*> work_list;
                std::set<BasicBlock*> visited;

                for(Function::iterator i(fun.begin()), e = fun.end(); i != e; ++i) {
                    if(isa<ReturnInst>(i->getTerminator())) {
                        BasicBlock *blk(&*i);
                        work_list.push(blk);
                        visited.insert(blk);
                    }
                }

               std::queue<Instruction*> to_delete;

               while (!work_list.empty()) {
                   BasicBlock *blk(work_list.front());
                   work_list.pop();

                   deleteDeadCodeBlock(blk, *killed, to_delete);

                   for(pred_iterator pi(pred_begin(blk)), e(pred_end(blk)); pi != e; pi++) {
                       BasicBlock *next(*pi);
                       std::set<BasicBlock*>::const_iterator f(visited.find(next));

                       if(f == visited.end()) {
                           work_list.push(next);
                           visited.insert(next);
                       }
                   }
               }

               // now delete everything there is to delete
               while(!to_delete.empty()) {
                   Instruction *i(to_delete.front());
                   to_delete.pop();

                   delete i;
               } 

            }





            /*-------------------------------------------------------------------------------------------------*/

            void printBv(const BitVector &bv){
                errs() << "\n { ";
                for(int i=0;i<domainSize;i++){
                    errs() << bv[0] << ", ";
                }
                errs() << " }";
            }

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
                    if (isDefinition(&*instruction)) {
                        domain.push_back(&*instruction);
                        bvIndexToInstrArg->push_back(&*instruction);
                        (*valueToBitVectorIndex)[&*instruction] = index;
                        index++;
                    }
                }

                domainSize = domain.size();

                //initialize the IN set set inside the block for each instruction.     
                for (inst_iterator instruction = inst_begin(F), e = inst_end(F); instruction != e; ++instruction) {
                    (*instrInSet)[&*instruction] = new BitVector(domainSize, true); 
                }

                DataFlow<BitVector>::runOnFunction(F); //call the analysis method in dataflow
                F.print(errs(), this);
                removeTheDead(F);
                return true; //not changing anything
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
