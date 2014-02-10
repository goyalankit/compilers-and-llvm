// Assignment 3: FunctionInfo.cpp
// 
// Based on code from Todd C. Mowry
// Modified by Arthur Peters
// Modified by Ankit Goyal
////////////////////////////////////////////////////////////////////////////////

#include "llvm/Pass.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/ValueMap.h"
#include "llvm/Support/CFG.h"

#include <ostream>
#include <list>

using namespace llvm;

#ifndef __DATAFLOW_H__
#define __DATAFLOW_H__

namespace {
    //forward indicates the flow direction. FlowValueType indicates the type of flow value. Ex: BitVector used for liveness and reachness.

    typedef std::list<BasicBlock*> Worklist;

    template <typename FlowValueType>
        class DataFlow{
            bool forward;
            public:
            enum SetType {
                IN,
                OUT
            };

            DataFlow(bool forward) {
                this->forward = forward;  
                in  = new BlockInOutMap();
                out = new BlockInOutMap();
            }   

            //mapping from basicblock to lattice
            typedef ValueMap<BasicBlock*, FlowValueType*> BlockInOutMap;    
            //In out sets for a basic block
            BlockInOutMap *in;
            BlockInOutMap *out;
            
            void performBackwardAnalysis(Worklist &w){
                BasicBlock *hotBlock = *w.begin();
                w.pop_front();
                //out of this basic block is equivalent to in of it's successor
                //OUT = Union IN
                int numSucc = 0; //to check for the exit node
                
                for (succ_iterator SI = succ_begin(hotBlock), E = succ_end(hotBlock); SI != E; ++SI) {
                    numSucc++;
                    if(SI == succ_begin(hotBlock)){
                        //call the copy constructor on first block
                        (*out)[hotBlock] = (*in)[*SI];
                        continue;
                    }else{
                        //call the meet operator
                        meetOp((*out)[hotBlock], (*in)[*SI]);
                    }
                }

                if(numSucc == 0) setBoundaryCondition((*out)[hotBlock]); //set boundary condition for the exit node.

                FlowValueType *newIn = transferFn(*hotBlock);
                if(*newIn != *(*in)[hotBlock] ){
                    *(*in)[hotBlock] = *newIn;

                    for (pred_iterator PI = pred_begin(hotBlock), E = pred_end(hotBlock); PI != E; ++PI) {
                        w.push_back(*PI); 
                    } 
                }
            }


            void initializeWorklist(Function &func, Worklist &worklist){
                /*insert all the block at first*/
                for (Function::iterator i = func.begin(), e = func.end(); i != e; ++i){
                    BasicBlock *bb = &*i;
                    worklist.push_back(bb);
                }
                if(forward) worklist.reverse();
            }

            bool runOnFunction(Function &F){
                BasicBlock& blockBoundary = F.getEntryBlock();

                for (Function::iterator bi = F.begin(), be = F.end(); bi != be; bi++) {
                    BasicBlock * bb = &*bi;

                    //set the appropriate values for forward and backward flow
                    //(*in)[bb] -> bit vector::lattice for that block
                    (*in)[bb] = initializeFlowValue(*bi, IN);
                    (*out)[bb] = initializeFlowValue(*bi, OUT);
                }

                Worklist *worklist = new Worklist();
                initializeWorklist(F,*worklist);
                performBackwardAnalysis(*worklist);
            }



            protected:
            //get the boundary conditions for entry nodes
            virtual void setBoundaryCondition(FlowValueType*) = 0;
            //meet operator. could be union or intersection based on the FlowValueType
            virtual void meetOp(FlowValueType* lhs, const FlowValueType* rhs) = 0;
            //initial flow values for IN and OUT sets. SetType: IN or OUT
            virtual FlowValueType* initializeFlowValue(BasicBlock& b, SetType setType) = 0;
            //transer function
            virtual FlowValueType* transferFn(BasicBlock& b) = 0;
        };


    
}

#endif

//#include "dataflow.cpp"
