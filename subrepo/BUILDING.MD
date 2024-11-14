# Building instructions
**DON'T USE CMAKE DIRECTLY**  
Just run ``make`` and have docker installed. The environment for building will automatically be setup, and the root will be dropped.  
There may be errors in the c++ project but this is a known side-effect of protobuf headers (ver mismatch).