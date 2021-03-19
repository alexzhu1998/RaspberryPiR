# rinternals <- file.path(R.home("include"), "Rinternals.h")
# file.show(rinternals)

dyn.load(paste0(getwd(),"../src/","fibonacci_main.so"))

wiringPiSetup <- function() {
  result <- .C("blink", greeting="")
  return(result$greeting)
}


# source(paste0(getwd(),"/RaspberryPiR/R/fibonacci_main.R"))
