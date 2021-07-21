
writeMemory_nohup<- function() {
    system("nohup Rscript -e 'RaspberryPiR::writeMemory()' &")
}

killWriteMemoryNohupAndFreeMemory<- function() {
    x <- system("pgrep -a R | egrep writeMemory | cut -d' ' -f1", intern= TRUE)
    if (length(x) == 0) {
        print("No writeMemory found")
        return(0)
    }
    x <-strtoi(x)
    stopifnot(!anyNA(x))
    for (i in (1:length(x))) {
        system(paste("kill",x[i]))
    }
    
    system("Rscript -e 'RaspberryPiR::freeMemory()'")
}
