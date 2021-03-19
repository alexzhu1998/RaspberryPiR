# rinternals <- file.path(R.home("include"), "Rinternals.h")
# file.show(rinternals)

dyn.load(paste0(getwd(),"/../src/","DHT.so"))

DHT <- function() {
    res <- .C("main")
    return(1)
}



