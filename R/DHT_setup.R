# rinternals <- file.path(R.home("include"), "Rinternals.h")
# file.show(rinternals)

dyn.load(paste0(getwd(),"/src/","DHTtempsensor_functions.so"))

DHTInitiate <- function(d) {
    res <- .C("DHTLoop", delays=d)
    return(1)
}


