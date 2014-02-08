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

#include <ostream>

using namespace llvm;

#ifndef __DATAFLOW_H__
#define __DATAFLOW_H__

namespace {
    //forward indicates the flow direction. FlowValueType indicates the type of flow value. Ex: BitVector used for liveness and reachness.
    template <typename FlowValueType>
        class DataFlow{
            bool forward;
            public:
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

                void runOnFunction();
            protected:
                //get the boundary conditions for entry nodes
                virtual void setBoundaryCondition(FlowValueType*) = 0;
                //meet operator. could be union or intersection based on the FlowValueType
                virtual void meetOp(FlowValueType* lhs, const FlowValueType* rhs) = 0;
                virtual FlowValueType* initializeFlowValue(BasicBlock& b) = 0;
                virtual FlowValueType* transferFn(BasicBlock& b) = 0;

        };
}

#endif

#include "dataflow.cpp"
