source("R/DHT_setup.R")

# res <- DHTInitiate()
# print(res)
temp = c()
humid = c()
sampTime = 0
rD = 100
for (count in 1:10) {
    print(paste("Current Counter",count))
    start = Sys.time()
    res <- DHTCFunction(sampTime,rD)
    end = Sys.time()
    print(end-start)
    print(paste("Temperature",res$tempOut))
    print(paste("Humidity",res$humidOut))

}

# print(store)



