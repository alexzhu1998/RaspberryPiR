# rinternals <- file.path(R.home("include"), "Rinternals.h")
# file.show(rinternals)

dyn.load(paste0(getwd(),"/src/","fibonacci_main.so"))
dyn.load(paste0(getwd(),"/src/","DHTtempsensor_functions.so"))

DHTInitiate <- function() {
    res <- .C("main")
    return(1)
}


