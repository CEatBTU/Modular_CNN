## Modular CNN
This work presents a C++-basedsoftware library of reusable modules to build arbitrary CNNs that sup-port High-Level-Synthesis to be implemented as FPGA hardware acceler-ators for the inference process. Our work demonstrates how parametriza-tion  and  modularization  of  basic  building  blocks  of  a  CNN  enable  aneasier customization of the hardware to match the software model. Thisproject also works with low-precision parameters throughout the CNN,in order to provide a more resource-efficient implementation
## Instructions
As of now, there are only the HLS soruces for each layer
1. Create a Viviado HLS project, add core.cpp and core.h as sources. (check the main function name of core.cpp in each layer to know which name to put in as 'top layer')
2. add test_core.cpp, testUtils.cpp, and testUtils.h as testbench.
3. Modify the parameters in core.h according to each layer needs (i.e. number of kernels, size of kernels, etc.)
4. Compile it, test it if needed with a testbench.
5. Sintetize it and export it as IP
6. Add in Vivado as IP block.

## Published paper
16th Arc 2020, link coming soon
