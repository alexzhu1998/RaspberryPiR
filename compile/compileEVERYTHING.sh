echo "Compiling RcppExports"
Rscript compile/compileAttributes.R
echo "Running R CMD INSTALL"
R CMD INSTALL ../RaspberryPiR