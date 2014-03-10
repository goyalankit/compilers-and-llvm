// Assignment 4: LoopICM.cpp
// 
//
// Based on code from Todd C. Mowry
// Modified by Arthur Peters
////////////////////////////////////////////////////////////////////////////////

/******************************************************************************

 *********************************************************************************/


#define DEBUG_TYPE "licm-pass" 

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
#include "llvm/ADT/Statistic.h"


#include "dataflow.h"

#include <ostream>
#include <fstream>
#include <iostream>
#include <stdlib.h>

STATISTIC(NumHoisted, "The # of hoists. increaments by 1 for each level hoist.");

using namespace llvm;

namespace {

    class LoopICM : public FunctionPass, public DataFlow<BitVector>{

        public:
            static char ID;

            /* set forward false in the constructor for liveness */
            LoopICM() : DataFlow<BitVector>(true), FunctionPass(ID) {
                bvIndexToInstrArg = new std::vector<Value*>();
                valueToBitVectorIndex = new ValueMap<Value*, int>();
                instrInSet = new ValueMap<const Instruction*, BitVector*>();
                modifed = false;
            }

            /* domain variable. [all the definitions and function arguments in case of liveness] */
            std::vector<Value*> domain;
            std::vector<Value*> *bvIndexToInstrArg; //map values to their bitvector. Each values has it's own bit vector
            ValueMap<Value*, int> *valueToBitVectorIndex;        //map values (args and variables) to their bit vector index
            ValueMap<const Instruction*, BitVector*> *instrInSet;     //In set for a instruction inside basic block : lower level of granularity

            int domainSize;
            int numArgs;
            int numInstr;
            bool modifed;

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

                    outNowIn = immOut;
                }

                //return a new copy
                return new BitVector(*immOut);
            }

            bool isDefinition(Instruction *ii) {
                return !(isa<TerminatorInst>(ii)) ;
            }
            /*---------------------------- General Purpose debug methods for printing stuff -----------------------*/

            template <typename T>
                void printMultiMap(T  mymap){                                
                    for(typename T::iterator it = mymap.begin(), ite=mymap.end(); it!=ite; ++it){
                        errs() << it->first->getName() << " " << it->second->getName() << "\n";
                    }
                }

            void printBv(const BitVector &bv){
                errs() << "\n { ";

                for(ValueMap<Value*, int>::iterator it = valueToBitVectorIndex->begin(), ie = valueToBitVectorIndex->end(); ie!=it; ++it){
                    if(it->second < domainSize)
                        errs() << it->first->getName() << "-" << bv[(it->second)] << ", ";              
                }

                errs() << " }";

                //                errs() << "\n { ";
                for(int i=0;i<domainSize;i++){
                    // errs() << bv[i] << ", ";
                }
                //              errs() << " }";
            }

            /*-----------------------------  Dominator specific methods ---------------------------------------*/


            std::map<BasicBlock*, BasicBlock*> immPreDomMap;
            //create immediate predominator map
            BasicBlock* ProcessDomTree( const DomTreeNode *Node, size_t Level ) {
                if( !Node )
                    return NULL;

                BasicBlock *BB = Node->getBlock();

                // Traverse child nodes
                for( DomTreeNode::const_iterator I = Node->begin(),
                        E = Node->end() ; I != E ; ++I ) {
                    BasicBlock *BC = ProcessDomTree( *I, Level + 1 );
                    if(BC != NULL && BB != NULL)
                        immPreDomMap.insert(std::pair<BasicBlock*, BasicBlock *>(BC, BB));
                }
                return BB;
            }




            /*------------------------------------------- identify invariant ---------------------------------------*/
            //TAG A
            //in and out methods for invariant analysis pass
            //it would be better to use a struct here for each block

            typedef ValueMap<const BasicBlock*, BitVector*> IvBlockInOutMap;    
            IvBlockInOutMap *ivIn;
            IvBlockInOutMap *ivOut;

            void initializeWorklist(Function &func, Worklist &worklist){
                BasicBlock& entry = func.getEntryBlock();
                worklist.push_back(&entry);
            }


            BitVector* isInvariantTransferFunction(BasicBlock &hotBlock){                 
                BitVector* outNowIn = new BitVector(*((*ivIn)[&hotBlock]));
                BitVector* immOut = outNowIn; // for empty blocks

                LoopInfo &LI = getAnalysis<LoopInfo>();
                BasicBlock *blk(&hotBlock);

                Loop *loop   = LI.getLoopFor(blk);

                if(loop == NULL) return immOut;

                /*============RENAME========================*/
                BasicBlock *loopHeader;
                DominatorTree &DT = getAnalysis<DominatorTree>();

                // now we need the dominator
                //                loopHeader =  loop->getLoopPreheader(); // (immPreDomMap)[blk];
                loopHeader =  loop->getHeader(); // (immPreDomMap)[blk];

                assert(loopHeader != NULL);

                errs() << "\nLoop Header: " << loop->getHeader()->getName() << "\n";
                printBv(*((*out)[loop->getLoopPreheader()]));
                errs() << "Loop PreHeader: " << loop->getLoopPreheader()->getName() << "\n";

                // We want the out of the reaching definitions for the dominator block
                //                BitVector rdFromParent =*((*in)[loopHeader]);
                BitVector rdFromParent =*((*out)[loop->getLoopPreheader()]);

                BasicBlock::InstListType &ls(hotBlock.getInstList());

                // iterate the instructions in order
                for(BasicBlock::InstListType::iterator it(ls.begin()), e(ls.end()); it != e; it++)
                {
                    User::op_iterator OI, OE;
                    bool instIsInv = true;


                    if(isa<PHINode>(it) || it->getType()->isVoidTy()) //want to keep these inside
                        continue;

                    for(OI = it->op_begin(), OE = it->op_end(); OI != OE; ++OI)
                    {

                        Value *val = *OI;

                        if(isa<PHINode>(val)){
                            PHINode* phiNode = cast<PHINode>(&*OI);

                            errs() << *phiNode << " encountered " << "\n ";
                            // if(!((rdFromParent)[(*valueToBitVectorIndex)[phiNode]])){

                            //check if it is in loop
                            if((phiNode->getParent() == &hotBlock) || !(rdFromParent)[(*valueToBitVectorIndex)[val]] ){

                                //                            if(phiNode->getParent() == &hotBlock) {
                                errs() << *phiNode << " set to false " << "\n ";
                                instIsInv&=false;
                                //    errs() << *phiNode << " encountered " << "\n ";
                                break;
                            }

                            } 

                            if(isa<Instruction>(val) || isa<Argument>(val)){
                                if((rdFromParent)[(*valueToBitVectorIndex)[val]] || (*immOut)[(*valueToBitVectorIndex)[val]])     
                                    instIsInv&=true;
                                else
                                {
                                    instIsInv&=false;
                                    break;
                                }
                            }else if(isa<Constant>(val)){
                                instIsInv&=true;
                            }
                        }

                        if(instIsInv){
                            //errs() << "We have a candidate " << *it << "\n";
                            (*immOut)[(*valueToBitVectorIndex)[it]] = true;
                        }
                        }

                        return immOut;

                    }

                    //            ValueMap<BasicBlock*, bool> visited;

                    ValueMap<BasicBlock*, bool> *identifyVisited;
                    void traverseInForward(Worklist &w){
                        BasicBlock *hotBlock = *w.begin();
                        w.pop_front();

                        (*identifyVisited)[hotBlock] = true;
                        errs() << "Running for block " << hotBlock->getName() << "\n";

                        int numPred = 0;
                        for (pred_iterator PI = pred_begin(hotBlock), E = pred_end(hotBlock); PI != E; ++PI){
                            ++numPred;
                            if(PI == pred_begin(hotBlock)){
                                *(*ivIn)[hotBlock] = *(*ivOut)[*PI];
                            }else{
                                *(*ivIn)[hotBlock] |= *(*ivOut)[*PI]; //union
                            }
                        }

                        if(numPred == 0)  setBoundaryCondition((*ivIn)[hotBlock]);

                        BitVector *newOut = isInvariantTransferFunction(*hotBlock);

                        bool changed= false;
                        changed = *newOut != *(*ivOut)[hotBlock];

                        if(changed)
                            *(*ivOut)[hotBlock] = *newOut;

                        for (succ_iterator SI = succ_begin(hotBlock), E = succ_end(hotBlock); SI != E; ++SI) {                  
                            if(changed || (*identifyVisited)[*SI] == false) {
                                w.push_back(*SI); 
                            } 
                        }
                    }

                    void identifyInvariants(Function &F){
                        errs() <<  "------------" <<"Running for function: " << F.getName() << "\n";
                        Worklist * worklist = new Worklist();
                        initializeWorklist(F, *worklist);

                        ivIn = new IvBlockInOutMap();
                        ivOut = new IvBlockInOutMap();
                        identifyVisited = new ValueMap<BasicBlock*, bool>();

                        for (Function::iterator bi = F.begin(), be = F.end(); bi != be; bi++) {
                            BasicBlock * bb = &*bi;

                            (*ivIn)[bb] = new BitVector(domainSize, false);    
                            (*ivOut)[bb] = new BitVector(domainSize, false);    
                        }

                        while(!worklist->empty())
                            traverseInForward(*worklist);

                    }

                    /*------------------------------- Current Pass Methods -------------------------------------------------*/
                    //debug method to check dominator relations.
                    void drel (Function &F) {
                        DominatorTree &DT = getAnalysis<DominatorTree>();
                        for(Function::iterator bbi = F.begin(), bbie = F.end(); bbi != bbie; bbi++){
                            BasicBlock *BBI = bbi;
                            for(Function::iterator bbj = F.begin(), bbje = F.end(); bbj != bbje; bbj++){
                                BasicBlock *BBJ = bbj;
                                if(BBI != BBJ && DT.dominates(BBI, BBJ)){
                                    errs() << BBI->getName() << " doms " << BBJ->getName() << "\n";
                                }
                            }
                        }
                    }


                    void dfs(BasicBlock &node, Worklist &worklist, ValueMap<BasicBlock*, bool> &visited){
                        visited[&node] = true;
                        for (succ_iterator SI = succ_begin(&node), E = succ_end(&node); SI != E; ++SI) {
                            if(!visited[*SI]){
                                worklist.push_back(*SI);
                                BasicBlock* B = *SI;
                                dfs(*B, worklist, visited);
                            }
                        }
                    }

                    virtual void dfsres(Function &F, Worklist &worklist){
                        ValueMap<BasicBlock*, bool> visited;
                        worklist.push_back(&F.getEntryBlock());
                        visited[&F.getEntryBlock()] = true;
                        dfs(F.getEntryBlock(), worklist, visited);
                    }


                    bool checkIfStillInvariant(BasicBlock *hotBlock, Instruction *ii){

                        BitVector immOut = *(*ivOut)[hotBlock];
                        LoopInfo &LI = getAnalysis<LoopInfo>();
                        BasicBlock *blk(hotBlock);

                        Loop *loop   = LI.getLoopFor(blk);
                        bool instIsInv = true;

                        User::op_iterator OI, OE;

                        BitVector rdFromParent =*((*out)[loop->getLoopPreheader()]);
                        for(OI = ii->op_begin(), OE = ii->op_end(); OI != OE; ++OI) {

                            Value *val = *OI;

                            if(isa<PHINode>(val)){
                                PHINode* phiNode = cast<PHINode>(&*OI);

                                errs() << *phiNode << " encountered " << "\n ";
                                // if(!((rdFromParent)[(*valueToBitVectorIndex)[phiNode]])){

                                //check if it is in loop
                                if((phiNode->getParent() == hotBlock) || !(rdFromParent)[(*valueToBitVectorIndex)[val]] ){

                                    //                            if(phiNode->getParent() == &hotBlock) {
                                    errs() << *phiNode << " set to false " << "\n ";
                                    instIsInv&=false;
                                    //    errs() << *phiNode << " encountered " << "\n ";
                                    break;
                                }

                                } 

                                if(isa<Instruction>(val) || isa<Argument>(val)){

                                    if((rdFromParent)[(*valueToBitVectorIndex)[val]] || (immOut)[(*valueToBitVectorIndex)[val]])     
                                        instIsInv&=true;
                                    else
                                    {
                                        instIsInv&=false;
                                        break;
                                    }
                                }else if(isa<Constant>(val)){
                                    instIsInv&=true;
                                }
                            }
                            return instIsInv;
                            }


                            ValueMap<Instruction*, Instruction*> to_be_moved;
                            virtual void applyCodeMotion(Worklist &worklist){
                                ValueMap<Instruction*, bool> alreadyHoisted;
                                while(!worklist.empty()){

                                    BasicBlock* CB = worklist.front();
                                    worklist.pop_front();

                                    BitVector ivCurrent = *(*ivOut)[CB];                    

                                    BasicBlock::InstListType &ls(CB->getInstList());

                                    LoopInfo &LI = getAnalysis<LoopInfo>();
                                    Loop *loop   = LI.getLoopFor(CB);

                                    if(loop == NULL) continue;
                                    //                    loop->getExitBlock();
                                    SmallVector<BasicBlock*, 128> exitBlocks;
                                    loop->getUniqueExitBlocks(exitBlocks);

                                    //TODO: make it a global variable
                                    DominatorTree &DT = getAnalysis<DominatorTree>();
                                    for(BasicBlock::InstListType::iterator it(ls.begin()), e(ls.end()); it != e ; )
                                    {
                                        bool canMove = false;
                                        if(it->getType()->isVoidTy()) {
                                            ++it;
                                            continue;
                                        }                                               

                                        Instruction *ii = &*it;

                                        if(ivCurrent[(*valueToBitVectorIndex)[it]]){ //check that the instruction is invariant
                                            canMove = true;
                                            //check that the block dominates all exits.
                                            for(SmallVectorImpl<BasicBlock*>::iterator i = exitBlocks.begin(), e = exitBlocks.end(); i != e; ++i) {
                                                BasicBlock* bb = (*i);
                                                if(DT.dominates(CB,bb)){
                                                    canMove &= true;
                                                }else{
                                                    canMove &= false;
                                                }
                                            }
                                        }

                                        //todo check the instruciton dominate all uses.

                                        for(Value::use_iterator ui = ii->use_begin(), ue = ii->use_end(); ui!=ue; ++ui){
                                            Instruction *vi = dyn_cast<Instruction>(*ui);
                                            if(ii->getName() == "cmp6")
                                                errs() << "USAGE: " << *vi << "  " << DT.dominates(ii,vi) << "\n";
                                            if(!DT.dominates(ii,vi)) {
                                                canMove &= false;
                                            }
                                        }  

                                        //Move the instruction
                                        if(canMove){
                                            Instruction *ti = loop->getLoopPreheader()->getTerminator();
                                            ++it;
                                            if(alreadyHoisted[ii] == true){
                                                if(checkIfStillInvariant(CB, ii) == false){
                                                    continue;
                                                }                               
                                            }

                                            ii->moveBefore(ti);

                                            alreadyHoisted[ii] = true;
                                            worklist.push_front(loop->getLoopPreheader());
                                            errs() << "hoisting " << *ii << " to " << loop->getLoopPreheader()->getName() << "\n";
                                            modifed = true;
                                            ++NumHoisted;
                                        }else{
                                            ++it;
                                        }
                                    }
                                }                  
                            }


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

                                //errs() << "============ START: Getting Immediate Predominators for each basic block " <<  F.getName() << "   =============\n\n";

                                DominatorTree &DT = getAnalysis<DominatorTree>();
                                // DT.dump();
                                ProcessDomTree(DT.getRootNode(), 0);
                                //printMultiMap< std::map< BasicBlock*, BasicBlock* > >(immPreDomMap);
                                //errs() << "============ END: Getting Immediate Predominators for each basic block " <<  F.getName() << "   =============\n\n\n";


                                //errs() << "============ START: Invariant Analysis for function " <<  F.getName() << "   =============\n";

                                identifyInvariants(F);
                                //drel(F); Debug method
                                //errs() << "============ END: Invariant Analysis for function " <<  F.getName() << "   =============\n";

                                Worklist worklist;
                                dfsres(F, worklist);

                                applyCodeMotion(worklist);

                                return modifed; //not changing anything
                            }

                            virtual void getAnalysisUsage(AnalysisUsage &AU) const {
                                AU.setPreservesAll();
                                AU.addRequired<DominatorTree>();
                                AU.addRequired<LoopInfo>();
                            }

                        };

                        char LoopICM::ID = 0;

                        static void registerMyPass(const PassManagerBuilder &,
                                PassManagerBase &PM) {
                            PM.add(new LoopICM());
                        }
                        RegisterStandardPasses
                            RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                                    registerMyPass);

                        RegisterPass<LoopICM> X("licm-pass", "reaching definitions pass");

                    }
