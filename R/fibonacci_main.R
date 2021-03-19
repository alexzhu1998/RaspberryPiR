# rinternals <- file.path(R.home("include"), "Rinternals.h")
# file.show(rinternals)
dyn.load("RaspberryPiR/src/fibonacci_main.so")

x = 1:3
ret_val = .Call("helloA1")


conv <- function(a,b) .Call("conv",a,b)

a= c(1,2,3,4)
b = c(2,3,4,5)
conv(a,b)
# add <- cfunction(c(a = "integer", b = "integer"), "
#   SEXP result = PROTECT(allocVector(REALSXP, 1));
#   REAL(result)[0] = asReal(a) + asReal(b);
#   UNPROTECT(1);
#
#   return result;
# ")
# add(1, 5)
