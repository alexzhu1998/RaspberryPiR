# rinternals <- file.path(R.home("include"), "Rinternals.h")
# file.show(rinternals)
sourceCpp(paste0(getwd(),"/src/","DHTtempsensor_functions.cpp"))
# dyn.load(paste0(getwd(),"/src/","DHTtempsensor_functions.so"))

# DHTInitiate <- function() {
#     res <- .C("main")
#     return(1)
# }

app <- new(DHT)
app$humidity
app$temperature
