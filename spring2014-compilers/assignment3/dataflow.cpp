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

namespace {
    //forward indicates the flow direction. FlowValueType indicates the type of flow value. Ex: BitVector used for liveness and reachness.
    template <bool forward, typename FlowValueType>
        class DataFlow{
            public:
                DataFlow() {
                    in  = new InOutMap();
                    out = new InOutMap();
                }   

                //mapping from basicblock to lattice
                typedef ValueMap<BasicBlock*, FlowValueType> InOutMap;    

                //In out sets for a basic block
                InOutMap *in;
                InOutMap *out;
                
        };
}
