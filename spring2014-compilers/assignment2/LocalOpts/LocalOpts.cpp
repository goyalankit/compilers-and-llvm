// 15-745 S13 Assignment 2: LocalOpts.cpp
// 
// Based on code from Todd C. Mowry
// Modified by Arthur Peters
// Modified by Ankit Goyal
////////////////////////////////////////////////////////////////////////////////

#define DEBUG_TYPE "my-local-opts" 

#include "llvm/Pass.h"
#include "llvm/PassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/DebugInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/DebugInfo.h"
#include "llvm/Assembly/AssemblyAnnotationWriter.h"
#include "llvm/IR/Constants.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/InstVisitor.h"

#include <ostream>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include "generic_methods.h"
//#include "my_arithmatic.h"

STATISTIC(NumXForms, "The # of algebric identies transformations performed");
STATISTIC(NumConstFolds, "The # of constant folding operations");
STATISTIC(NumStrReductions, "The # of strength reductions");

using namespace llvm;

namespace {


    template<typename APType, typename ConstantType>
        Value * algIdentityAS(Instruction &i, APType generalZeroOne, int operation){
            errs() << i << "\n";
            Value *operand1 = i.getOperand(0);
            Value *operand2 = i.getOperand(1);
            /* ConstantInt and ConstantFP. http://llvm.org/docs/doxygen/html/Constants_8h.html */
            /* Constant types */
            ConstantType *cint; 

            if(cint = dyn_cast<ConstantType>(operand2)){
                if(id_compare<ConstantType, APType>(*cint, generalZeroOne))
                    return operand1;
            }
            else if((cint = dyn_cast<ConstantType>(operand1)) && operation!=1){
                if(id_compare<ConstantType, APType>(*cint, generalZeroOne))
                    return operand2;
            }
            return NULL;
        }



    //TODO Refactor this thing!
    template<typename APType, typename ConstantType>
        Value * algIdentityMD(Instruction &i, APType generalZeroOne, int operation, int identity){ //0: Multiplication. 1 Division
            errs() << i << "\n";
            Value *operand1 = i.getOperand(0);
            Value *operand2 = i.getOperand(1);
            /* ConstantInt and ConstantFP. http://llvm.org/docs/doxygen/html/Constants_8h.html */
            /* Constant types */
            ConstantType *cint; 
            switch(operation){
                case 0:{ //multiplication
                           if(identity == 0){
                               if(cint = dyn_cast<ConstantType>(operand2)){ //operand 2 is 0 and it's multiplication
                                   if(id_compare<ConstantType, APType>(*cint, generalZeroOne)) 
                                       return ConstantType::get(cint->getContext(), generalZeroOne);
                               }
                               else if(cint = dyn_cast<ConstantType>(operand1)){ //operand 1 is 0 and it's multiplication
                                   if(id_compare<ConstantType, APType>(*cint, generalZeroOne))
                                       return ConstantType::get(cint->getContext(), generalZeroOne);
                               }
                           }else if(identity == 1){
                               if(cint = dyn_cast<ConstantType>(operand2)){ //operand 2 is 1 and it's multiplication
                                   if(id_compare<ConstantType, APType>(*cint, generalZeroOne)) 
                                       return cint;
                               }
                               else if(cint = dyn_cast<ConstantType>(operand1)){ //operand 1 is 0 and it's multiplication
                                   if(id_compare<ConstantType, APType>(*cint, generalZeroOne))
                                       return cint;
                               }

                           }
                           break;
                       }
                case 1:{ //division
                           if(identity == 0){
                               if(cint = dyn_cast<ConstantType>(operand1)){ //operand 1 is 0 and it's division 0 / X = 0
                                   if(id_compare<ConstantType, APType>(*cint, generalZeroOne))
                                       return ConstantType::get(cint->getContext(), generalZeroOne);
                               }
                           }else if(identity == 1){
                               if(cint = dyn_cast<ConstantType>(operand2)){ //operand 2 is 1 and it's division X / 1 = X
                                   if(id_compare<ConstantType, APType>(*cint, generalZeroOne)) 
                                       return cint; //if operand 2 is 1 return X
                               }
                           }
                           break;
                       }
            }

            return NULL;
        }


    void makeTheChanges(BasicBlock::iterator &ib, Value* val){
        Instruction* bb = ib;
        ReplaceInstWithValue(bb->getParent()->getInstList(), ib,val);
    }

    void reduce_strength(BasicBlock::iterator &ib, ConstantInt &cint, Value &other, unsigned opcode){
        ++NumStrReductions;
        APInt apint = cint.getValue();
        unsigned reduction_factor = apint.logBase2();
        BinaryOperator* shift_instruction ;
        if(opcode == Instruction::Mul)
            shift_instruction = BinaryOperator::Create( Instruction::Shl, &other, ConstantInt::get(cint.getType(), reduction_factor, false));
        else
            shift_instruction = BinaryOperator::Create( Instruction::AShr, &other, ConstantInt::get(cint.getType(), reduction_factor, false));
        Instruction *ii= ib;
        ii->getParent()->getInstList().insertAfter(ib, shift_instruction);
        makeTheChanges(ib, shift_instruction);
    }
    bool can_reduce_strength(ConstantInt& tcint){
        APInt op1 = tcint.getValue();
        if(op1.isPowerOf2()) return true;
        return false;
    }


    void optimize(Instruction *ii, BasicBlock::iterator& i, Type *instructionType){
        Value *v;
        IntegerType *intype;
        intype = dyn_cast<IntegerType>(instructionType);

        switch(ii->getOpcode()){
            case Instruction::Add: 
                v = algIdentityAS<APInt, ConstantInt>(*ii, getZeroOne<APInt>(intype->getBitWidth(),0),0);
                if(v) { 
                    makeTheChanges(i, v);                                             
                    ++NumXForms;
                }

                break; //X+0 = 0+X = X  
            case Instruction::Sub: 
                v = algIdentityAS<APInt, ConstantInt>(*ii, getZeroOne<APInt>(intype->getBitWidth(),0), 1);  
                if(v) { 
                    makeTheChanges(i, v); 
                    ++NumXForms;
                }
                break; //X-0 = X;
            case Instruction::Mul:{
                                      v = algIdentityMD<APInt, ConstantInt>(*ii, getZeroOne<APInt>(intype->getBitWidth(),1), 0, 1);  //X * 1 = X , 1 * X = X; operation = 0 identity = 1
                                      if(v) { 
                                          makeTheChanges(i, v); 
                                          ++NumXForms;

                                      }

                                      else if (v = algIdentityMD<APInt, ConstantInt>(*ii, getZeroOne<APInt>(intype->getBitWidth(),0), 0, 0)){ //X * 0 = 0, 0 * X = 0 OP=0 ID = 0
                                          makeTheChanges(i, v); 
                                          ++NumXForms;
                                      }
                                      break; //X*1 = X; 1*X=X
                                  }

            case Instruction::UDiv:
            case Instruction::SDiv:{
                                       if(v = algIdentityMD<APInt, ConstantInt>(*ii, getZeroOne<APInt>(intype->getBitWidth(),1),1,1))// X / 1 = X; OP=1 ID=1
                                       {
                                           makeTheChanges(i, v); 
                                           ++NumXForms;
                                       }
                                       else if(v = algIdentityMD<APInt, ConstantInt>(*ii, getZeroOne<APInt>(intype->getBitWidth(),0),1,0)) // X / 1 = X OP=1 ID=0
                                       {      
                                           makeTheChanges(i,v); 
                                           ++NumXForms;
                                       }
                                       break;
                                   }
            default: break;
        }               

        //Constant Folding
        if(ii->getNumOperands() == 2 && isa<Constant>(ii->getOperand(0)) && isa<Constant>(ii->getOperand(1))){
            //           v = fold_constants(op, ii->getOperand(0), ii->getOperand(1));
            //         makeTheChanges(i,v);
        }

        //strength reduction

        switch(ii->getOpcode()){
            case Instruction::UDiv:
            case Instruction::SDiv:
            case Instruction::Mul:{
                                      ConstantInt *tcint;
                                      Value *other;
                                      if((tcint = dyn_cast<ConstantInt>(ii->getOperand(0))) && can_reduce_strength(*tcint)){
                                          other = ii->getOperand(0);
                                          reduce_strength(i, *tcint, *other,ii->getOpcode());
                                      }              
                                      else if((tcint = dyn_cast<ConstantInt>(ii->getOperand(1))) && can_reduce_strength(*tcint)){
                                          other = ii->getOperand(0);
                                          reduce_strength(i, *tcint, *other,ii->getOpcode());
                                      }
                                      break;
                                  }
            default: break;
        }

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
                    optimize(ii,i,i->getType());
                }
            }

            virtual bool runOnFunction(Function &func){
                errs() << "Function: " << func.getName() << "\n";
                for (Function::iterator i = func.begin(), e = func.end(); i != e; ++i){ //iterating over the basic blocks                    
                    runOnBasicBlock(*i);
                    //print_basic_block_info(*i);
                }
                return true;
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
