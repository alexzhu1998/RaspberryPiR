source("R/DHT_setup.R")

# res <- DHTInitiate()
# print(res)
temp = c()
humid = c()
for (count in 1:3) {
    print(paste("Current Counter",count))
    # print("Call DHT function")
    res <- DHTCFunction()
    # print("res received")
    # temp = c(temp,res$tempOut)
    # humid = c(humid,res$humidOut)
    print(paste("Temperature",res$tempOut))
    print(paste("Humidity",res$humidOut))

}

# print(store)



