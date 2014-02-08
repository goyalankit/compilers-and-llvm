template<typename FlowValueType>

void DataFlow<FlowValueType>::runOnFunction(){
    errs() << "hello" << "\n";
    setBoundaryCondition(NULL);    
}
