########### LLVM/Specialisation

Directory info:

##### src

This directory would contain the source code for your LLVM passes. Each person should add separate folders under the 'src' directory. 

##### build

The llvm passes are built as shared objects (.so files). These files should be built by your label in the Makefile under the 'build' directory. The shared objects need to be run with the 'opt' tool

##### Makefile

Single project wide Makefile; use separate labels to build your llvm passes.

##### docs

Add documentation for the code along with run instructions.

