
### Highly dependent on structure of arguments of shmem_user.cpp


# writeMemory_nohup<- function(sensor = 'DHT11',pin = 0, w = 1000) {
#     sensor = paste0('"',sensor,'"')
#     system(paste0("nohup Rscript -e 'RaspberryPiR::",sensor,"_writeMemory(,",pin,",",w,")' >/dev/null 2>&1 &"))
# }

# killWriteMemoryNohupAndFreeMemory<- function(sensor = 'DHT11') {
#     sensor = paste0('"',sensor,'"')
#     x <- system("pgrep -a R | egrep writeMemory | cut -d' ' -f1", intern= TRUE)
#     if (length(x) == 0) {
#         print("No writeMemory found")
#         return(0)
#     }
#     x <-strtoi(x)
#     stopifnot(!anyNA(x))
#     for (i in (1:length(x))) {
#         system(paste("kill",x[i]))
#     }
#     print("Success")
#     system(paste0("Rscript -e 'RaspberryPiR::freeMemory(",sensor,")'"))
# }