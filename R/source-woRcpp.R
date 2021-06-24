file = paste0(getwd(),"/src/RaspberryPiR.so")
print(file)
simpleDHTFunc <- function(sampTime = 0) {
    # file = paste0(getwd(),"/src/simpledht11.so")
    dyn.load(file)
    res <- .C("dht")
    dyn.unload(file)
    return(sampTime)
}

writer <- function(x = 0) {
    dyn.load(file)
    res <- .C("write")
    dyn.unload(file)
    return(x)
}

reader <- function(x = 0) {
    # file = paste0(getwd(),"/src/read.so")
    dyn.load(file)
    res <- .C("read")
    dyn.unload(file)
    return(x)
}

free <- function(x = 0) {
    # file = paste0(getwd(),"/src/free.so")
    dyn.load(file)
    res <- .C("free")
    dyn.unload(file)
    return(x)
}


