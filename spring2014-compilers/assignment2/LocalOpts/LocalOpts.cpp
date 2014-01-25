// 15-745 S13 Assignment 2: LocalOpts.cpp
// 
// Based on code from Todd C. Mowry
// Modified by Arthur Peters
// Modified by Ankit Goyal
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
#include "llvm/IR/Constants.h"

#include <ostream>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include "generic_methods.h"
using namespace llvm;

namespace {

    template<typename APType, typename ConstantType>
        Value * algIdentity(Instruction &i, APType generalZero){
            errs() << i << "\n";
            Value *operand1 = i.getOperand(0);
            Value *operand2 = i.getOperand(1);
            /* ConstantInt and ConstantFP. http://llvm.org/docs/doxygen/html/Constants_8h.html */
            /* Constant types */
            ConstantType *cint; 
            if(cint = dyn_cast<ConstantType>(operand2)){
                if(id_compare<ConstantType, APType>(*cint, generalZero))
                    return cint;
            }
            else if(cint = dyn_cast<ConstantType>(operand1)){
                if(id_compare<ConstantType, APType>(*cint, generalZero))
                    return cint;
            }
            return NULL;
        }

    void makeTheChanges(BasicBlock::iterator &i, Value* val){
        i->replaceAllUsesWith(val);     //replace all uses of the registers with the value
        BasicBlock::iterator j = i;     //record the location of the instruction
        ++i;                            //increment the pointer
        j->eraseFromParent();           //delete the instruction from the basic block
        --i;                            //rollback to the previous statement.
    }
    
    void tryAlgebraicIdentities(Instruction *ii, BasicBlock::iterator& i, Type *instructionType){
        Value *v;
        IntegerType *intype;
        intype = dyn_cast<IntegerType>(instructionType);

        switch(ii->getOpcode()){
            case Instruction::Add: 
                v = algIdentity<APInt, ConstantInt>(*ii, getZeroOne<APInt>(intype->getBitWidth(),0));
                if(v) makeTheChanges(i, v);
                break; //X+0 = 0+X = X  
            case Instruction::FAdd:
                v = algIdentity<APFloat, ConstantFP>(*ii, getZeroOne<APFloat>(0,0)); 
                if(v) makeTheChanges(i, v);
                break; //X+0 = 0+X = X
            case Instruction::Sub: 
                v = algIdentity<APInt, ConstantInt>(*ii, getZeroOne<APInt>(intype->getBitWidth(),0));  
                if(v) makeTheChanges(i, v);
                break; //X-0 = X; 0-X=-X
                //if(ii->getOpcode() == Instruction::Add)
                //errs() << *ii << "  --- " << ii->getOpcode() << "\n";
        }               
    }

    void tryConstantFolding(Instruction *ii, Value *bb_it){
        errs() << "CONSTANT: " << *ii << "\n";
        if(ii->getNumOperands() == 2 && isa<Constant>(ii->getOperand(0)) && isa<Constant>(ii->getOperand(1)))
            errs() << *ii << "\n" << "constant folding  here\n";
    }


    class LocalOpts : public FunctionPass{

        public:
            static char ID;
            LocalOpts() : FunctionPass(ID) { } //default constructor
            ~LocalOpts() {  }

            /* Method to print details for a all the functions in a module */
            void printFunctionDetails(Function &F) {
                int arg_size = F.arg_size();
                int num_call_sites = F.getNumUses();
                int num_basic_blocks = F.size(); //defined in value class.
                /*count the number of instructions.*/
                int number_of_instructions = 0;
                for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I)
                    number_of_instructions += 1;

                errs() << F.getName() <<": arguments=" << arg_size << " call sites=" <<  num_call_sites << " basic blocks=" << num_basic_blocks << " instructions=" << number_of_instructions << "\n\n";
            }



            void runOnBasicBlock(BasicBlock &blk){
                for (BasicBlock::iterator i = blk.begin(), e = blk.end(); i != e; ++i){
                    Instruction *ii= dyn_cast<Instruction>(i);
                    tryAlgebraicIdentities(ii,i,i->getType());
                    //tryConstantFolding(ii,i);
                }
            }

            virtual bool runOnFunction(Function &func){
                errs() << "Function: " << func.getName() << "\n";
                for (Function::iterator i = func.begin(), e = func.end(); i != e; ++i){ //iterating over the basic blocks                    
                    runOnBasicBlock(*i);
                    //print_basic_block_info(*i);
                }
                return false;
            }


            // We don't modify the program, so we preserve all analyses
            virtual void getAnalysisUsage(AnalysisUsage &AU) const {
                AU.setPreservesAll();
            }

        private:
            //helper functions
            void print_basic_block_info(BasicBlock &b){
                errs() << "Basic block (name=" << b.getName() << ") has " << b.size() << " instructions.\n";

            }
    };

    char LocalOpts::ID = 0;

    static void registerMyPass(const PassManagerBuilder &,
            PassManagerBase &PM) {
        PM.add(new LocalOpts());
    }
    RegisterStandardPasses
        RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                registerMyPass);

    RegisterPass<LocalOpts> X("my-local-opts", "Local Optimizations");

}
