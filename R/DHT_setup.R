# rinternals <- file.path(R.home("include"), "Rinternals.h")
# file.show(rinternals)

dyn.load(paste0(getwd(),"/src/","DHTtempsensor_functions.so"))

DHTInitiate <- function(delays= 1000) {
    res <- .C("DHTLoop", d=delays)
    return(1)
}


