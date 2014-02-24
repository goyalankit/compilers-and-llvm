// Assignment 4: LoopICM.cpp
// 
//
// Based on code from Todd C. Mowry
// Modified by Arthur Peters
////////////////////////////////////////////////////////////////////////////////

/******************************************************************************

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
#include "llvm/Analysis/LoopInfo.h"


#include "dataflow.h"

#include <ostream>
#include <fstream>
#include <iostream>
#include <stdlib.h>

using namespace llvm;

namespace {

    class LoopICM : public FunctionPass, public DataFlow<BitVector>, public AssemblyAnnotationWriter{

        public:
            static char ID;

            /* set forward false in the constructor for liveness */
            LoopICM() : DataFlow<BitVector>(true), FunctionPass(ID) {
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
            
            /*------------------ implement framework methods for reaching definitions---------------------------*/

            //Entry block. set the reaching set true for all the arguments.
            virtual void setBoundaryCondition(BitVector *blockBoundary) {
                *blockBoundary = BitVector(domainSize, false); 
                for(int i=0; i<numArgs; i++) (*blockBoundary)[i] = true; //set true for arguments
            }

            //union aka bitwise OR. operator '|=' overriden in BitVector class
            virtual void meetOp(BitVector* lhs, const BitVector* rhs){
                *lhs |= *rhs; 
            }

            //empty set initially. for both IN and OUT
            virtual BitVector* initializeFlowValue(BasicBlock& b, SetType setType){ 
                return new BitVector(domainSize, false); 
            }


            //transfer function:
            //out[n] = gen[n] ∪ (in[n] – kill[n])

            virtual BitVector* transferFn(BasicBlock& bb) {
                BitVector* outNowIn = new BitVector(*((*in)[&bb]));
                                   
                BitVector* immOut = outNowIn; // for empty blocks
                
                //go through instructions in forward direction
                for (BasicBlock::iterator ii = bb.begin(), ie = bb.end(); ii != ie; ii++) {
                    // inherit data from next instruction
                    immOut = (*instrInSet)[&*ii];            
                    *immOut = *outNowIn;

                    // definition generates reachingness
                    if (isDefinition(&*ii)){
                        (*immOut)[(*valueToBitVectorIndex)[&*ii]] = true;
                    }

                    //Asuming phi nodes kill the param instructions.             
                    if(isa<PHINode>(*ii)){
                        PHINode* phiNode = cast<PHINode>(&*ii);
                        for (int incomingIdx = 0; incomingIdx < phiNode->getNumIncomingValues(); incomingIdx++) {
                            Value* val = phiNode->getIncomingValue(incomingIdx);
                            if (isa<Instruction>(val) || isa<Argument>(val)) {
                                (*immOut)[(*valueToBitVectorIndex)[val]] = false;
                            }
                        }
                    }

                    outNowIn = immOut;
                }
            
                //return a new copy
                return new BitVector(*immOut);
            }

            bool isDefinition(Instruction *ii) {
                return !(isa<TerminatorInst>(ii)) ;
            }


            /*-----------------------------  Dominator specific methods ---------------------------------------*/
              
            //taken from : http://163.25.101.87/~yen3/wiki/doku.php?id=llvm:llvm_notes#dominator_tree_transveral
            virtual std::multimap<BasicBlock*, BasicBlock*> parseDominatorTree(Function &F){
                //x dominates y => domRelation[x] = y
                std::multimap<BasicBlock*,BasicBlock*> domRelations;
                
                errs() << "----------- START: Dominator Tree for function " <<  F.getName() << "  --------------\n";
                DominatorTree &DT = getAnalysis<DominatorTree>();
                DT.dump();
                errs() << "----------- END:   Dominator Tree for function " <<  F.getName() << "   --------------\n";

                for(Function::iterator bbi = F.begin(), bbie = F.end(); bbi != bbie; bbi++){
                    BasicBlock *BBI = bbi;
                    for(Function::iterator bbj = F.begin(), bbje = F.end(); bbj != bbje; bbj++){
                        BasicBlock *BBJ = bbj;
                        if(BBI != BBJ && DT.dominates(BBI, BBJ)){
                            errs() << BBI->getName() << " dominates " << BBJ->getName() << "\n";
                            domRelations.insert(std::pair<BasicBlock*, BasicBlock*>(BBI,BBJ));
                        }
                    }
                }
                return domRelations;
            }

            
            /*---------------------------- General Purpose debug methods for printing stuff -----------------------*/

            template <typename T>
            void printMultiMap(T  mymap){                                
                for(typename T::iterator it = mymap.begin(), ite=mymap.end(); it!=ite; ++it){
                    errs() << it->first->getName() << " " << it->second->getName() << "\n";
                }
            }

            /*------------------------------------------- identify invariant ---------------------------------------*/
            //TAG A

            Bitvector identifyInvariants(BasicBlock& currBlk){
              /*

                   for(Function::iterator bbi = F.begin(), bbie = F.end(); bbi != bbie; bbi++){
                    BasicBlock *BBI = bbi;
                    LoopInfo &LI = getAnalysis<LoopInfo>();                    
                    Loop *loop   = LI.getLoopFor(BBI);
                    if(loop!=NULL)
                        loop->dump();
                }

              */

                LoopInfo &LI = getAnalysis<LoopInfo>(*(currBlk->getParent()));
                BasicBlock *blk(currBlk);
                Loop *loop   = LI.getLoopFor(blk);
                BasicBlock *immediateDomBlock;
                
            }

            /*------------------------------- Current Pass Methods -------------------------------------------------*/

            virtual bool runOnFunction(Function &F) {
                domain.clear();
                numArgs = 0;
                int index = 0;
                for (Function::arg_iterator arg = F.arg_begin(); arg != F.arg_end(); ++arg){
                    domain.push_back(arg);
                    bvIndexToInstrArg->push_back(&*arg);
                    (*valueToBitVectorIndex)[&*arg] = index;
                    index++;
                    numArgs++;
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
               
                errs() << "============ START: Dominator Analysis for function " <<  F.getName() << "   =============\n";

                std::multimap<BasicBlock*, BasicBlock*> dominatorRelations = parseDominatorTree(F);                
                //printMultiMap< std::multimap< BasicBlock*, BasicBlock* > >(dominatorRelations);
                
                errs() << "============ END:   Dominator Analysis for function " <<  F.getName() <<"    ==============\n";
//                F.print(errs(), this);
                return false; //not changing anything
            }

            virtual void getAnalysisUsage(AnalysisUsage &AU) const {
                AU.setPreservesAll();
                AU.addRequired<DominatorTree>();
                AU.addRequired<LoopInfo>();
            }

    };

    // LLVM uses the address of this static member to identify the pass, so the
    // initialization value is unimportant.
    char LoopICM::ID = 0;

    // Register this pass to be used by language front ends.
    // This allows this pass to be called using the command:
    //    clang -c -Xclang -load -Xclang ./LoopICM.so loop.c
    static void registerMyPass(const PassManagerBuilder &,
            PassManagerBase &PM) {
        PM.add(new LoopICM());
    }
    RegisterStandardPasses
        RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                registerMyPass);

    // Register the pass name to allow it to be called with opt:
    //    clang -c -emit-llvm loop.c
    //    opt -load ./LoopICM.so -live loop.bc > /dev/null
    // See http://llvm.org/releases/3.4/docs/WritingAnLLVMPass.html#running-a-pass-with-opt for more info.
    RegisterPass<LoopICM> X("licp-pass", "reaching definitions pass");

}
