template<typename FlowValueType>
bool DataFlow<FlowValueType>::runOnFunction(Function &F){
    BasicBlock& blockBoundary = F.getEntryBlock();

//        errs() << "\nAdded to Worklist : " <<blockBoundary.getName();
    for (Function::iterator bi = F.begin(), be = F.end(); bi != be; bi++) {
        BasicBlock * bb = &*bi;

        //set the appropriate values for forward and backward flow
        //(*in)[bb] -> bit vector::lattice for that block
        (*in)[bb] = initializeFlowValue(*bi, IN);
        (*out)[bb] = initializeFlowValue(*bi, OUT);
    }
    
    Worklist *worklist = new Worklist();
    DataFlow<FlowValueType>::initializeWorklist(F,worklist);
  
    //DEBUG(errs() << "Entry size: " << blockBoundary.size() << ";");
}

template<typename FlowValueType>
void DataFlow<FlowValueType>::initializeWorklist(Function &F, Worklist &worklist){
    
}

void performBackwardAnalysis(){

}
