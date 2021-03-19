# rinternals <- file.path(R.home("include"), "Rinternals.h")
# file.show(rinternals)

dyn.load(paste0(getwd(),"/../src/","fibonacci_main.so"))

# wiringPiSetup <- function() {
#     res <- .C("helloWorld", greeting="")
#     result <- .C("blink", num=1000)
#     return(res$greeting)
# }


setPin <- function(extension,iotype) {
    res <- .C("setPin",e=extension,type=iotype)
    return(res$e)
}

wiringPiSetup <- function() {
    res <- .C("setup")
    return(1)
}

destroy <- function(extension) {
    res <- .C("destroy",e=extension)
}
# source(paste0(getwd(),"/RaspberryPiR/R/fibonacci_main.R"))
