echo "Compiling RcppExports"
Rscript compile/compileAttributes.R
echo "Running R CMD INSTALL"
R CMD INSTALL ../RaspberryPiR
rm -r src/utils/*.o
rm -r src/*.o
rm src/RaspberryPiR.so