# RaspberryPiR

This package only works for the Raspberry Pi. Currently tested on Raspberry Pi 4B+

Link to download Honours Thesis: https://fnavarro.perso.math.cnrs.fr/files/HonoursThesis.pdf

This is an R package that can control Raspberry Pi GPIO pins with Shared
Memory Functions to write and read streaming data. Streaming analysis
code is also provided.

Please ensure wiringPi.h is installed. Compile the library using the shell script

``` bash
compile/compileEVERYTHING.sh
```

Picture of the structure:
![alt text](https://raw.githubusercontent.com/alexzhu1998/RaspberryPiR/master/pictures/Infrastructure.png)

Picture of the setup:
![alt text](https://raw.githubusercontent.com/alexzhu1998/RaspberryPiR/master/pictures/PictureOfThePi.jpg)

## Code that requires compilation

C++ Source Code: located at `src/`

-   utils/ contains utility functions for wiringPi.h library, sys/shm.h
    library and RaspiCam library

-   remaining source code converts writeMemory, readMemory, freeMemory
    functions into R functions

R Source Code: located at `R/`

-   contain some simple Linux command lines to examine the state of
    memory usage.

R Analysis Code located at `demo/`

-   Code for Online Mean and Variance

-   Code for Control Charts

-   Code for Online PCA

-   Code for Tukey Region

-   Code for Bayesian Change point Detection

Additional Code for Model Assessments can be found in

-   Additional_Code_model_assessment.R

-   Additional_Code_Online_PCA.R
