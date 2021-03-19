# rinternals <- file.path(R.home("include"), "Rinternals.h")
# file.show(rinternals)
dyn.load("fibonacci_main.so")

wiringPiSetup <- function() {
  result <- .C("blink", greeting="")
  return(result$greeting)
}
