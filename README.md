# RaspberryPiR
This is an R package that can control Raspberry Pi GPIO pins with Shared Memory Functions to write and read streaming data. Streaming analysis code is also provided.

Compile the library using the shell script compile/compileEVERYTHING.sh

## Code that requires compilation:
C++ Source Code: located at src/
- utils/ contains utility functions for wiringPi.h library, sys/shm.h library and RaspiCam library
- remaining source code converts writeMemory, readMemory, freeMemory functions into R functions
R Source Code: located at R/
- contain some simple Linux command lines to examine the state of memory usage.
R Analysis Code located at demo/:
- Code for Online Mean and Variance
- Code for Control Charts
- Code for Online PCA
- Code for Tukey Region
- Code for Bayesian Change point Detection

