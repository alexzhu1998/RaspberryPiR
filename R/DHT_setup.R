# rinternals <- file.path(R.home("include"), "Rinternals.h")
# file.show(rinternals)
# sourceCpp(paste0(getwd(),"/src/","DHTtempsensor_functions.cpp"))


# DHTInitiate <- function() {
#     res <- .C("main",out = 0)
#     return(res$out)
# }

DHTCFunction <- function() {
    dyn.load(paste0(getwd(),"/src/","DHTtempsensor_functions.so"))
    res <- .C("myDHT", tempOut = 0, humidOut = 0, samplingTime = 0, readDelay = 100)
    return(res)
}


# app <- new(DHT)
# app$humidity
# app$temperature
